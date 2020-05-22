/********
 * OLED LIB: https://github.com/ThingPulse/esp8266-oled-ssd1306
 ********/
#include "Images.h"

#define PER_PAGE 4
#define NULL '\0'

// QRCode
QRCode qrcode;
uint8_t *qrcodeData = NULL;

bool drawStartup() {
  lcd.setFont(ArialMT_Plain_24);
  lcd.drawString(10, 5, "Caching");
  lcd.drawString(28, 22, "Buddy");

  lcd.setFont(ArialMT_Plain_10);
  lcd.setTextAlignment(TEXT_ALIGN_LEFT);
  lcd.drawString(0, 50, "v" + VERSION);
  lcd.setTextAlignment(TEXT_ALIGN_RIGHT);
  lcd.drawString(128, 50, "by Haimi");

  lcd.display();
  return false;
}

// FRAMES
void drawCacheInfo(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);

  // Name
  display->drawString(x + 0, y + 0, cache.wpts[curWptIdx].name);
  // Direction
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(x + 0, y + 10, cache.geocode);
  // Size
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(x + 64, y + 10, "S:");
  display->drawXbm(x + 64, y + 12, locW, locH, sizes[cache.size]);
  // Terrain/Difficulty
  char buf[10];
  sprintf(buf, "D%01.1f T%01.1f", cache.difficulty, cache.terrain);
  display->drawString(128, y + 10, buf);
    
  display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
  if (gps.location.isValid()) {
    display->drawXbm(x + 2 , y + 28, locW, locH, loc);
    display->drawString(x + 64, y + 24, humanDistance(current_search.distance) + "  " + humanCourse(current_search.course));
  
    // Satellites + Location
    display->setFont(ArialMT_Plain_10);
    sprintf(buf, "%d/%d | Q: %s (%d Sat)", curWptIdx + 1, numWpts, hdopToString(gps.hdop.isValid()).c_str(), gps.satellites.value());
    display->drawString(x + 64, y + 42, buf);
  } else {
    display->drawString(x + 64, y + 22, "Suche...");
    // wpts
    display->setFont(ArialMT_Plain_10);
    sprintf(buf, "%d/%d", curWptIdx + 1, numWpts);
    display->drawString(x + 64, y + 42, buf);
  }
  
}

void drawCacheList(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(x + 0, y + 0, "Verfügbare Caches");

  char buf[8];
  sprintf(buf, "%d/%d", curCacheIdx + 1, numCaches);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128, y + 0, buf);

  short ly = y + 11;
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  uint8_t subPageIdx = curCacheIdx / PER_PAGE;
  uint8_t startIdx = subPageIdx * PER_PAGE;
  for (uint8_t i = startIdx; i < min(numCaches, (uint8_t)(startIdx + PER_PAGE)); i++) {
    short cy =  ((i - startIdx) * 10);
    if (i == curCacheIdx) {
      display->drawString(x + 0, ly + cy, "#");
    }
    display->drawString(x + 10, ly + cy, avlbCaches[i]);
  }
}

void drawSettings(OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(x + 0, y + 0, "WiFi Einstellungen");
  if (! isServerActive()) {
    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(x + 64, y + 26, "Deaktiviert");
  } else {
    initQRCode();
    display->drawString(x, y + 10, "SSID: " + SW_NAME);
    display->drawString(x, y + 40, "IP:" + getIPAddress());
    uint8_t dx = 128 - qrcode.size, dy = 64 - qrcode.size;
    for (uint8_t y = 0; y < qrcode.size; y++) {
      for (uint8_t x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) display->setPixel(dx + x, dy + y);
      }
    }
  }
}

void initQRCode() {
  if (qrcodeData == NULL) {
    qrcodeData = (uint8_t*)malloc(sizeof(uint8_t) * qrcode_getBufferSize(1));
    String url = "http://" + getIPAddress();
    qrcode_initText(&qrcode, qrcodeData, 1, 0, url.c_str());
  }
}

void resetQRCode() {
  free(qrcodeData);
  qrcodeData = NULL;
}

String humanDistance(unsigned long distance) {
  String result = String(distance) + "m";
  if (distance >= 1000L) {
    char buf[16];
    unsigned long km = distance / 1000L;
    if (km >= 500L) {
      sprintf(buf, "> 500km");
    } else if (km >= 100L) {
      sprintf(buf, "%ldkm", km);
    } else {
      sprintf(buf, "%2.1fkm", distance / 1000.0);
    }
    result = String(buf);
  }
  return result;
}

String humanCourse(uint16_t course) {
  char buf[8];
  sprintf(buf, "%3d°", course);
  return String(buf);
}

FrameCallback frames[] = {
  drawCacheInfo,
  drawCacheList,
  drawSettings
};
int frameCount = 3;

void initLCD() {
  ui.setTargetFPS(15);
  
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  
  ui.setFrames(frames, frameCount);
  ui.disableAutoTransition();
  
  ui.init();
  lcd.flipScreenVertically();

  drawStartup();
}

void loopDisplay() {
  if (! isBooting) {
    int remainingTimeBudget = ui.update();
    if (remainingTimeBudget > 0) {
      // Do work here

      delay(remainingTimeBudget);
    }
  }
}
