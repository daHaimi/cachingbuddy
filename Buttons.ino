#define BUTTON1 2
#define BUTTON2 14

void initButtons() {
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2), handleButton2, FALLING);
}

ICACHE_RAM_ATTR void handleButton1() {
  ButtonsPressed |= 0x1;
}

ICACHE_RAM_ATTR void handleButton2() {
  ButtonsPressed |= 0x2;
}

void loopButtons() {
  if (ButtonsPressed & 0x1) {
    ButtonsPressed &= ~0x1;
    ui.nextFrame();
  }
  if (ButtonsPressed & 0x2) {
    ButtonsPressed &= ~0x2;
    Serial.println(ButtonsPressed);
    uint8_t curFrame = ui.getUiState()->currentFrame;
    if (curFrame == FRAME_MAIN) {
      nextWaypoint();
    } else if (curFrame == FRAME_CACHES) {
      nextCache();
    } else if (curFrame == FRAME_WIFI) {
      toggleWifi();
    }
  } 
}
