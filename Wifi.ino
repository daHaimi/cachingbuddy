// Flashing via Wifi
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriBraces.h>
#include "Upload.h"

bool serverActive = false;

ESP8266WebServer server(80);    // Server Port  hier einstellen

void startServer() {
  Serial.println("Starte WLAN-Hotspot");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SW_NAME);
  delay(500);
  if (MDNS.begin(SW_NAME)) {
    Serial.println("MDNS responder started");
  }
 
  server.on(F("/"), indexAction);
  server.on(F("/upload"), HTTP_POST, [](){ server.send(200); }, uploadAction);
  server.on(UriBraces("/{}/download"), downloadAction);
  server.on(UriBraces("/{}/delete"), deleteAction);
 
  server.begin();
  serverActive = true;
}

void stopServer() {
  serverActive = false;
  server.close();
  server.stop();
  WiFi.mode(WIFI_OFF);
  resetQRCode();
}

void toggleWifi() {
  if (serverActive) {
    stopServer();
  } else {
    startServer();
  }
}

void loopServer() {
  if (serverActive) {
    server.handleClient();
  }
}

bool isServerActive() {
  return serverActive;
}


String getIPAddress() {
  return WiFi.softAPIP().toString();
}

void indexAction() {
  uint8_t i;
  char buf[256];

  String list = "<ul>";
  for (i = 0; i < numCaches; i++) {
    const char *str = avlbCaches[i].c_str();
    sprintf(buf, TPL_ITEM, str, str + 1, str);
    list += String(buf);
  }
  list += "</ul>";

  server.send(200, "text/html", TPL_HTML_START + list + TPL_SEP + TPL_FORM + TPL_HTML_END);
}

File fsUploadFile;

void uploadAction() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    HTTPUpload& upload = server.upload();
    if(upload.status == UPLOAD_FILE_START){
      String filename = upload.filename;
      if(!filename.startsWith("/")) filename = "/" + filename;
      Serial.print("handleFileUpload Name: "); Serial.println(filename);
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
    } else if(upload.status == UPLOAD_FILE_WRITE) {
      if(fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
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

void downloadAction() {
    String gpxFile = server.pathArg(0);
    server.send(200, "text/plain", "File: '" + gpxFile + "'");
}

void deleteAction() {
    String gpxFile = server.pathArg(0);
    SPIFFS.remove("/" + gpxFile);
    curCacheIdx = 0;
    listCaches();
    server.sendHeader("Location", String("/"), true);
    server.send ( 302, "text/plain", "");
}
