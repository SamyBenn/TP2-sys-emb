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
        1.1        22/08/15   Alain       Première version du logiciel
        1.2        01/05/22   Samy        Lecture de temperature avec senseur DHT22 (non oriente-objet)
        1.3        05/05/22   Samy        Temperature en oriente objet et changement du splash screen
        1.4        06/05/22   Samy        bouton sur la pin pour demarrer le four
        1.5        07/05/22   Samy        Logique pour le compteur
        1.6        10/05/22   Samy        Ecriture des temperature et compteur sur l'ecran stone
        1.7        11/05/22   Samy        Bouton sur l'ecran stone fonctionnel
        2.0        12/05/22   Samy        Corrections finales pour la remise et supression de la classe MyButton

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

// pinout stone
#define RXD2 18//16
#define TXD2 19//17
#define BAUD_RATE 115200

// pinout dht
#define DHTPIN 27
#define DHTTYPE DHT22

#include <iostream>

#include "MyStone.h"
MyStone *myStone;
bool btnDemarrer;

// initialization pour dht22
MyDHT *temp;

bool fourOn = false;
short tempMin = 24;
short tempMax = tempMin * 1.1;
short compteur = 0;
short delayMS = 1000;
void compteurReset(short& i){ i = 0; }

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
  switch(rd.id)
  {
    case 0x0002:  //Version
    {
      std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
      std::string stoneVersion = rd.name;
      std::cout << "Version : " <<  stoneVersion.c_str() << "\n";
      //std::this_thread::sleep_for(std::chrono::milliseconds(10));
      break;
    }

    case 0x1001:  //Button
    {
      std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
      std::string stoneButton = rd.name;
      std::cout << "Button : " <<  stoneButton.c_str() << "\n";
      // si le "btn_demarrer" est appuye on garde l'information
      if(stoneButton == "btn_demarrer" && rd.type == 2)
      {
        btnDemarrer = true;
      }
      break;
    }
      
  }

  if(rd.id<0) std::cout << "Data received ( id: : " << intToHexa(abs(rd.id)) << "  Command: " << rd.command << " Type: " << rd.type<< ")\n";
}

void setup() {
  Serial.begin(9600);
  
  //Affichage des infos du Stone
  Serial.println("Stone serial Txd is on pin: "+String(TXD2));
  Serial.println("Stone serial Rxd is on pin: "+String(RXD2));
  myStone = new MyStone(115200, SERIAL_8N1, RXD2, TXD2);

  // Envoi de la commande pour obtenir la version du stone
  myStone->getVersion();

  // delai puis changement du splash screen vers l'ecran principal
  delay(2000);
  myStone->changePage("main");
  Serial.println("la version de stone");

  // lecture des donnees envoyees du stone pour obtenir la version
  readStoneData();

  
  //initialization de l'objet DHT
  temp = new MyDHT(DHTPIN, DHTTYPE);

  // obtenir et ecrire les details du senseur
  temp->printSensorDetails();
}

void loop() { 
  // lecture des donnees envoyees par le stone
  readStoneData();

  // Si le bouton demarrer est appuye on demarre le four
  if(btnDemarrer)
  {
    Serial.println("Four Demarre");
    fourOn = true;
    // reset de l'etat du bouton pour pouvoir le reutiliser
    btnDemarrer = false;
  }

  // logique du four, si le four est allume on fait:
  if(fourOn)
  {
    // affichage de la temperature dans le terminal
    temp->printTemp();
    // garder la derniere valeur obtenue de la temperature
    float temperature = temp->getTemp();
    
    // Si la temperature est entre 25 est 26 : incrementer le compteur
    if(temperature >= tempMin && temperature < tempMax){ compteur ++; }
    // afficher sur le terminal le compteur
    Serial.print("Time : ");
    Serial.println(compteur);

    // formater et afficher la temperature sur le label du stone
    char strTemperature[64];
    sprintf(strTemperature, "%g Celcius", temperature);
    myStone->setLabel("lbl_temp", strTemperature);

    // formater et afficher la valeur du compteur sur la label du stone
    char strCompteur[64];
    sprintf(strCompteur, "%ds/20s", compteur);
    myStone->setLabel("lbl_comp", strCompteur);
  }
  
  // quand le compteur atteint 20
  if(compteur == 20)
  { 
    // eteindre le four, remettre le comteur a zero et afficher que le bois a brule a la place du compteur
    fourOn = false; 
    compteurReset(compteur);
    delay(delayMS);
    myStone->setLabel("lbl_comp", "Le bois a seché");
  }
  // delai 1 seconde
  delay(delayMS);
}