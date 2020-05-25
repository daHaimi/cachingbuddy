#define GPIO_BUTTON1 2
#define GPIO_BUTTON2 14
#define BUTTON_DEBOUNCE_MS 100

// Debounce timer
uint64_t next_interrupt_time = 0;

/**
 * Initialize Button handling
 * To avoid blocking the main program, the button presses are handled via interrupts
 * with a debounce timer instead.
 */
void initButtons() {
  pinMode(GPIO_BUTTON1, INPUT_PULLUP);
  pinMode(GPIO_BUTTON2, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON1), handleButton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON2), handleButton2, FALLING);
}

/**
 * Interrupt handler for button 1
 */
ICACHE_RAM_ATTR void handleButton1() {
  buttonPressedHdl(0x1);
}

/**
 * Interrupt handler for button 2
 */
ICACHE_RAM_ATTR void handleButton2() {
  buttonPressedHdl(0x2);
}

/**
 * Button press handler
 * Every Button is assigned to a bit, so that multiple buttons may be pressed at once
 */
ICACHE_RAM_ATTR void buttonPressedHdl(uint8_t btn) {
  uint64_t interrupt_time = millis();
  if (interrupt_time >= next_interrupt_time && !(buttonsPressed & btn)) {
    buttonsPressed |= btn;
    next_interrupt_time = interrupt_time + BUTTON_DEBOUNCE_MS;
  }
}

/**
 * Button handler called from main loop
 * This is meant to decouple the button pressing interrupt from the execution logic
 */
void loopButtons() {
  if (buttonsPressed & 0x1) {
    buttonsPressed &= ~0x1;
    // Button 1 always goes to the next page
    ui.nextFrame();
  }
  if (buttonsPressed & 0x2) {
    buttonsPressed &= ~0x2;
    // Button 2 is depending on the currently selected page
    uint8_t curFrame = ui.getUiState()->currentFrame;
    if (curFrame == FRAME_MAIN) {
      // Main Frame: Iterate waypoint
      nextWaypoint();
    } else if (curFrame == FRAME_CACHES) {
      // Cache list: Iterate current cache
      nextCache();
    } else if (curFrame == FRAME_WIFI) {
      // Wifi settings: Toggle Wifi
      toggleWifi();
    }
  } 
}
