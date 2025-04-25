#ifndef RADIOTFT_H
#define RADIOTFT_H
#include <TFT_eSPI.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "Secrets.h"

#define BG_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE


extern TFT_eSPI tft;

void tft_init();

void tft_printTitle(String title);
void tft_printAuthor(String author);

void tft_drawHeader(String name, int x = 10, int y = 10);
void tft_drawMenu();

void tft_initImageShow(String title, String author);

void tft_clearSongInfo();

void updateSong(String title, String author);

int readByteWithRetry(WiFiClient* stream);
void showRawRGB888ImageFromURL(const char* url);

#endif
