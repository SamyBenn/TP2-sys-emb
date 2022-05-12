/* Copyright (C) 2022 Alain Dube
 * All rights reserved.
 *
 * Projet Stone de base
 * Ecole du Web
 * Cours Systèmes embarqués (c)2022
 *
    @file     main.cpp
    @author   Alain Dubé
    @version  1.1 22/08/15
    @description
      Démonstration comment utiliser le PORT SERIE pour accèder aux fonctionnalités
      de l'écran STONE en utilisant la classe MyStone et MySerial

    platform = ESP32
    OS : Arduino
    Langage : C++

    Historique des versions
        Version    Date       Auteur       Description
        1.1        22/08/15  Alain       Première version du logiciel
        1.2        05/05/22   Samy        Lecture de temperature avec senseur DHT22 (non oriente-objet)

    Fonctionnalités implantées
        Lecture des evénements envoyés par l'écran
        Envoyer une commande à l'écran
          Optenir la version du Firmware de l'écran

        Affichage des infos du senseur DHT22 dans le terminal
        Lecture de la temperature
        Affichage de la temperature dans le terminal
 * */

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "MyDHT.h"

// stone
#define RXD2 18//16
#define TXD2 19//17
#define BAUD_RATE 115200

// dht
#define DHTPIN 27
#define DHTTYPE DHT22

#include <iostream>

#include "MyButton.h"
MyButton *myButtonT4 = new MyButton();
MyButton *myButtonT5 = new MyButton();

#include "MyStone.h"
MyStone *myStone;

std::string intToHexa(int value){
  char buffer[10];
  sprintf(buffer , "0x%4X", value);
  return (buffer);
};

std::string charToString(char str, std::string formatStr){
  char buffer[10];
  sprintf(buffer , formatStr.c_str(), str);
  return (buffer);
};

std::string intToString(int value, std::string formatStr){
  char buffer[10];
  sprintf(buffer , formatStr.c_str(), value);
  return (buffer);
};

//Thread qui permet de LOOPER et lire le contenu du port serie
//avec l'aide du la fonction getValidsDatasIfExists
void readStoneData() {
  datasRead rd = myStone->getValidsDatasIfExists();
  //std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
  switch(rd.id){

      case 0x0002: { //Version
          std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
          std::string stoneVersion = rd.name;
          std::cout << "Version : " <<  stoneVersion.c_str() << "\n";

          //std::this_thread::sleep_for(std::chrono::milliseconds(10));

          break;
          }
      }

  if(rd.id<0) std::cout << "Data received ( id: : " << intToHexa(abs(rd.id)) << "  Command: " << rd.command << " Type: " << rd.type<< ")\n";
}

// initialization pour dht22
MyDHT *temp;

bool fourOn = false;
int compteur = 0;
short delayMS = 1000;
void compteurReset(int& i){ i = 0; }

/*void countdown(MyDHT& temp, int& compteur)
{
  temp->printTemp();
  if(temp->getTemp() > 25){ compteur += -1; }
  if(compteur == 0) { fourOn = false; }
  Serial.print("Time Remaining : ");
  Serial.println(compteur);
}*/

void setup() {
  Serial.begin(9600);
  
  // testing equipment
  myButtonT4->init(T4);
  myButtonT4->autoSensibilisation(); //Trouve la sensibilité automatiquement

  myButtonT5->init(T5);
  myButtonT5->autoSensibilisation(); //Trouve la sensibilité automatiquement
  
  //Stone
  Serial.println("Stone serial Txd is on pin: "+String(TXD2));
  Serial.println("Stone serial Rxd is on pin: "+String(RXD2));
  myStone = new MyStone(115200, SERIAL_8N1, RXD2, TXD2);

  cout << std::string("Début de l'exemple Stone de base pour le ESP32")  << "\n";

  readStoneData();
  Serial.println("le version de stone");
  delay(1000);
  myStone->changePage("main");
  myStone->getVersion();

  
  //DHT
  temp = new MyDHT(DHTPIN, DHTTYPE);

  temp->printSensorDetails();
}

void loop() { 

  // Delay between measurements.
  int buttonActionT4 = myButtonT4->checkMyButton();
  if(buttonActionT4 > 2)  //Si appuyé plus de 0.2 secondes
  {  
    Serial.println("Four Demarre");
    fourOn = true;
  }

  int buttonActionT5 = myButtonT5->checkMyButton();
  if(buttonActionT5 > 2) //Si appuyé plus de 0.2 secondes
  {
    Serial.println("Four arrete & reset");
    compteurReset(compteur);
    fourOn = false;
  }

  if(fourOn)
  {
    temp->printTemp();
    float temperature = temp->getTemp();
    
    if(temperature >= 25){ compteur ++; }
    if(compteur == 21) { fourOn = false; }
    Serial.print("Time : ");
    Serial.println(compteur);


    readStoneData();
    char strTemperature[64];
    sprintf(strTemperature, "%g", temperature);
    myStone->setLabel("lbl_temp", strTemperature);

    /*char strCompteur[64];
    sprintf(strCompteur, "%d", compteur);
    myStone->setLabel("lbl_compteur", strCompteur);*/
  }else{
    Serial.println("NO!");
  }
  delay(delayMS);

  // bouton iteragissants avec stone
  /*int buttonActionT4 = myButtonT4->checkMyButton();
      if(buttonActionT4 > 2)  {  //Si appuyé plus de 0.2 secondes
            Serial.println("Button T4 pressed");
            //if(myStone) myStone->changePage("main");
            
            }

  int buttonActionT5 = myButtonT5->checkMyButton();
      if(buttonActionT5 > 2)  {  //Si appuyé plus de 0.2 secondes
            Serial.println("Button T5 pressed");
            if(myStone) myStone->getVersion();
          }*/
}