#define GPIO_BUTTON1 2
#define GPIO_BUTTON2 14
#define BUTTON_DEBOUNCE_MS 100

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

// Debounce timer
uint64_t next_interrupt_time = 0;
uint8_t buttonsDirty = 0;

/**
 * Initialize Button handling
 * To avoid blocking the main program, the button presses are handled via interrupts
 * with a debounce timer instead.
 */
void initButtons() {
  pinMode(GPIO_BUTTON1, INPUT_PULLUP);
  pinMode(GPIO_BUTTON2, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON1), handleButton1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON2), handleButton2, CHANGE);
}

/**
 * Interrupt handler for button 1
 */
ICACHE_RAM_ATTR void handleButton1() {
  buttonPressedHdl(0x1, digitalRead(GPIO_BUTTON1));
}

/**
 * Interrupt handler for button 2
 */
ICACHE_RAM_ATTR void handleButton2() {
  buttonPressedHdl(0x2, digitalRead(GPIO_BUTTON2));
}

/**
 * Button press handler
 * Every Button is assigned to a bit
 */
ICACHE_RAM_ATTR void buttonPressedHdl(uint8_t btn, uint8_t val) {
  uint64_t interrupt_time = millis();
  if (interrupt_time >= next_interrupt_time) {
    if (val == LOW) {
      // Pressed
      if (! (buttonsDirty & btn)) {
        // Set buttons dirty to prevent debounce
        Serial.print("Pressed ");
        Serial.println(btn);
        buttonsPressed |= btn;
        buttonsDirty |= btn;
      }
    }
    next_interrupt_time = interrupt_time + BUTTON_DEBOUNCE_MS;
  }
  if (val == HIGH) {
    // Released -> Release dirty state
    buttonsDirty &= ~btn;
    Serial.print("relase; dirty: ");
    Serial.print(buttonsDirty);
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
