#include <Arduino.h>
#include "RadioTft.h"
#include "AudioDac.h"

void changeRadioStation(int stationNumber)
{
    if (stationNumber < 0 || stationNumber >= sizeof(menuItems) / sizeof(menuItems[0])) {
        Serial.println("Invalid station number");
        return;
    }
    
    String url = menuItems[stationNumber].url;
    Serial.print("Changing radio station to: ");
    Serial.println(url);
    currentRadioStation.item = menuItems[stationNumber];
    saveCurrentRadioStationToFlash(stationNumber);
    
    audio.stopSong();
    audio.connecttohost(url.c_str());
    
    tft_drawHeader(menuItems[stationNumber].name);
}


void changeVolume(int volume)
{
    if (volume < 0 || volume > 100) {
        Serial.println("Invalid volume level");
        return;
    }
    
    audio.setVolume(volume);
    Serial.print("Volume set to: ");
    Serial.println(volume);
}