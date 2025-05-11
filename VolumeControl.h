#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <Arduino.h>
#include "RadioTft.h"
#include "AudioDac.h"

#define POTENTIOMETER_PIN 32
#define MAX_VOLUME 64
#define MIN_VOLUME 0
#define ADC_RESOLUTION 4095

void initVolumeControl();
void startVolumeMonitoring();
void setVolume(int volume);
void displayVolumePercentage(int volumePercentage);

#endif
