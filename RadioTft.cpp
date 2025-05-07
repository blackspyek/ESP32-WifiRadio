#include <RadioTft.h>
#include <HTTPClient.h>
#include <WiFi.h>

TFT_eSPI tft = TFT_eSPI(); 
Preferences prefs;
menuItem menuItems[3] = {
    {"Juwenalia", "http://158.101.190.156:8000/a55d4ea54bb8159dfec8e7ece1c0fa5c.mp3"},
    {"RMF Maxx", "http://31.192.216.7:8000/rmf_maxxx"},
    {"357", "http://stream.rcs.revma.com/ye5kghkgcm0uv"},
};
RadioStation currentRadioStation = {"", "", ""};
bool isMenuOpen = false;
void tft_init() {
  loadRadiosFromFlash();
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft_drawHeader(menuItems[0].name);
  tft_drawMenu();
  
  // Draw lines to separate sections Header and Song Info
  tft.drawLine(10, 45, tft.width() - 10, 45, TEXT_COLOR);

  // Draw lines to separate sections Song Info and Footer
  tft.drawLine(10, 180, tft.width() - 10, 180, TEXT_COLOR);
}

void tft_printTitle(String title) {
  tft.setTextColor(TEXT_COLOR, BG_COLOR); 
  tft.setTextSize(2);
  tft.setCursor(15, 60, 1);
  tft.print(title);
}

void tft_printAuthor(String author) {
  tft.setTextColor(TEXT_COLOR, BG_COLOR); 
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
  tft.fillRect(0, 46, tft.width(), 135, BG_COLOR);
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
