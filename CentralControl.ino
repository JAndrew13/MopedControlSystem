#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <splash.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define WHITE SSD1306_WHITE // Default solid color
#define BLACK SSD1306_BLACK // Default negative space

const int thermistorPin = A0;
const int headlightPin = 2;
const int speedometerPin = A2;

void setup() {
  Serial.begin(9600);
  // Initialize with the I2C addr 0x3C (for the 128x64)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  pinMode(headlightPin, INPUT);
  display.clearDisplay();
  drawStaticElements();
  display.display();
}

void loop() {
  int headlightStatus = digitalRead(headlightPin); // Read the headlight status as digital
  int speed = 00;
  

  // temporary fuel testing
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  int fuelLevel = map(sensorValue, 0, 1023, 1, 5);
  Serial.println(fuelLevel);

  // Read the incoming speed until a newline is received
  String incomingSpeed = Serial.readStringUntil('\n');
  
  // Convert the incomingSpeed to an integer
  speed = incomingSpeed.toInt();
  Serial.println(speed);
  
  updateElements(speed, fuelLevel, headlightStatus);
}

void drawStaticElements()
{
    drawNotificationBar();
    drawFuelGaugeFrame();
    drawSpeedometerFrame();
}

void updateElements(int speed, int fuelLevel, int headlightStatus) {
    drawStaticElements(); // Draw static elements
    drawSpeed(speed); // Update the speed display
    drawFuelLevel(fuelLevel); // Update the fuel level display
    drawLowFuelWarning(fuelLevel);
    drawHeadlight(headlightStatus);
  
    display.display();
}

void drawNotificationBar()
{
    display.drawRect(1, 0, 126, 18, WHITE);
}

void drawSpeedometerFrame()
{
    //display.drawRect(0, 19, 70, 45,WHITE);
    display.fillRect(0, 19, 70, 45, BLACK);
    display.setTextSize(1);      
    display.setTextColor(WHITE);
    display.setCursor(72,22);
    display.print("MPH");
}

void drawSpeed(int speed)
{
  display.setTextSize(6);      
  display.setTextColor(WHITE);

  int number1 = speed / 10; // Get the tens digit
  int number2 = speed % 10; // Get the ones digit

  if (number1 != 0)
  {
    // Draw Number 1
    display.setCursor(3,22);     // Start at top-left corner
    display.print(number1);
  }
  // Draw Number 2
  display.setCursor(37, 22);
  display.print(number2);

}

void drawFuelGaugeFrame()
{
  display.drawRect(112, 22, 14, 42, WHITE);
  display.drawRect(114, 21, 10, 2, WHITE);
}

void drawFuelLevel(int fuelLevel) {
  // Clear the fuel level area before drawing the new level
  display.fillRect(114, 24, 11, 38, BLACK); // Adjust size and position as necessary

  // Draw the bars from bottom to top
  for (int i = 0; i < 5; ++i) {
    display.setCursor(114, 60 - i * 9); // Adjust the positioning as necessary
    
    // Check if the current bar should be filled based on the fuelLevel
    if (i < fuelLevel) {
      display.setTextSize(2);
      display.write(0xDE); // Write the custom character for filled bar
    } else {
      // Optionally draw an empty bar if you want a clear indicator of the 'empty' sections
      display.drawRect(114, 60 - i * 9, 10, 8, WHITE); // Adjust the size as necessary
    }
    drawFuelArrow(fuelLevel);
  }
}

void drawFuelArrow(int fuelLevel)
{
  // Clear the arrow area to ensure only one arrow is displayed at a time
  display.fillRect(103, 20, 8, 40, BLACK); // Adjust the width and height to cover all arrows

  // Calculate the Y position of the arrow based on the fuel level
  int arrowYPosition = 56 - (fuelLevel - 1) * 9; // Adjust if necessary, based on your layout

  // Draw the arrow next to the correct bar
  display.setCursor(103, arrowYPosition);
  display.setTextSize(1);
  display.write(0x10); 
}

void drawLowFuelWarning(int fuelLevel)
{
  static unsigned long lastToggleTime = 0;
  static bool isStrobeWhite = false;
  
  // Clear the warning area
  
  // Only show warning if fuel level is 1
  if (fuelLevel == 1) {

    display.setCursor(20, 6);
    display.setTextSize(1);
    display.print("LOW JUICE!"); // Draw the "LOW FUEL!" text
    
    // Check if it's time to toggle the strobe
    if (millis() - lastToggleTime >= 500) {
      // It's time to toggle
      isStrobeWhite = !isStrobeWhite;
      lastToggleTime = millis();
    }

    // Draw the rounded rectangle with the current strobe color
    display.fillRoundRect(4, 3, 12, 12, 3, isStrobeWhite ? WHITE : BLACK);

    // Set the colors for the "!!" symbol and "LOW FUEL!" text
    display.setTextColor(isStrobeWhite ? BLACK : WHITE);
    display.setCursor(7, 6);
    display.setTextSize(1);
    display.write(0x13); // Draw the "!!" symbol
  }
}

void drawHeadlight(int status)
{
    display.setCursor(114, 3);
    display.setTextSize(2);
    
    // Check the status and display the corresponding symbol
    if (status == 1) {
        display.setTextColor(WHITE);
        display.write(0x2A); // Display the "on" symbol
    } else {
        display.setTextColor(BLACK);
        display.write(0x2A); // Display the "off" symbol
    }
}
