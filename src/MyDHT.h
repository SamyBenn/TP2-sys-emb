#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class MyDHT{

private :

public :
    sensor_t sensor;
    DHT_Unified *dht;
    sensors_event_t event;

    MyDHT(uint8_t DHTPIN, uint8_t DHTTYPE);
    ~MyDHT(){};

    void printSensorDetails();
    float getTemp();
    void printTemp();
};