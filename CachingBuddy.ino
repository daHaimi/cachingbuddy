#include <RF24.h>
#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <FS.h>
#include <TinyXML.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

#define SW_NAME String("CachingBuddy")
#define VERSION String("0.1")
#define SERIAL false

#define BUFFER_SIZE 256
#define LED_PIN 0
#define LED_COUNT 24
#define DEGREE_PER_LED (360.0 / LED_COUNT)
#define MAX_DISTANCE 50.0
#define STEP_NUM 3

typedef enum {
  Micro,
  Small,
  Regular,
  Large,
  Other
} container_t;

container_t stringToContainer(String s) {
  if (s.equalsIgnoreCase("micro")) return Micro;
  if (s.equalsIgnoreCase("small")) return Small;
  if (s.equalsIgnoreCase("regular")) return Regular;
  if (s.equalsIgnoreCase("large")) return Large;
  return Other;
}

typedef struct {
  String geocode;
  String name;
  container_t size;
  float difficulty;
  float terrain;
  double lat;
  double lon;
} geocache_t;

static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;

// XML
TinyXML xml;
uint8_t xmlBuffer[BUFFER_SIZE];

// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// LED Compass
Adafruit_NeoPixel compass(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t activeLed = 0;

// Display
LiquidCrystal_I2C lcd(0x27,20,4);

// The current Cache
String path = "/GC7W1NN.gpx";
geocache_t cache;

void setup()
{
  if (SERIAL) Serial.begin(115200);
  ss.begin(GPSBaud);
  SPIFFS.begin();
  xml.init((uint8_t *)xmlBuffer, sizeof(xmlBuffer), &XML_callback);

  initLCD();
  initCompass();
  loadGpxFile();

  if (SERIAL) Serial.println(SW_NAME);
  if (SERIAL) Serial.print(F("Using TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());

  delay(3000);
}

void listCaches() {
  String str = "";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      str += dir.fileName();
      str += " / ";
      str += dir.fileSize();
      str += "\r\n";
  }
  if (SERIAL) Serial.print(str);
}

void loadGpxFile() {
  if (!SPIFFS.exists(path)) {
    if (SERIAL) Serial.println("File not found.");
    return;
  }
  File file = SPIFFS.open(path, "r");
  xml.reset();
  geocache_t newgc;
  cache = newgc;
  while(file.available()){
    xml.processChar(file.read());
  }
  char statbuf[100];
  sprintf(statbuf, "Loaded Cache: %s (%s) D%1.1f/T%1.1f %d @N%f,E%f\n", cache.name.c_str(), cache.geocode.c_str(), cache.difficulty, cache.terrain, cache.size, cache.lon, cache.lat);
  if (SERIAL) Serial.println(statbuf);
  lcdSetCacheInfo();
}

#define HDOP_POOR "schlecht" /* >20 */
#define HDOP_FAIR "ungenau" /* 10-20 */
#define HDOP_MODERATE "maessig" /* 5-10 */
#define HDOP_GOOD "gut" /* 2-5 */
#define HDOP_EXCELLENT "exzellent" /* 1-2 */
#define HDOP_PERFECT "perfekt" /* 1 */

String hdopToString(double hdop) {
  uint8_t h = (uint8_t) round(hdop);
  if (h > 20) return HDOP_POOR;
  if (h > 10) return HDOP_FAIR;
  if (h > 5) return HDOP_MODERATE;
  if (h > 2) return HDOP_GOOD;
  if (h > 1) return HDOP_EXCELLENT;
  return HDOP_PERFECT;
}

double effectiveCourse() {
  double deg = TinyGPSPlus::courseTo(
    gps.location.lat(),
    gps.location.lng(),
    cache.lat,
    cache.lon) - (gps.course.isValid() ? gps.course.deg() : 0.0);
  if (deg < 360.0) deg += 360.0;
  while (deg > 360.0) deg -= 360.0;
  return deg;
}

void printStatus() {
  double courseToTarget = effectiveCourse();
  unsigned long distanceToTarget =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      cache.lat,
      cache.lon);
      
  char line[120];
  sprintf(line, "%d Sats, Qual %s. Pos: N%f,E%f in %.2f° Ziel: %dm, %.0f°", 
    gps.satellites.isValid() ? gps.satellites.value(): 0,
    hdopToString(gps.hdop.isValid() ? gps.hdop.hdop() : 100.0).c_str(),
    gps.location.isValid() ? gps.location.lat() : 0,
    gps.location.isValid() ? gps.location.lng() : 0,
    gps.course.isValid() ? gps.course.deg() : 0,
    distanceToTarget, courseToTarget);
  if (SERIAL) Serial.println(line);

  double distPercentage = (double)distanceToTarget / MAX_DISTANCE;
  updateCompass(courseToTarget, distPercentage, gps.location.isValid());
  lcdUpdateStatus(distanceToTarget, courseToTarget);
}

void loop()
{
  printStatus();
  
  smartDelay(2000);

  if (SERIAL && millis() > 5000 && gps.charsProcessed() < 10)Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
