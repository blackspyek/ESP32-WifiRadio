#include "TemperatureSensor.h"

DS18B20 ds(14); 
bool sensorWasConnected = false;
TaskHandle_t temperatureTaskHandle = NULL;
unsigned long lastSuccessfulRead = 0;
float lastValidTemperature = 0;  
int failedReadCount = 0;   

void initTemperatureSensor() {
    delay(1000);
    if (ds.selectNext()) {
      sensorWasConnected = true;
      lastSuccessfulRead = millis();
    } else {
      sensorWasConnected = false;
    }
}

float readTemperature() {

    bool sensorConnected = ds.selectNext();

    if (sensorConnected) {
      failedReadCount = 0;
      if (!sensorWasConnected) {
        delay(1000);
        sensorWasConnected = true;
      }
      
      lastSuccessfulRead = millis();
      float tempC = ds.getTempC();
      
      if (tempC == 85.0) {
        delay(750);
        ds.selectNext();
        tempC = ds.getTempC();
      }

      if (tempC != -127.0 && tempC != 85.0) {
        lastValidTemperature = tempC;  
      }

      return tempC;
    } 
    else {
      failedReadCount++; 

      if (failedReadCount < 5 && lastValidTemperature != 0) {
        return lastValidTemperature;
      }

     if (failedReadCount >= 5 || (sensorWasConnected && (millis() - lastSuccessfulRead > 5000))) {
        sensorWasConnected = false;
      }

      return NAN;
    }
  }

  void displayTemperature(float temperature) {
    tft.fillRect(10, tft.height() - 30, 170, 28, BG_COLOR);
    
    tft.setTextColor(TEXT_COLOR, BG_COLOR);
    tft.setTextSize(2);
    
   if (!isnan(temperature) && temperature != -127.0 && temperature != 85.0) {
    char tempStr[10];
    sprintf(tempStr, "%.1f C", temperature);
    tft.drawString(tempStr, 10, tft.height() - 30);
    } else {
        if (!sensorWasConnected || failedReadCount >= 5) {
          tft.drawString("Blad czujnika", 10, tft.height() - 30);
        } else if (lastValidTemperature != 0) {
          char tempStr[10];
          sprintf(tempStr, "%.1f C", lastValidTemperature);
          tft.drawString(tempStr, 10, tft.height() - 30);
    }
  }
  }

  void temperatureTask(void * parameter) {
    for(;;) {
      float temperature = readTemperature();
      
      displayTemperature(temperature);
      
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void startTemperatureMonitoring() {
  xTaskCreate(
    temperatureTask,    
    "TemperatureTask",  
    2048,             
    NULL,               
    1,                  
    &temperatureTaskHandle 
  );
}
