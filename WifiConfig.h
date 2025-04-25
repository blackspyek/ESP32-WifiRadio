#ifndef WIFICONFIG_H
#define WIFICONFIG_H
#include <Arduino.h>
#include <WiFi.h>
#include "Secrets.h" 

// Funkcja do połączenia z WiFi
void connectToWiFi() {
  // Rozpoczynamy połączenie z WiFi
  WiFi.begin(SSID, WIFI_PASSWORD);

  // Czekamy, aż połączenie będzie nawiązane
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
}

#endif
