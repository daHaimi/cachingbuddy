

/**
 * Translate HDOP value to a human-readable string
 */
String hdopToString(double hdop) {
  uint8_t h = (uint8_t) round(hdop);
  if (h > 20) return HDOP_POOR;
  if (h > 10) return HDOP_FAIR;
  if (h > 5) return HDOP_MODERATE;
  if (h > 2) return HDOP_GOOD;
  if (h > 1) return HDOP_EXCELLENT;
  return HDOP_PERFECT;
}

/**
 * Parses container size from an XML-string to container_t
 */
container_t stringToContainer(String s) {
  if (s.equalsIgnoreCase("micro")) return Micro;
  if (s.equalsIgnoreCase("small")) return Small;
  if (s.equalsIgnoreCase("regular")) return Regular;
  if (s.equalsIgnoreCase("large")) return Large;
  return Other;
}

/**
 * Create a human-readable string from distance in meters
 */
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

/**
 * Create a human-readable string from course in degrees
 */
String humanCourse(uint16_t course) {
  char buf[8];
  sprintf(buf, "%3d°", course);
  return String(buf);
}
