#include <Arduino.h>
#include <Audio.h>
#include "WifiConfig.h"
#include "AudioDac.h" 
#include "RadioTft.h"

void setup() {
  Serial.begin(115200);
  tft_init();

  connectToWiFi();
  configureAudio(RADIO_URL);

  Serial.println("Max volume: "+ audio.maxVolume());
  
  pinMode(14, INPUT_PULLUP);

}
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50 ms debounce delay
bool lastButtonState = HIGH;
bool buttonState;
void loop() {
  audio.loop();
  
  // int reading = digitalRead(14);

  // if (reading != lastButtonState) {
  //   lastDebounceTime = millis(); // reset debounce timer
  // }

  // if ((millis() - lastDebounceTime) > debounceDelay) {
  //   // stable state
  //   if (reading != buttonState) {
  //     buttonState = reading;

  //     if (buttonState == LOW) {
  //       Serial.println("Przycisk wcisniety");

  //       tft_burgerClicked();
  //     }
  //   }
  // }

  // lastButtonState = reading;
}

// Funkcja callback wyświetlająca info o audio
void audio_info(const char *info) {
  Serial.print("Info: ");
  Serial.println(info);

  const char *prefix = "StreamTitle='";
  const char *start = strstr(info, prefix);


  if (start) {
    start += strlen(prefix); // Move past the "StreamTitle='"
    const char *end = strchr(start, '\''); // Find closing quote
    if (end) {
      int len = end - start;
      char streamTitle[128];
      strncpy(streamTitle, start, len);
      streamTitle[len] = '\0'; // Null terminate

      // Convert to String for easier handling
      String fullTitle = String(streamTitle);
      fullTitle.trim(); // Remove any accidental whitespace

      String author = "Unknown";
      String title = fullTitle;

      int sepIndex = fullTitle.indexOf(" - ");
      if (sepIndex != -1) {
        author = fullTitle.substring(0, sepIndex);
        title = fullTitle.substring(sepIndex + 3);
      }
      currentRadioStation.title = title;
      currentRadioStation.author = author;

      Serial.println("Ustawiono Title: " + currentRadioStation.title);
      Serial.println("Ustawiono Author: " + currentRadioStation.author);
      if (!isMenuOpen)
        updateSong(title, author);
    }
  }
}
