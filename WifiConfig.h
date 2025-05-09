#ifndef WIFICONFIG_H
#define WIFICONFIG_H
#include <Arduino.h>
#include <WiFi.h>
//#include "Secrets.h" 
#include <WiFiManager.h>
#include <Preferences.h>

#define CONFIG_PIN   0               
#define PORTAL_SSID  "ESP32-RadioCfg"
#define PORTAL_PASS  "12345678"      


extern Preferences prefs;

void connectToWiFi() {
  Serial.println(F("📡 Inicjalizacja WiFi..."));
  
  pinMode(CONFIG_PIN, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);              // start od trybu STA (bez zbędnego AP)

  WiFiManager wm;

  // Factory reset, jeśli użytkownik przytrzyma GPIO0 podczas resetu
  if (digitalRead(CONFIG_PIN) == LOW) {
    Serial.println(F("👉 Factory reset Wi-Fi credentials"));
    wm.resetSettings();             // czyści NVS
  }

  wm.setConfigPortalTimeout(120);   // portal gaśnie po 2 min braku aktywności

  Serial.println(F("📡 Łączenie... (portal, jeśli brak kredencji)"));
  bool ok = wm.autoConnect(PORTAL_SSID, PORTAL_PASS);

  if (!ok) {
    Serial.println(F("❌ Provisioning nieudane / timeout – restart"));
    delay(2000);
    ESP.restart();
  }

  // ---------- SPRZĄTANIE ----------
  wm.stopWebPortal();               // gasi HTTP + DNS (WiFiManager ≥2.0)
  WiFi.softAPdisconnect(true);      // wyłącza SoftAP i zwalnia ~10 kB heapu
  WiFi.mode(WIFI_STA);              // pewność, że jesteśmy tylko STA

  Serial.printf("✅ Wi-Fi OK: %s  IP: %s\n",
                WiFi.SSID().c_str(),
                WiFi.localIP().toString().c_str());
}


#endif
