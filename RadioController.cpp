#include <Arduino.h>
#include "RadioController.h"
#include "RadioTft.h"
#include "AudioDac.h"

int currVolume = 2;
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
    if (volume < 0 || volume > 10) {
        return;
    }
    currVolume = volume;
    tft_displayVolume(volume);
    audio.setVolume(volume);
    saveCurrentVolumeToFlash(volume);
}
void volume_up()
{
    if (currVolume < 10) {
        currVolume++;
        changeVolume(currVolume);
    }
}
void volume_down()
{
    if (currVolume > 0) {
        currVolume--;
        changeVolume(currVolume);
    }
}
void saveCurrentVolumeToFlash(int volume)
{
    prefs.begin("radio", false);
    prefs.putInt("volume", volume);
    prefs.end();
}
int getLastVolumeFromFlash()
{
    prefs.begin("radio", true);
    int volume = prefs.getInt("volume", 2);
    prefs.end();
    return volume;
}