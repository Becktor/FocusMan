#ifndef FOCUS_WEBSERVER_H
#define FOCUS_WEBSERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>  // ✅ Ensure ESPAsyncWebServer is properly included
#include <AsyncTCP.h> // ✅ Required for ESPAsyncWebServer
#include <WiFi.h>       // ✅ Required for ESP32 networking

// ✅ Declare the global WebServer instance
extern AsyncWebServer server;

void initWebServer();

#endif
