#include "VolumeControl.h"

TaskHandle_t volumeTaskHandle = NULL;
volatile int lastVolumePercentage = -1;
volatile bool volumeChanged = false;
portMUX_TYPE volumeMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR volumeReadingISR() {
  portENTER_CRITICAL_ISR(&volumeMux);
  volumeChanged = true;
  portEXIT_CRITICAL_ISR(&volumeMux);
}

void initVolumeControl() {

    pinMode(POTENTIOMETER_PIN, INPUT);
  
  hw_timer_t * timer = timerBegin(0, 80, true); // Prescaler 80
  timerAttachInterrupt(timer, &volumeReadingISR, true);
  timerAlarmWrite(timer, 100000, true); // 100ms
  timerAlarmEnable(timer);
}

void setVolume(int volumeLevel) {

    volumeLevel = constrain(volumeLevel, MIN_VOLUME, MAX_VOLUME);
    audio.setVolume(volumeLevel);
}

void displayVolumePercentage(int volumePercentage) {

    tft.fillRect(tft.width() - 100, tft.height() - 30, 90, 25, BG_COLOR);
    tft.setTextColor(TEXT_COLOR, BG_COLOR);
    tft.setTextSize(2);
  
    char volStr[10];
    sprintf(volStr, "%d%%", volumePercentage);
    tft.drawString(volStr, tft.width() - 70, tft.height() - 30);
}

void volumeTask(void * parameter) {
  for(;;) {
    if (volumeChanged) {
      portENTER_CRITICAL(&volumeMux);
      volumeChanged = false;
      portEXIT_CRITICAL(&volumeMux);
      
      int potValue = analogRead(POTENTIOMETER_PIN);
      
      int volumeLevel = map(potValue, 0, ADC_RESOLUTION, MIN_VOLUME, MAX_VOLUME);
      
      int volumePercentage = map(volumeLevel, MIN_VOLUME, MAX_VOLUME, 0, 100);
      
      if (volumePercentage != lastVolumePercentage) {
        lastVolumePercentage = volumePercentage;
        setVolume(volumeLevel);
        displayVolumePercentage(volumePercentage);
      }
    }
    
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void startVolumeMonitoring() {
  xTaskCreate(
    volumeTask,        
    "VolumeTask",     
    2048,              
    NULL,             
    1,                 
    &volumeTaskHandle  
  );
}
