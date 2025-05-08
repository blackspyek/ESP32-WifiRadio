#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>
#include <DS18B20.h>
#include "RadioTft.h"

void initTemperatureSensor();
float readTemperature();
void startTemperatureMonitoring();
void displayTemperature(float temperature);

#endif
