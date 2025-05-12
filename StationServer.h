#ifndef STATIONSERVER_H
#define STATIONSERVER_H
#include <WiFi.h>
#include <Preferences.h>

extern WiFiServer server;
extern Preferences prefs;

void saveRadioURL(int radioStationNumber, const String& name, const String& url);
void handleRequest(WiFiClient client);
String urlDecode(const String &input);
#endif