#ifndef RADIOTFT_H
#define RADIOTFT_H
#include <TFT_eSPI.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "Secrets.h"
#include <Preferences.h>


#define BG_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE
#define TOUCH_IRQ_PIN 0

extern TFT_eSPI tft;
extern Preferences prefs;


struct menuItem {
    String name;
    String url;
};
struct RadioStation {
    menuItem item;
    String title;
    String author;
};
extern menuItem menuItems[3];
extern RadioStation currentRadioStation;
void saveRadioURL(int radioStationNumber, const String& name, const String& url);
void loadRadiosFromFlash();
void saveCurrentRadioStationToFlash(int radioStationNumber);
int getLastRadioStationFromFlash();

// struct that holds pointer to previous menuView and current menuView

extern bool isMenuOpen;

void tft_init();
void tft_printTitle(String title);
void tft_printAuthor(String author);
void tft_printCurrentSongInfo();
void tft_printWrapped(const String &text, int x, int y, uint8_t textSize);
void tft_drawHeader(String name, int x = 10, int y = 10);
void tft_drawMenu();

void tft_drawMenuScreen();

void tft_initImageShow(String title, String author);

void tft_clearSongInfo();
void tft_OpenMenu();
void tft_CloseMenu();
void tft_burgerClicked();

void updateSong(String title, String author);

int readByteWithRetry(WiFiClient* stream);
void showRawRGB888ImageFromURL(const char* url);

#endif
