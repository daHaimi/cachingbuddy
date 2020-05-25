// Variables for the config server
bool serverActive = false;
ESP8266WebServer server(80);

/**
 * Start the configuration server:
 * * Create a Wifi hotspot
 * * Serve Settings via HTML and listen for HTTP endoints
 */
void startServer() {
#ifdef SERIAL
  Serial.println("Starting WLAN-Hotspot");
#endif
  WiFi.forceSleepWake();
  delay( 1 );
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SW_NAME);
  delay(500);
 
  server.on(F("/"), indexAction);
  server.on(F("/upload"), HTTP_POST, [](){ server.send(200); }, uploadAction);
  server.on(UriBraces("/{}/download"), downloadAction);
  server.on(UriBraces("/{}/delete"), deleteAction);
 
  server.begin();
  serverActive = true;
}

/**
 * Stop the HTTP-Server and deactivate the WiFi-Interface
 */
void stopServer() {
  serverActive = false;
  server.close();
  server.stop();
  disableWifi();
  resetQRCode();
}

/**
 * Completely deactivate the WiFi interface to reduce power consumption
 */
void disableWifi() {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
}

/**
 * Toggle the WiFi interface
 */
void toggleWifi() {
  if (serverActive) {
    stopServer();
  } else {
    startServer();
  }
}

/**
 * Called from main loop: Handle HTTP-Requests
 */
void loopServer() {
  if (serverActive) {
    server.handleClient();
  }
}

/**
 * Getter: Is server active?
 */
bool isServerActive() {
  return serverActive;
}

/**
 * Get server's IP address as String
 */
String getIPAddress() {
  return WiFi.softAPIP().toString();
}

/**
 * GET /
 * Create administration HTML page
 */
void indexAction() {
  uint8_t i;
  char buf[256];
  sprintf(buf, TPL_HTML_START, SW_NAME, TEXT_ADMINISTRATION, SW_NAME, TEXT_ADMINISTRATION);
  String list = String(buf) + "<ul>";
  for (i = 0; i < numCaches; i++) {
    const char *str = avlbCaches[i].c_str();
    sprintf(buf, TPL_ITEM, str, str + 1, str, TEXT_ARE_YOU_SURE);
    list += String(buf);
  }
  list += "</ul>";
  sprintf(buf, TPL_FORM, TEXT_CHOOSE_GPX, TEXT_UPLOAD); 
  server.send(200, "text/html", TPL_HTML_START + list + TPL_SEP + String(buf) + TPL_HTML_END);
}

File fsUploadFile;
/**
 * POST /upload
 * Upload file to flash memory
 */
void uploadAction() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    HTTPUpload& upload = server.upload();
    if(upload.status == UPLOAD_FILE_START){
      String filename = upload.filename;
      if(!filename.startsWith("/")) filename = "/" + filename;
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
    } else if(upload.status == UPLOAD_FILE_WRITE) {
      if(fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END){
      if(fsUploadFile) {
        fsUploadFile.close();
        listCaches();
        server.sendHeader("Location", String("/"), true);
        server.send ( 302, "text/plain", "");
      } else {
        server.send(500, "text/plain", "500: couldn't create file");
      }
    }
  }
}

/**
 * GET /<filename>/download
 * Download GPX from device
 * @TODO: Implement
 */
void downloadAction() {
    String gpxFile = server.pathArg(0);
    server.send(200, "text/plain", "File: '" + gpxFile + "'");
}

/**
 * GET /<filename>/delete
 * Delete GPX file from device
 * @FIXME: Device cannot handle no GXP file being present at the moment
 */
void deleteAction() {
    String gpxFile = server.pathArg(0);
    SPIFFS.remove("/" + gpxFile);
    cache.index = 0;
    cache.wptIndex = 0;
    listCaches();
    server.sendHeader("Location", String("/"), true);
    server.send ( 302, "text/plain", "");
}
