/* Copyright (C) 2022 Samy Bennabi
 * All rights reserved.
 * Projet d'intégration 2
 * 
 * Ecole du Web
 * Cours Systèmes embarqués (c)2022
 *  
    @file           : MyDHT.h
    @author         : Samy Bennabi
    @version        : 1.0   04/05/22
    @description    : Classe permettant d'utiliser le senseur DHT

    platform = ESP32
    OS = Arduino
    Langage : C++

    Historique des versions
        Version    Date       Auteur       Description
        1.0        04/05/22   Samy       Première version du logiciel

**/
#ifndef MYDHT_H
#define MYDHT_H

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class MyDHT
{
    private :
    sensor_t sensor;
    DHT_Unified *dht;
    sensors_event_t event;

    public :

    MyDHT(uint8_t DHTPIN, uint8_t DHTTYPE);
    ~MyDHT(){};

    void printSensorDetails();
    float getTemp();
    void printTemp();
};

#endif