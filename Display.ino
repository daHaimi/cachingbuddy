/********
 * OLED LIB: https://github.com/rene-mt/esp8266-oled-sh1106/
 ********/

#define OLED_ADDR   0x3C

SH1106Brzo lcd(OLED_ADDR, SDA, SCL);

const char satelite[] PROGMEM = {
  B00000000,
  B11100000,
  B11100100,
  B01111000,
  B00111000,
  B00111100,
  B01001110,
  B00001110
};

const char loc[] PROGMEM = {
  B0011100,
  B0111110,
  B0110110,
  B0111110,
  B0011100,
  B0011100,
  B0001000,
  B0001000
};

void initLCD() {
  lcd.init();
  lcd.flipScreenVertically();
  
  drawStartup();
}

bool drawStartup() {
  lcd.setFont(ArialMT_Plain_24);
  lcd.drawString(10, 5, "Caching");
  lcd.drawString(28, 22, "Buddy");

  lcd.setFont(ArialMT_Plain_10);
  lcd.setTextAlignment(TEXT_ALIGN_LEFT);
  lcd.drawString(0, 50, "v" + VERSION);
  lcd.setTextAlignment(TEXT_ALIGN_RIGHT);
  lcd.drawString(0, 50, "by Haimi");

  lcd.display();
  return false;
}


void lcdSetCacheInfo() {

/*
  lcd.setCursor(13,0);
  lcd.print(cache.geocode);
  lcd.setCursor(0,0);
  if (cache.name.length() > 12) {
    lcd.print(cache.name.substring(0, 12));
    lcd.write(CHAR_DOTS);
  } else {
    lcd.print(cache.name);
  }
  */
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
  /*
  clearLine(1);
  lcd.setCursor(0, 1);m
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
  */
}
