#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "arduino_secrets.h"


AsyncWebServer server(80);

String lampStatus = "";

void setup() {

  Serial.begin(74880);

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

  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  server.serveStatic("/", LittleFS, "/")
    .setDefaultFile("index.html");

  server.on("/lampStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", lampStatus);
    lampStatus = "";
  });

  server.on(
    "/changeLampStatus", HTTP_POST, [](AsyncWebServerRequest *request) {
      //TODO: HERE UPDATE ARDUINO LAMP.
      request->send(200, "application/json", lampStatus);
    });

  server.on(
    "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Files uploaded successfully!");
    },
    [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
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
  server.on("/deleteAllFiles", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      if (dir.fileName() != "fileUploader.html") {
        Serial.print("archivo " + dir.fileName());
        bool removed = LittleFS.remove(dir.fileName());
        Serial.print(removed ? " Eliminado exitosamente " : " Error al eliminar ");
      }
    }
    request->send(200, "text/plain", "Archivos eliminados exitosamente, solo queda fileUploader.html");
  });
  server.on("/listAllFiles", HTTP_GET, [](AsyncWebServerRequest *request) {
    Dir dir = LittleFS.openDir("/");
    String files = "";
    while (dir.next()) {
      files += dir.fileName() + "\n";
    }
    request->send(200, "text/plain", files);
  });
  server.begin();
}


void loop() {
  if (Serial.available()) {
    String aux = Serial.readStringUntil('\n');
    if (aux.startsWith("json")) {
      aux.replace("json", "");
    }
    lampStatus.trim();
  }
}
