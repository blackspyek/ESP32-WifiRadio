#ifndef AUDIODAC_H
#define AUDIODAC_H

#include <Audio.h>
#include <WiFi.h>
#include <Arduino.h>

// Zdefiniuj piny I2S
#define I2S_BCLK      26  // Bit clock
#define I2S_LRC       25  // Left/Right Clock
#define I2S_DOUT      27  // Data Out


extern Audio audio;  // Obiekt audio

// Funkcje konfigurujące audio
void configureAudio(const char* radioURL, int volume);

// Funkcje callback do obsługi audio
void audio_info(const char *info);
void audio_eof_mp3(const char *info);

#endif
