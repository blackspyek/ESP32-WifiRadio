#include <RadioTft.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "AudioDac.h"
TFT_eSPI tft = TFT_eSPI(); 
Preferences prefs;
menuItem menuItems[3] = {
    {"Juwenalia", "http://158.101.190.156:8000/a55d4ea54bb8159dfec8e7ece1c0fa5c.mp3"},
    {"RMF Maxx", "http://31.192.216.7:8000/rmf_maxxx"},
    {"Paradise", "http://stream.radioparadise.com/mp3-192"},
};
RadioStation currentRadioStation = {{"", ""}, "", ""};
  
bool isMenuOpen = false;
void tft_init() {
  loadRadiosFromFlash();
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  
  tft_drawMenu();
  
  // Draw lines to separate sections Header and Song Info
  tft.drawLine(10, 45, tft.width() - 10, 45, TEXT_COLOR);

  // Draw lines to separate sections Song Info and Footer
  tft.drawLine(10, 180, tft.width() - 10, 180, TEXT_COLOR);
}

void tft_printWrapped(const String &text, int x, int y, uint8_t textSize) {
  tft.setTextColor(TEXT_COLOR, BG_COLOR); 
  tft.setTextSize(textSize);

  const int charW    = 6  * textSize;     // default font cell width
  const int charH    = 8  * textSize;     // default font cell height
  const int maxWidth = tft.width() / 2;   // wrap at half screen

  int cursorY = y;
  String line = "";

  int idx = 0;
  while (idx < text.length()) {
    int nextSpace = text.indexOf(' ', idx);
    if (nextSpace < 0) nextSpace = text.length();
    String word = text.substring(idx, nextSpace);

    // measure if adding this word would overflow
    int testLen = (line.length() + (line.length() ? 1 : 0) + word.length()) * charW;
    if (testLen <= maxWidth) {
      // fits on current line
      if (line.length()) line += " ";
      line += word;
    } else {
      // flush current line, start new one
      tft.setCursor(x, cursorY);
      tft.print(line);
      cursorY += charH;
      line = word;
    }

    idx = nextSpace + 1;
  }

  // print whatever’s left
  if (line.length()) {
    tft.setCursor(x, cursorY);
    tft.print(line);
  }
}

// now your two functions become trivial:

void tft_printTitle(String title) {
  tft_printWrapped(title, 15, 60, 2);
}

void tft_printAuthor(String author) {
  tft_printWrapped(author, 15, 120, 1);
}
void tft_initImageShow(String title, String author) {
  title.replace(" ", "%20");
  author.replace(" ", "%20");
  String url = String(BASE_SERVER_URL) + "?title=" + title + "&author=" + author;
  showRawRGB888ImageFromURL(url.c_str());
}

void tft_clearSongInfo() {
  tft.fillRect(0, 46, tft.width(), 133, BG_COLOR);
}

void tft_drawHeader(String name, int x, int y) {
  tft.fillRect(0, 0, tft.width() - 50, 45, BG_COLOR);
  tft.setTextColor(TEXT_COLOR, BG_COLOR);
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
void tft_printCurrentSongInfo() {
  tft_clearSongInfo();
  tft_initImageShow(currentRadioStation.title, currentRadioStation.author);
  tft_printTitle(currentRadioStation.title);
  tft_printAuthor(currentRadioStation.author);
}
void tft_OpenMenu() {
  isMenuOpen = true;
  tft_drawMenuScreen();

}
void tft_CloseMenu() {
  isMenuOpen = false;
  tft_clearSongInfo();
  tft_printCurrentSongInfo();
}
void tft_burgerClicked() {
  if (isMenuOpen) {
    tft_CloseMenu();
  } else {
    tft_OpenMenu();
  }
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
    //TODO default image
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
}

void tft_drawMenuScreen() {
  isMenuOpen = true;

  tft_clearSongInfo();

  const int numItems = 3;
  const int spacing = 4; // space between boxes
  int availableWidth = tft.width() - (spacing * (numItems - 1));
  int rectWidth = availableWidth / numItems;
  int rectHeight = 30; // fixed height
  int rectY = (tft.height() - rectHeight) / 2; // center vertically

  for (int i = 0; i < numItems; i++) {
    int rectX = i * (rectWidth + spacing);

    // Draw rectangle
    tft.fillRect(rectX, rectY, rectWidth, rectHeight, TEXT_COLOR);

    // Center text in box
    tft.setTextSize(1);
    int textW = tft.textWidth(menuItems[i].name);
    int textH = tft.fontHeight();
    int textX = rectX + (rectWidth - textW) / 2;
    int textY = rectY + (rectHeight - textH) / 2;

    tft.setTextColor(BG_COLOR); // invert for contrast
    tft.setCursor(textX, textY);
    tft.print(menuItems[i].name);
  }
}


void saveRadioURL(int radioStationNumber, const String& name, const String& url) {
  prefs.begin("radio", false);

  String key_name = "station" + String(radioStationNumber) + "_name";
  String key_url  = "station" + String(radioStationNumber) + "_url";

  prefs.putString(key_name.c_str(), name);
  prefs.putString(key_url.c_str(), url);

  prefs.end();
}

void loadRadiosFromFlash() {
  prefs.begin("radio", true);

  for (int i = 0; i < 3; i++) {
    String key_name = "station" + String(i) + "_name";
    String key_url  = "station" + String(i) + "_url";

    String name = prefs.getString(key_name.c_str(), "");
    String url  = prefs.getString(key_url.c_str(), "");

    if (name != "" && url != "") {
      menuItems[i].name = name;
      menuItems[i].url = url;
    }
  }

  prefs.end();
}

void saveCurrentRadioStationToFlash(int radioStationNumber) {
  prefs.begin("radio", false);

  String key_name = "lastStation";

  prefs.putString(key_name.c_str(), String(radioStationNumber));

  prefs.end();
}
int getLastRadioStationFromFlash() {
  prefs.begin("radio", true);

  String key_name = "lastStation";
  String lastStation = prefs.getString(key_name.c_str(), "-1");

  prefs.end();

  return lastStation.toInt();
}