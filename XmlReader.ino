/**
 * TinyXML callback for GPX parsing
 * Detects the information tags and extracts the information from the tags into the current cache.
 */
void XML_callback(uint8_t statusflags, char* tagName, uint16_t tagNameLen, char* data, uint16_t dataLen) {
  if ((statusflags & STATUS_START_TAG) && !strcasecmp(tagName, "/gpx/wpt")) {
    cache.numWpts++;
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/name") && cache.numWpts == 1) {
    cache.geocode = String(data);
  }
  if ((statusflags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/gpx/wpt/urlname")) {
    // TODO: XML/HTML Entity decode
    cache.wpts[cache.numWpts - 1].name = String(data);
  }
  if ((statusflags & STATUS_ATTR_TEXT) && !strcasecmp(tagName, "lat")) {
    cache.wpts[cache.numWpts - 1].lat = String(data).toFloat();
  }
  if ((statusflags & STATUS_ATTR_TEXT) && !strcasecmp(tagName, "lon")) {
    cache.wpts[cache.numWpts - 1].lon = String(data).toFloat();
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
