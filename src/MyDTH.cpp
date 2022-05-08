#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "MyDHT.h"

MyDHT::MyDHT(uint8_t DHTPIN, uint8_t DHTTYPE)
{
    MyDHT::dht = new DHT_Unified(DHTPIN, DHTTYPE);
    // Initialize device.
    dht->begin();
    Serial.println(F("DHTxx Unified Sensor Example"));
    // Print temperature sensor details.
    dht->temperature().getSensor(&sensor);
};

void MyDHT::printSensorDetails()
{
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
};

float MyDHT::getTemp()
{
    dht->temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
        return 0;
    }
    else {
        return(event.temperature);
    }
};

void MyDHT::printTemp()
{
    dht->temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
    }
    else {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
    }
};