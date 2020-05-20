#define BUTTON1 16
#define BUTTON2 14

void initButtons() {
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  
  //attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1, FALLING);
  //attachInterrupt(digitalPinToInterrupt(BUTTON2), handleButton2, FALLING);
}

void handleButton1() {
  Serial.printf("Pressed Button 1");
}

void handleButton2() {
  Serial.printf("Pressed Button 2");
  
}
