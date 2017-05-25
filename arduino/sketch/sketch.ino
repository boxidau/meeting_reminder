// input status LED pin numbers
const int inputLED_a = 9;
const int inputLED_b = 10;
const int inputLED_c = 11;
const int headlightLED = 14;
const int rightBrakeLED = 15;
const int leftBrakeLED = 16;

String inputString = "";
boolean eventReady = false;

int interval = 10;
const int maxAlert = 5;

unsigned short cyclePos = 0;
unsigned long current = 0;
unsigned long previous = 0;
unsigned long lastSerialEvent = 0;
unsigned long alertSerialInterval = 30000; // 30 sec

//////////////////////
// SETUP /////////////
//////////////////////
void setup() 
{
  Serial.begin(9600);  // Serial for debugging
  inputString.reserve(255);
  delay(1000); // allow us time to reprogram in case things are freaking out
  pinMode(headlightLED, OUTPUT);
  pinMode(rightBrakeLED, OUTPUT);
  pinMode(leftBrakeLED, OUTPUT);
}

////////////////////
// MAIN LOOP ///////
////////////////////
void loop() 
{
  serialEvent();
  current = millis();
  
  if (eventReady) {
    lastSerialEvent = current;
    interval = inputString.toInt();
    Serial.print(inputString);
    inputString = "";
    eventReady = false;
  }

  // alert by lighting LED's if no serial data 
  // for alertSerialInterval milliseconds
  if (current > lastSerialEvent + alertSerialInterval) {
    illuminateLed(5);
    illuminateLed(6);
    illuminateLed(7);
    illuminateLed(8);

  // serial data present but out of range
  } else if (interval > maxAlert || interval < 1) {
    disableLeds();
  
  // data in range, do flashing
  } else if (current - previous >= interval * 50) { 
    cycleLeds();
    previous = current;
  }

}

void cycleLeds() {
  disableLeds();
  cyclePos++;
  illuminateLed(cyclePos % 6);
  if (cyclePos % 3 == 0) {
    illuminateLed(6);
  }
  if (cyclePos % 3 == 1) {
    illuminateLed(7);
  }
  if (cyclePos % 3 == 2) {
    illuminateLed(8);
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      eventReady = true;
    }
  }
}

void disableLeds() {
  pinMode(inputLED_a, INPUT);
  pinMode(inputLED_b, INPUT);
  pinMode(inputLED_c, INPUT);
  digitalWrite(headlightLED, LOW);
  digitalWrite(leftBrakeLED, LOW);
  digitalWrite(rightBrakeLED, LOW);
}

void illuminateLed(int ledId) {
  if (ledId == 0) {
    // UP
    pinMode(inputLED_a, INPUT);
    digitalWrite(inputLED_a, HIGH);
    pinMode(inputLED_b, OUTPUT);
    digitalWrite(inputLED_b, HIGH);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, LOW);
  }

  if (ledId == 1) {
    // LEFT
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, LOW);
    pinMode(inputLED_b, OUTPUT);    
    digitalWrite(inputLED_b, HIGH);
    pinMode(inputLED_c, INPUT);
    digitalWrite(inputLED_c, LOW); 
  }

  if (ledId == 2) {
    // DOWN
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, HIGH);
    pinMode(inputLED_b, OUTPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, INPUT);
    digitalWrite(inputLED_c, LOW);
  }
  
  if (ledId == 3) {
    //RIGHT
    pinMode(inputLED_a, INPUT);
    digitalWrite(inputLED_a, LOW);
    pinMode(inputLED_b, OUTPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, HIGH);
  }

  if (ledId == 4) {
    // SPACE
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, LOW);
    pinMode(inputLED_b, INPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, HIGH);  
  }
  
  if (ledId == 5) {
    // CLICK
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, HIGH);
    pinMode(inputLED_b, INPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, LOW);
  }

  if (ledId == 6) {
    // HEADLIGHTS
    digitalWrite(headlightLED, HIGH);
  }

  if (ledId == 7) {
    // HEADLIGHTS
    digitalWrite(leftBrakeLED, HIGH);
  }

  if (ledId == 8) {
    // HEADLIGHTS
    digitalWrite(rightBrakeLED, HIGH);
  }
}

