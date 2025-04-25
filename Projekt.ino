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

}

void loop() {
  audio.loop();
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
      updateSong(title, author);
    }
  }
}
