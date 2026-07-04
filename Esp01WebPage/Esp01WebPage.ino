#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "arduino_secrets.h"


AsyncWebServer server(80);

String lastMessage = "";

void setup() {

  Serial.begin(74880);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(ESP.getFlashChipRealSize());
  // Mount LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  // Serve website
  server.serveStatic("/", LittleFS, "/")
        .setDefaultFile("index.html");

  // Endpoint for messages
  server.on("/message", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", lastMessage);
    lastMessage = "";
  });

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Files uploaded successfully!");
  }, [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    static File uploadFile;
    
    if (index == 0) {
      String filepath = "/" + filename;
      Serial.printf("Upload Start: %s\n", filepath.c_str());
      uploadFile = LittleFS.open(filepath, "w");
      if (!uploadFile) {
        Serial.println("Failed to open file for writing");
      }
    }

    if (uploadFile && len) {
      size_t written = uploadFile.write(data, len);
      if (written != len) {
        Serial.printf("Write mismatch: %zu/%zu\n", written, len);
      }
    }

    if (final) {
      if (uploadFile) {
        uploadFile.close();
        Serial.printf("Upload Complete: %s (%u bytes)\n", filename.c_str(), index + len);
      }
    }
  });
  server.begin();
}

void loop() {

  if (Serial.available()) {
    lastMessage = Serial.readStringUntil('\n');
    lastMessage.trim();
  }

}
