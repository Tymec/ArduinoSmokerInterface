#include <DallasTemperature.h>

float getTemp(DallasTemperature* tempSensor) {
  tempSensor->requestTemperatures();
  return tempSensor->getTempCByIndex(0);
}

unsigned short getSizeOfInt(int i) {
  char buffer[6];
  sprintf(buffer, "%d", abs(i));
  return(strlen(buffer));
}

unsigned short getSizeOfFloat(float i) {
  char buffer[10];
  dtostrf(i, 5, 1, buffer);
  return(strlen(buffer) - 1);
}
