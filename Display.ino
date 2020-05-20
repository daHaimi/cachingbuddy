/********
 * OLED LIB: https://github.com/rene-mt/esp8266-oled-sh1106/
 ********/
/*
#define OLED_SDA    D2
#define OLED_SDC    D4
#define OLED_ADDR   0x3C

#include <font8x16.h>
#include <ssd1306xled.h>
#include <font6x8.h>

SH1106 lcd(OLED_ADDR, OLED_SDA, OLED_SDC);
*/

#define CHAR_SAT 0
#define CHAR_DOTS 1
#define CHAR_LOC 2
#define CHAR_DEG 3

byte satelite[8] = {
  B00000,
  B11000,
  B11010,
  B11110,
  B01110,
  B01111,
  B01011,
  B00011
};

byte dots[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B10101,
  B00000
};

byte loc[] = {
  B01110,
  B11111,
  B11011,
  B11111,
  B01110,
  B01110,
  B00100,
  B00100
};

byte degree[] = {
  B01000,
  B10100,
  B01000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

void initLCD() {
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Symbols
  lcd.createChar(CHAR_SAT, satelite);
  lcd.createChar(CHAR_DOTS, dots);
  lcd.createChar(CHAR_LOC, loc);
  lcd.createChar(CHAR_DEG, degree);
  
  lcd.setCursor(1,0);
  lcd.print(SW_NAME + " v" + VERSION);
  
  lcd.setCursor(12,2);
  lcd.print("By Haimi");
  
  lcd.setCursor(0, 3);
  lcd.print("Lade Caches...");
}


void lcdSetCacheInfo() {
  clearLine(0);
  lcd.setCursor(13,0);
  lcd.print(cache.geocode);
  lcd.setCursor(0,0);
  if (cache.name.length() > 12) {
    lcd.print(cache.name.substring(0, 12));
    lcd.write(CHAR_DOTS);
  } else {
    lcd.print(cache.name);
  }
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
  sprintf(buf, "%3d\x03", course);
  return String(buf);
}

void lcdUpdateStatus(unsigned long distance, double course) {
  clearLine(1);
  lcd.setCursor(0, 1);
  lcd.write(CHAR_LOC);
  if (gps.location.isValid()) {
    lcd.print(" " + humanDistance(distance));
    lcd.setCursor(16, 1);
    lcd.print(humanCourse(course));
  } else {
    lcd.print(" Suche Satelliten\x01");
  }
  
  char line[21];
  sprintf(line, "\x08%2d Qual: %s", 
    gps.satellites.isValid() ? gps.satellites.value(): 0, // Sats
    hdopToString(gps.hdop.isValid() ? gps.hdop.hdop() : 100.0).c_str() // HDOP
  );
  clearLine(3);
  lcd.setCursor(0, 3);
  lcd.printf(line);
}

void clearLine(uint8_t line) {
  lcd.setCursor(0, line);
  lcd.printf("                    ");
}
