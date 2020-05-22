// TinyXML invokes this callback as elements are parsed in XML,
// allowing us to pick out just the tidbits of interest rather than
// dynamically allocating and then traversing a whole brobdingnagian
// tree structure; very Arduino-friendly!
// As written here, this looks for XML tags named "seconds" and extracts
// the accompanying value (a predicted bus arrival time, in seconds).
// If it's longer than our minimum threshold, the least two values are
// sorted and saved (sooner time first); one or both may be zero if
// further predictions are not available. Results are stored in global
// two-element int array named 'seconds' and moved to stop struct later.
void XML_callback(uint8_t statusflags, char* tagName,
 uint16_t tagNameLen, char* data, uint16_t dataLen) {
  if ((statusflags & STATUS_START_TAG) && !strcasecmp(tagName, "/gpx/wpt")) {
    numWpts++;
    Serial.println("A Wpts: " + numWpts);
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/name") && numWpts == 1) {
    cache.geocode = String(data);
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/urlname")) {
    cache.wpts[numWpts - 1].name = String(data);
    Serial.println("B Wpts: " + numWpts);
    Serial.println("Name: " + cache.wpts[numWpts - 1].name);
  }
  if ((statusflags & STATUS_ATTR_TEXT) && !strcasecmp(tagName, "lat")) {
    cache.wpts[numWpts - 1].lat = String(data).toFloat();
  }
  if ((statusflags & STATUS_ATTR_TEXT) && !strcasecmp(tagName, "lon")) {
    cache.wpts[numWpts - 1].lon = String(data).toFloat();
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/groundspeak:cache/groundspeak:container")) {
    cache.size = stringToContainer(String(data));
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/groundspeak:cache/groundspeak:difficulty")) {
    cache.difficulty = String(data).toFloat();
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/groundspeak:cache/groundspeak:terrain")) {
    cache.terrain = String(data).toFloat();
  }
}
