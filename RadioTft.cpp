#include <RadioTft.h>
#include <HTTPClient.h>
#include <WiFi.h>

TFT_eSPI tft = TFT_eSPI(); 

void tft_init() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft_drawHeader("Eska Warszawa");
  tft_drawMenu();
  
  // Draw lines to separate sections Header and Song Info
  tft.drawLine(10, 45, tft.width() - 10, 45, TEXT_COLOR);

  // Draw lines to separate sections Song Info and Footer
  tft.drawLine(10, 180, tft.width() - 10, 180, TEXT_COLOR);
}

void tft_printTitle(String title) {
  tft.setTextSize(2);
  tft.setCursor(15, 60, 1);
  tft.print(title);
}

void tft_printAuthor(String author) {
  tft.setTextSize(1);
  tft.setCursor(15, 120, 1);
  tft.print(author);
}
void tft_initImageShow(String title, String author) {
  title.replace(" ", "%20");
  author.replace(" ", "%20");
  String url = String(BASE_SERVER_URL) + "?title=" + title + "&author=" + author;
  showRawRGB888ImageFromURL(url.c_str());
}

void tft_clearSongInfo() {
  tft.fillRect(0, 46, tft.width() / 2, 120, BG_COLOR);
}

void tft_drawHeader(String name, int x, int y) {
  tft.setTextColor(TEXT_COLOR);
  tft.setTextSize(2);
  tft.drawString(name, 10, 10);
}
void tft_drawMenu() {
  int menuX = tft.width() - 40;
  for(int i = 0; i < 3; i++) {
    tft.fillRect(menuX, 15 + (i * 10), 30, 5, TEXT_COLOR);
  }
}

void updateSong(String title, String author) {
  tft_clearSongInfo();
  tft_printTitle(title);
  tft_printAuthor(author);
  tft_initImageShow(title, author);
}

// Read one byte with retries
int readByteWithRetry(WiFiClient* stream) {
  int value = -1;
  int retries = 0;
  const int MAX_RETRIES = 10;
  
  while (value == -1 && retries < MAX_RETRIES) {
    value = stream->read();
    if (value == -1) {
      delay(2);
      retries++;
    }
  }
  
  return value;
}

void showRawRGB888ImageFromURL(const char* url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    WiFiClient* stream = http.getStreamPtr();
    int x = (tft.width() + 50) / 2;
    int y = (tft.height() - 100) / 2;
    
    for (int row = 0; row < 100; row++) {
      for (int col = 0; col < 100; col++) {
        int r = readByteWithRetry(stream);
        int g = readByteWithRetry(stream);
        int b = readByteWithRetry(stream);
        
        if (r == -1 || g == -1 || b == -1) {
          Serial.println("Failed to read pixel data");
          continue;
        }

        uint16_t pixelColor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        tft.drawPixel(x + col, y + row, pixelColor);
      }
    }
    
    Serial.println("Image displayed successfully");
  } else {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
}
