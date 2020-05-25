#include "Images.h"

#define PER_PAGE 4
#define NULL '\0'

// QRCode and data pointer
QRCode qrcode;
uint8_t *qrcodeData = NULL;

/**
 * Draw startup screen
 * This is extra, because we don't want to be able to loop over this frame again
 */
bool drawStartup() {
  lcd.setFont(ArialMT_Plain_24);
  lcd.drawString(10, 5, "Caching");
  lcd.drawString(28, 22, "Buddy");

  lcd.setFont(ArialMT_Plain_10);
  lcd.setTextAlignment(TEXT_ALIGN_LEFT);
  lcd.drawString(0, 50, VERSION);
  lcd.setTextAlignment(TEXT_ALIGN_RIGHT);
  lcd.drawString(128, 50, "by Haimi");

  lcd.display();
  return false;
}

// FRAMES
/**
 * Frame 1: Display all possible infos for the current cache and search on the page
 */
void drawCacheInfo(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  // Name
  display->drawString(x + 0, y + 0, cache.wpts[cache.wptIndex].name);
  // Direction
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(x + 0, y + 10, cache.geocode);
  // Size
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(x + 64, y + 10, "S:");
  display->drawXbm(x + 64, y + 12, locW, locH, sizes[cache.size]);
  // Terrain/Difficulty
  char buf[64];
  sprintf(buf, "D%01.1f T%01.1f", cache.difficulty, cache.terrain);
  display->drawString(128 - x, y + 10, buf);

  // Search
  display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
  if (gps.location.isValid()) {
    display->drawXbm(x + 2 , y + 28, locW, locH, loc);
    display->drawString(x + 64, y + 24, humanDistance(current_search.distance) + "  " + humanCourse(current_search.course));
  
    // Satellites + Location
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    sprintf(buf, "Q: %s (%d Sat)", hdopToString(gps.hdop.isValid()).c_str(), gps.satellites.value());
    display->drawString(x, y + 42, buf);
  } else {
    display->drawString(x + 64, y + 22, TEXT_SEARCHING);
  }
  
  // Waypoints
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  sprintf(buf, "%d/%d", cache.wptIndex + 1, cache.numWpts);
  display->drawString(128 - x, y + 42, buf);
}

/**
 * Frame 2: Display all available caches with currently selected cache highlighted
 */
void drawCacheList(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(x + 0, y + 0, TEXT_AVAILABLE_CACHES);

  char buf[8];
  sprintf(buf, "%d/%d", cache.index + 1, numCaches);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 - x, y + 0, buf);

  short ly = y + 11;
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  uint8_t subPageIdx = cache.index / PER_PAGE;
  uint8_t startIdx = subPageIdx * PER_PAGE;
  for (uint8_t i = startIdx; i < min(numCaches, (uint8_t)(startIdx + PER_PAGE)); i++) {
    short cy =  ((i - startIdx) * 10);
    if (i == cache.index) {
      display->drawString(x + 0, ly + cy, "■");
    }
    display->drawString(x + 10, ly + cy, avlbCaches[i]);
  }
}

/**
 * Frame 3: Display WiFi Settings page
 * If WiFi server is active, also display SSID, IP and QR for the IP
 */
void drawSettings(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(x + 0, y + 0, TEXT_WIFI_SETTINGS);
  if (! isServerActive()) {
    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(x + 64, y + 26, TEXT_DEACTIVATED);
  } else {
    initQRCode();
    display->drawString(x, y + 10, String(TEXT_SSID) + " " + SW_NAME);
    display->drawString(x, y + 40, "IP:" + getIPAddress());
    uint8_t dx = 128 - x - qrcode.size, dy = 64 - qrcode.size;
    for (uint8_t y = 0; y < qrcode.size; y++) {
      for (uint8_t x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) display->setPixel(dx + x, dy + y);
      }
    }
  }
}

/**
 * Create a new QR Code for the current IP Address
 */
void initQRCode() {
  if (qrcodeData == NULL) {
    qrcodeData = (uint8_t*)malloc(sizeof(uint8_t) * qrcode_getBufferSize(1));
    String url = "http://" + getIPAddress();
    qrcode_initText(&qrcode, qrcodeData, 1, 0, url.c_str());
  }
}

/**
 * Free memory for the QR Code
 */
void resetQRCode() {
  free(qrcodeData);
  qrcodeData = NULL;
}

/**
 * List of frames callback methods
 */
FrameCallback frames[FRAME_COUNT] = {
  drawCacheInfo,
  drawCacheList,
  drawSettings
};

/**
 * Initialize the Display with initial settings
 */
void initLCD() {
  ui.setTargetFPS(15);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.setFrames(frames, FRAME_COUNT);
  ui.disableAutoTransition();
  ui.init();
  
  lcd.flipScreenVertically();
  drawStartup();
}

/**
 * Called from main loop
 * Update the frame states
 */
void loopDisplay() {
  if (! isBooting) {
    int remainingTimeBudget = ui.update();
    if (remainingTimeBudget > 0) {
      delay(remainingTimeBudget);
    }
  }
}
