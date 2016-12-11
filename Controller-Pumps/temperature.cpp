#include <OneWire.h>
#include <DS18B20.h>
#include "temperature.h"

// Pin on which DS1820 have communication
#define ONEWIRE_PIN 2

// How many sesnsors?
#define SENSORS_NUM 2

// Sensors addresses
const byte address[SENSORS_NUM][8] = {
  0x28, 0xD2, 0xFD, 0xAA, 0x8, 0x0, 0x0, 0x69,    //INTERNAL (C.O.)
  0x28, 0x5E, 0x6D, 0xAC, 0x8, 0x0, 0x0, 0x40     //WATER (C.W.)
};

float temperature[2];
bool sensorsError = false;
static OneWire onewire(ONEWIRE_PIN);
static DS18B20 sensors(&onewire);

void beginTemperatures() {
  sensorsError = sensors.begin();
  sensors.request();  
}

void updateTemperatures() {
  if (sensors.available()) {
    for (byte i = 0; i < SENSORS_NUM; i ++) {
      temperature[i] = sensors.readTemperature(address[i]);
      if (temperature[i] < -273) {
        sensorsError = true;
      }
    }

    sensors.request();
  }
}

