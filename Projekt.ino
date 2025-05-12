#include <Arduino.h>
#include <Audio.h>
#include "WifiConfig.h"
#include "AudioDac.h" 
#include "RadioTft.h"
#include "RadioController.h"
#include "StationServer.h"

// --- touch irq pin ---------------------------------
constexpr int TOUCH_IRQ_PIN2 = 35;     // GPIO 0


volatile bool touchFlag = false;     // set in ISR
void IRAM_ATTR touchIsr() {          // interrupt routine
  touchFlag = true;                  // just set a flag, keep it tiny
}
void audioTask(void *param) {
  while (true) {
      audio.loop();
      vTaskDelay(1 / portTICK_PERIOD_MS); 
  }
}

QueueHandle_t songInfoQueue;

struct SongInfo {
  char title[128];
  char author[128];
};
void songUpdateTask(void *param) {
  SongInfo info;
  while (true) {
    if (xQueueReceive(songInfoQueue, &info, portMAX_DELAY)) {
      currentRadioStation.title = String(info.title);
      currentRadioStation.author = String(info.author);

      if (!isMenuOpen)
        updateSong(currentRadioStation.title, currentRadioStation.author);
    }
  }
}

void httpServerTask(void *param) {
  WiFiServer *server = static_cast<WiFiServer *>(param);

  while (true) {
    WiFiClient client = server->accept();
    if (client) {
      while (client.connected()) {
        if (client.available()) {
          handleRequest(client); // Twoja funkcja do obsługi HTTP
          break;
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // mały oddech
  }
}
void setup() {
  Serial.begin(115200);

  delay(300);
  pinMode(TOUCH_IRQ_PIN2, INPUT_PULLUP);          // keep line high at boot
  attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ_PIN2),
                  touchIsr, FALLING);
  tft_init();

  connectToWiFi();
  
  int lastStation = getLastRadioStationFromFlash();
  int lastVolume = getLastVolumeFromFlash();
  currVolume = lastVolume;
  if (lastStation != -1) {
    Serial.println("Odczytano ostatnią stację: " + String(lastStation));
    currentRadioStation = {menuItems[lastStation], "", ""};
    configureAudio(currentRadioStation.item.url.c_str(), lastVolume);
  } else {
    saveCurrentRadioStationToFlash(0);
    currentRadioStation = {menuItems[0], "", ""};
    configureAudio(menuItems[0].url.c_str(), lastVolume);
  }
  tft_drawHeader(currentRadioStation.item.name);
  

  server.begin();
  xTaskCreatePinnedToCore(httpServerTask, "HTTPTask", 4096, &server, 1, NULL, 1);
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 4096, NULL, 3, NULL, 1);
  songInfoQueue = xQueueCreate(5, sizeof(SongInfo));
  xTaskCreatePinnedToCore(songUpdateTask, "SongUpdate", 12288, NULL, 1, NULL, 1);
}
void loop() {
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
        else if (touchX >= 5 && touchX <= 50 &&
          touchY >= 0 && touchY <= 60) {
          volume_down();
        }
        // 3) Plus
        else if (touchX >= 70 && touchX <= 120 &&
                  touchY >= 0 && touchY <= 60) {
                    volume_up();
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
  const char *prefix = "StreamTitle='";
  const char *start = strstr(info, prefix);

  if (start) {
    start += strlen(prefix);
    const char *end = strchr(start, '\'');

    if (end) {
      int len = end - start;
      char streamTitle[128];
      strncpy(streamTitle, start, len);
      streamTitle[len] = '\0';

      String fullTitle = String(streamTitle);
      fullTitle.trim();

      String author = "Unknown";
      String title = fullTitle;

      int sepIndex = fullTitle.indexOf(" - ");
      if (sepIndex != -1) {
        author = fullTitle.substring(0, sepIndex);
        title = fullTitle.substring(sepIndex + 3);
      }

      SongInfo infoToSend;
      strncpy(infoToSend.title, title.c_str(), sizeof(infoToSend.title));
      strncpy(infoToSend.author, author.c_str(), sizeof(infoToSend.author));
      infoToSend.title[sizeof(infoToSend.title) - 1] = '\0';
      infoToSend.author[sizeof(infoToSend.author) - 1] = '\0';

      xQueueSend(songInfoQueue, &infoToSend, 0);  // nie blokuj callbacka
    }
  }
}
