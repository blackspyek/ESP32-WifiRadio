#ifndef AUDIODAC_H
#define AUDIODAC_H

#include <Audio.h>
#include <WiFi.h>
#include <Arduino.h>

#define I2S_BCLK      26  // Bit clock
#define I2S_LRC       25  // Left/Right Clock
#define I2S_DOUT      27  // Data Out


extern Audio audio; 

void configureAudio(const char* radioURL, int volume);
void audio_info(const char *info);
void audio_eof_mp3(const char *info);

#endif
