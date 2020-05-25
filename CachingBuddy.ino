#include <printf.h>
#include <OLEDDisplayUi.h>
#include <SH1106Brzo.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <FS.h>
#include <TinyXML.h>
#include <Adafruit_NeoPixel.h>
#include <qrcode.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>

// HTML Templates for the Upload
#include "Upload.h"

// German Translation
#include "text_DE.h"

// Debug mode: Write to Serial
//#undef SERIAL
#define SERIAL

/**
 * Frames
 */
#define FRAME_COUNT 3
#define FRAME_MAIN 0
#define FRAME_CACHES 1
#define FRAME_WIFI 2

/**
 * Constants
 */
// Software Settings
#define SW_NAME "CachingBuddy"
#define VERSION "v0.3"
#define BOOTING_TIME 3000
// Address of SH1106 Display controller
#define OLED_ADDR   0x3C
// GPS Settings: GPIO Pins and data rate and query delay
#define GPIO_GPS_RX 12
#define GPIO_GPS_TX 13
#define GPS_BAUD 9600
#define GPS_DELAY 500
// LED-Ring connected to GPIO 0 
#define GPIO_LED 0
// LED-Ring size: CJMCV-2812-24
#define LED_COUNT 24
// Set degrees per LED
#define DEGREE_PER_LED (360.0 / LED_COUNT)
// LED Brightness in percent (0.0 - 1.0)
#define LED_BRIGHTNESS .25
// Distance under that the color gradient is used
#define MAX_DISTANCE 50.0
// XML read buffer
#define XML_BUFFER_SIZE 256
// Define max Number of geocaches on CachingBuddy
#define MAX_CACHES 128
// Define max number of waypoints per cache
#define MAX_WAPOINTS 32

// Color gradient definition. Will be lerped according to percentage.
// * Number of Steps
// * Break-even point of percentage from MAX_DISTANCE (= 0%) to zero distance (= 100%)
// * Colors at break-even points
// * Color for distances larger than MAX_DISTANCE
#define STEP_NUM 3
float steps[] = {0.0, .5, 1.0};
uint32_t stepColors[] = {0xff0000, 0xffff00, 0x00ff00};
#define COLOR_FAR_AWAY 0x0000ff

// The cache cointainer sizes
typedef enum {
  Micro,
  Small,
  Regular,
  Large,
  Other
} container_t;

// A waypoint.
// Definied by coordinates and name
typedef struct wpt_t {
  double lat;
  double lon;
  String name;
} wpt_t;

// A geoache.
// Index of current cache
// Number of waypoints in cache (0 = not loaded)
// Index of current waypoint
// Defined by its geocode, groundspeak-options (size, D/T rating) and its waypoints
typedef struct geocache_t {
  uint8_t index = 0;
  uint8_t numWpts = 0;
  uint8_t wptIndex = 0;
  String geocode;
  container_t size;
  float difficulty;
  float terrain;
  wpt_t wpts[MAX_WAPOINTS];
} geocache_t;

// Representation of a tracking search.
// It is defined "how" to reach the target, by distance in meters and course in degrees
typedef struct search_t {
  unsigned long distance;
  double course;
} search_t;

////////////////////
// GOLBAL SECTION //
////////////////////
// XML object and buffer instance
TinyXML xml;
uint8_t xmlBuffer[XML_BUFFER_SIZE];

// The TinyGPS++ object and the SoftwareSerial to read the GPS data
TinyGPSPlus gps;
SoftwareSerial ss(GPIO_GPS_RX, GPIO_GPS_TX);

// OLED Display and UI instance for Frames
SH1106Brzo lcd(OLED_ADDR, SDA, SCL);
OLEDDisplayUi ui(&lcd);

// LED Compass and the currently active led index
Adafruit_NeoPixel compass(LED_COUNT, GPIO_LED, NEO_GRB + NEO_KHZ800);
uint8_t activeLed = 0;

// List of available Caches on internal flash and number of caches there
String avlbCaches[MAX_CACHES];
uint8_t numCaches;

/**
 * The current Cache:
 */
// Geocache instance and current search
geocache_t cache;
search_t current_search;

// status
uint64_t nextUpdate;
bool isBooting = true;
uint8_t buttonsPressed = 0;

/**
 * Setup Caching Buddy
 * * Starts GPS
 * * Initializes compass
 * * Initialize button interrupts
 * * Initialize OLED Display
 * * Load list of available caches
 * * Load first cache
 */
void setup() {
#ifdef SERIAL
  Serial.begin(115200);
#endif
  ss.begin(GPS_BAUD);
  SPIFFS.begin();
  xml.init((uint8_t *)xmlBuffer, sizeof(xmlBuffer), &XML_callback);

  initCompass();
  initButtons();
  initLCD();
  disableWifi();
  listCaches();
  loadGpxFile(0);

#ifdef SERIAL
  Serial.println(SW_NAME);
  Serial.print(F("Using TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
#endif
}

/**
 * Load all Caches vom Flash memory
 * The caches are not sorted alphabetically but by the order they get uploaded
 * @TODO: Check filetypes. Currently assuming only correct files
 */
void listCaches() {
  uint8_t i = 0;
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      avlbCaches[i++] = dir.fileName();
  }
  numCaches = i;
#ifdef SERIAL
  Serial.print(numCaches);
  Serial.println(" loaded.");
#endif
}

/**
 * Iterate the currently active cache by 1
 */
void nextCache() {
  loadGpxFile((cache.index + 1) % numCaches);
}

/**
 * Iterate the curently active waypoint by 1
 */
void nextWaypoint() {
  cache.wptIndex = (cache.wptIndex + 1) % cache.numWpts;
}

/**
 * Load a Geocache by index of the GPX file
 */
void loadGpxFile(uint8_t index) {
  if (!SPIFFS.exists(avlbCaches[index])) {
#ifdef SERIAL
    Serial.println("File not found.");
#endif
    return;
  }
  File file = SPIFFS.open(avlbCaches[index], "r");
  xml.reset();
  geocache_t newgc;
  newgc.index = index;
  cache = newgc;
  while(file.available()) {
    xml.processChar(file.read());
  }
#ifdef SERIAL
  char statbuf[100];
  sprintf(statbuf, "Loaded Cache: %s (%s) D%1.1f/T%1.1f %d @N%f,E%f\n", cache.wpts[cache.wptIndex].name.c_str(), cache.geocode.c_str(), cache.difficulty, cache.terrain, cache.size, cache.wpts[cache.wptIndex].lon, cache.wpts[cache.wptIndex].lat);
  Serial.println(statbuf);
#endif
}

/**
 * Calculate the cours to the target depending on the current movement course
 * Using over-and underflow correction
 */
double effectiveCourse() {
  double deg = TinyGPSPlus::courseTo(
    gps.location.lat(),
    gps.location.lng(),
    cache.wpts[cache.wptIndex].lat,
    cache.wpts[cache.wptIndex].lon) - (gps.course.isValid() ? gps.course.deg() : 0.0);
  if (deg < 360.0) deg += 360.0;
  while (deg > 360.0) deg -= 360.0;
  return deg;
}

/**
 * Calculate and return the current search to tthe target as search_t
 */
struct search_t getSearch() {
#ifdef SERIAL
  char line[120];
  sprintf(line, "%d Sats, Qual %s. Pos: N%f,E%f in %.2f°", 
    gps.satellites.isValid() ? gps.satellites.value(): 0,
    hdopToString(gps.hdop.isValid() ? gps.hdop.hdop() : 100.0).c_str(),
    gps.location.isValid() ? gps.location.lat() : 0,
    gps.location.isValid() ? gps.location.lng() : 0,
    gps.course.isValid() ? gps.course.deg() : 0);
  Serial.println(line);
#endif
  return {
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      cache.wpts[cache.wptIndex].lat,
      cache.wpts[cache.wptIndex].lon),
      effectiveCourse()
  };
}

/**
 * The main loop
 * The GPS and compass updates are delayed, as the GPS cannot be queries too often
 */
void loop() {
  uint64_t mil = millis();
  while (ss.available() > 0)
    gps.encode(ss.read());
  if (! isBooting) {
    loopDisplay();
    loopServer();
    loopButtons();
    if (mil >= nextUpdate) {
      loopCompass();
      current_search = getSearch();
#ifdef SERIAL
      if (mil > 5000 && gps.charsProcessed() < 10)
        Serial.println(F(ERROR_NO_GPS));
#endif  
      nextUpdate = mil + GPS_DELAY;
    }
  }
  if (isBooting && mil > BOOTING_TIME) {
    isBooting = false;
  }
}
