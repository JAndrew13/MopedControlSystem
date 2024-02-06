const int hallPin = 9;
const int wheelSize = 16;
const int magnetPosition = 10;

float wheelCircumference = 2 * PI * magnetPosition; 
int speed = 0;
unsigned long prevTime = 0;
unsigned long prevHallPulseTime = 0;
const unsigned long debounceDelay = 200000; // 200 ms debounce delay in microseconds

void setup() {
  Serial.begin(9600);
  pinMode(hallPin, INPUT);
}

void loop() {
  unsigned long currTime = micros();

  // check if the hall effect sensor has been triggered and implement a debounce
  if (digitalRead(hallPin) == HIGH && (currTime - prevHallPulseTime > debounceDelay)) {
    float timeDiffSec = (currTime - prevHallPulseTime) / 1000000.0; // time difference in seconds
    speed = floor((wheelCircumference / timeDiffSec) * 0.0568182); // convert speed to MPH and round down
    
    prevHallPulseTime = currTime;
  }
  
  // if no pulse from the Hall sensor for more than a second, then speed is 0
  if (micros() - prevHallPulseTime > 1000000) {
    speed = 0;
  }

  // refresh the LCD every 500ms
  if (micros() - prevTime > 330000) {
    Serial.print(speed);
    Serial.write('\n'); 
    prevTime = micros(); 
  }
}
