#include <Arduino.h>
#include <Audio.h>
#include "WifiConfig.h"
#include "AudioDac.h" 
#include "RadioTft.h"
#include "RadioController.h"
#include "TemperatureSensor.h"
// --- touch irq pin ---------------------------------
constexpr int TOUCH_IRQ_PIN2 = 36;     // GPIO 0
volatile bool touchFlag = false;     // set in ISR
void IRAM_ATTR touchIsr() {          // interrupt routine
  touchFlag = true;                  // just set a flag, keep it tiny
}

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_IRQ_PIN2, INPUT_PULLUP);          // keep line high at boot
  attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ_PIN2),
                  touchIsr, FALLING);
  tft_init();
  startTemperatureMonitoring();
  connectToWiFi();
  
  int lastStation = getLastRadioStationFromFlash();
  if (lastStation != -1) {
    Serial.println("Odczytano ostatnią stację: " + String(lastStation));
    currentRadioStation = {menuItems[lastStation], "", ""};
    configureAudio(currentRadioStation.item.url.c_str());
  } else {
    saveCurrentRadioStationToFlash(0);
    currentRadioStation = {menuItems[0], "", ""};
    configureAudio(menuItems[0].url.c_str());
  }
  tft_drawHeader(currentRadioStation.item.name);
  

  Serial.println("Max volume: "+ audio.maxVolume());
  Serial.println("Max volume: "+ audio.maxVolume());


}
void loop() {
  audio.loop();
  if (!touchFlag) return;
  touchFlag = false;
  static uint32_t lastTouchCheck = 0;
  static bool wasTouched = false;
  uint16_t touchX, touchY;

  if (millis() - lastTouchCheck > 50) {
    lastTouchCheck = millis();

    if (tft.getTouch(&touchX, &touchY)) {
      if (!wasTouched) {
        wasTouched = true;
        Serial.printf("Touch detected at X=%u, Y=%u\n", touchX, touchY);

        // 1) Hard-coded burger button in the corner
        if (touchX >= 285 && touchX <= 325 &&
            touchY >= 185 && touchY <= 235) {
          Serial.println("Burger touched!");
          tft_burgerClicked();
        }
        // 2) Otherwise, if menu is open, hit-test the boxes
        else if (isMenuOpen) {
          const int numItems = 3;
          const int spacing  = 4;
          int availableWidth = tft.width() - spacing * (numItems - 1);
          int rectWidth      = availableWidth / numItems;
          int rectHeight     = 30;
          int rectY          = (tft.height() - rectHeight) / 2;

          for (int i = 0; i < numItems; i++) {
            int rectX = i * (rectWidth + spacing);
            if (touchX >= rectX && touchX <  rectX + rectWidth &&
                touchY >= rectY && touchY <  rectY + rectHeight) {
              Serial.printf("Menu item %d touched\n", i);
              changeRadioStation(i);
              break;
            }
          }
        }
      }
    } else {
      wasTouched = false;  // reset on release
    }
  }
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
