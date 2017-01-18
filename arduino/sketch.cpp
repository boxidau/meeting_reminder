// input status LED pin numbers
const int inputLED_a = 9;
const int inputLED_b = 10;
const int inputLED_c = 11;

String inputString = "";
boolean eventReady = false;

int interval = 10;
const int maxAlert = 5;

unsigned short cyclePos = 0;
unsigned long previous = 0;

//////////////////////
// SETUP /////////////
//////////////////////
void setup() 
{
  Serial.begin(9600);  // Serial for debugging
  inputString.reserve(255);
  delay(1000); // allow us time to reprogram in case things are freaking out
}

////////////////////
// MAIN LOOP ///////
////////////////////
void loop() 
{
  serialEvent();
  if (eventReady) {
    interval = inputString.toInt();
    inputString = "";
    eventReady = false;
    Serial.println(inputString);
  }
  
  unsigned long current = millis();
  
  if (current - previous >= interval * 50) {
    previous = current;
    cyclePos++;
    if (interval > maxAlert) {
      disableLeds();
    } else {
      illuminateLed(cyclePos % 6);
    }
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
    //RIGHT
    pinMode(inputLED_a, INPUT);
    digitalWrite(inputLED_a, LOW);
    pinMode(inputLED_b, OUTPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, HIGH);
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
    // LEFT
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, LOW);
    pinMode(inputLED_b, OUTPUT);    
    digitalWrite(inputLED_b, HIGH);
    pinMode(inputLED_c, INPUT);
    digitalWrite(inputLED_c, LOW); 
  }    

  if (ledId == 4) {
    // SPACE
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, HIGH);
    pinMode(inputLED_b, INPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, LOW);
  }

  if (ledId == 5) {
    // CLICK
    pinMode(inputLED_a, OUTPUT);
    digitalWrite(inputLED_a, LOW);
    pinMode(inputLED_b, INPUT);
    digitalWrite(inputLED_b, LOW);
    pinMode(inputLED_c, OUTPUT);
    digitalWrite(inputLED_c, HIGH);  
  }
}

