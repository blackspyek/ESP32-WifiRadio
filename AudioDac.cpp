#include "AudioDac.h"
#include <Audio.h>
#include <WiFi.h>
#include "RadioController.h"

// Deklaracja obiektów Audio
Audio audio;  // Obiekt audio

// Funkcja konfigurująca audio
void configureAudio(const char* radioURL, int volume) {

  // Ustawienia pinów I2S
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolumeSteps(21); 
  // Ustawienie głośności (0-15)
  audio.setVolume(volume);

  audio.setBufsize(16384, 65536);

  // Połączenie z serwisem audio (np. strumieniem radia)
  audio.connecttohost(radioURL);
  
  Serial.println("Audio configured and connected to host.");
}



// Funkcja callback wywoływana po zakończeniu pliku MP3
void audio_eof_mp3(const char *info) {

}
