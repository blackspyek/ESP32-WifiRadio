#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

#include <Audio.h>
#include <Preferences.h>
extern Preferences prefs;
extern int currVolume;
void changeRadioStation(int stationNumber);
void changeVolume(int volume);
void volume_up();
void volume_down();
int getLastVolumeFromFlash();
void saveCurrentVolumeToFlash(int volume);
#endif
