/**
 * @file CodeBatterie.c
 * @brief Programme de tests.
 * @author Guillaume.F
 * @version 12
 * @date 29 Avril 2016
 *
 * Programme de récupération d'information concernant le Vélomobile, batterie, vitesse, distance.
 *
 */

#include <RTClib.h>                                                   // Ulisation de l'horloge temps réel
#include <SD.h>                                                       // Librairie permettant de sauvegarder des informations sur une carte SD
#include <SPI.h>                                                      // Permet d'utiliser la communication via le bus SPI de l'arduino
#include <SoftwareSerial.h>                                           // Librairie permetteant de mettre en oeuvre le module Bluetooth
#include <Wire.h>                                                     // Permet d'utiliser la communication I2C
#include "rgb_lcd.h"                                                  // Librairie de l'écran LCD
#include <Ticks.h>                                                    // Compte des impulsions sur un certains laps de temps et en déduis la fréquence

#define LOG_INTERVAL 1000

SoftwareSerial Bluetooth(10, 11) ;                                    // RX | TX
rgb_lcd MonEcran                 ;

const byte ValeurCapteur = 2     ;                                    // Le capteur du compteur de vitesse est câblé à la broche numéro 2
const byte numInterrupt = 0      ;                                    // Numéro de la broche d'interruption
const int Periode = 500          ;                                    // Période en milliseconde, permet d'avoir la fréquence instantanée du passage de l'aimant

float Tension (0.0)              ;                                    // tension de la batterie
float Intensite  (0.0)           ;                                    // Intensite
float Capacite (0.0)             ;                                    // Capacité de la batterie en Ah
int Puissance (0.0)              ;                                    // Puissance délivrée par la batterie principale
float PuissanceConsommee (0.0)   ;                                    // Puissance consommée par la batterie en Wh
float PuissanceConsommeeKM (0.0) ;                                    // Puissance consommée par kilomètre par la batterie en Wh/km
float Charge (0.0)               ;                                    // Etat de charge de la batterie
float Vitesse (0.0)              ;                                    // Vitesse du vélo
float Distance (0.0)             ;                                    // Distance parcourue par le vélo
float Aimant (0)                 ;                                    // Passage de l'aimant du compteur de vitesse
int  BoutonChoixEcran (0)        ;                                    // Bouton permettant d'afficher sur l'écran LCD les tensions, l'intensité, la puissance et la distance parcourue
int CompteurBoucle (0)           ;                                    // Variable qui compte le nombre de fois que la boucle loop() à été faite
char ValeurPrecedente(0)         ;                                    // Valeur précédente du bouton permettant de bloquer l'écran
char Bouton (0)                  ;                                    // Variable qui va prendre la valeur de la lecture du bouton de blocage de l'écran
char ChangementEtat (0)          ;                                    // Variable qui enregistre l'état précédant le passage de l'aimant devant le capteur
char ValeurPrecedenteDist(0)     ;                                    // Variable qui va prendre la valeur de la variable ChangementEtat
unsigned long Temps = 0L         ;
unsigned long Intervalle = 0L    ;
float Perimetre (1.6)            ;                                    // Périmètre de la roue
const int chipSelect = 10        ;




RTC_DS1307 RTC; //Classe RTC_DS1307

File Rapport ;                                                        // Va permettre la création du fichier CSV

int Test     ;                                                        // Variable utilisée pour tester valeur renvoyée par fonctions SD Card

Ticks  Compteur (numInterrupt, ValeurCapteur, Periode) ;              // Appel du constructeur de la librairie Ticks permettant d'avoir accès aux fonctions associées

/**
 * @class Batterie
 * @brief Objet Batterie.
 *
 * Batterie représente la batterie moteur du Vélomobile, elle est définie par une tension, 
 * une intensite et sa capacite. 
 */

class Batterie                                                        // Création de la classe Batterie
{
  private:                                                            // Attributs
    float Tension   ;
    float Intensite ;
    float Puissance ;
    float Charge    ;

  public:                                                             // Méthodes
    Batterie()  ;                                                     // Constructeur par défaut
    ~Batterie() ;                                                     // Destructeur

    float Get_Tension ()   ;                                          // Méthode d'acquisition de la tension
    float Get_Intensite () ;                                          // Méthode d'acquisition de l'intensite

    float CalculerPuissance (float Tension, float Intensite) ;        // Méthode de calcul de la puissance
    float ChargeBatterie (float Tension)                     ;
};

/**
 * @class CapteurVitesse
 * @brief Objet Capteur de vitesse.
 *
 * Capteur qui va calculer la vitesse du Vélomobile.
 */

class CapteurVitesse                                                   // Création de la classe CapteurVitesse
{
  private:                                                            // Attributs
    float Vitesse     ;

  public:                                                             // Méthodes
    CapteurVitesse()  ;
    ~CapteurVitesse() ;

    float Get_Vitesse ()     ;                                       // Méthode d'acquisition de la vitesse                                     
};

/**
 * @class CapteurTemperature
 * @brief Objet Capteur de temperature.
 *
 * Capteur qui va calculer la temperature du moteur du Vélomobile.
 */

class CapteurTemperature                                             // Création de la classe CapteurTemperature
{
  private:
    float Temperature ;

  public:
    CapteurTemperature()  ;
    ~CapteurTemperature() ;

    float Get_Temperature() ;                                        // Méthode d'acquisition de la température
};

Batterie::Batterie():
  Tension(0.0),
  Intensite(0.0),
  Puissance(0.0),
  Charge(0.0)
{

}

Batterie::~Batterie()
{
  this->Tension = 0.0   ;
  this->Intensite = 0.0 ;
  this->Puissance = 0.0 ;
  this->Charge = 0.0    ;
}

CapteurVitesse::CapteurVitesse():
  Vitesse(0.0)
{

}

CapteurVitesse::~CapteurVitesse()
{
  this->Vitesse = 0.0     ;
}

CapteurTemperature::CapteurTemperature():
  Temperature(0.0)
{

}

CapteurTemperature::~CapteurTemperature()
{
  this->Temperature = 0.0 ;
}

float Batterie::Get_Tension()
{
  Tension = analogRead (0)               ;                              // Lire la Tension délivrée par la batterie
  Tension = Tension * 5 / 1023 * 10.2854 ;                              // Calcul permettant d'afficher la tension en volt

  return Tension ;
}

float Batterie::Get_Intensite()
{
  Intensite = analogRead (2)                           ;               // Lire l'intensité délivrée par la batterie principal
  Intensite = (Intensite * 5 / 1053) * (73.3 / 5) - 37 ;               // Calcul permettant d'afficher l'intensité en ampère

  return Intensite ;
}

/**
 * @brief Fonction qui permet de calculer la puissance délivrée par la batterie
 * @param Tension est la tension délivrée par la batterie
 * @param Intensite est le courant délivré par la batterie
 * @return Puissance
 */

float Batterie::CalculerPuissance(float Tension, float Intensite)
{
  Puissance = Tension * Intensite   ;                                  // Calcul de la puissance délivrée par la batterie principale

  return Puissance ;
}

float Batterie::ChargeBatterie(float Tension)                         // Calcul de la charge de la batterie (pourcentage)
{
  Charge = (Tension * 100) / 40 ;

  return Charge ;
}

/**
 * @brief Fonction qui calcule la vitesse du Vélomobile
 * @return Vitesse
 */

float CapteurVitesse::Get_Vitesse()
{
  Compteur.operate()                  ;                                // Fonction qui met à jour la fréquence instantanée
  Aimant = Compteur.TickRate1Period() ;                                // Nombre de passage de l'aimant par seconde
  Vitesse = Aimant * Perimetre * 3.6  ;                                // Calcul de la vitesse du vélo
  
  return Vitesse ;
}

/**
 * @brief Fonction qui calcule la température du moteur
 * @return Temperature
 */

float CapteurTemperature::Get_Temperature()
{
  Temperature = tmp006.readObjTempC() ;

  return Temperature ;
}

/**
 * @brief Methode qui permet d'envoyer de toutes les informations récupérées en bluetooth vers le smartphone Android
 * @param Tension est la tension délivrée par la batterie
 * @param Intensite est le courant délivré par la batterie
 * @param Puissance est la puissance délivrée par la batterie
 * @param Vitesse est la vitesse du Vélomobile
 * @param Distance est la distance parcourue par l'utilisateur
 */

void EnvoyerBluetooth(float Tension, float Intensite, float Puissance,
                      float Vitesse, float Distance, float Charge)               // Fonction d'affichage de la tension, de l'intensité, de la vitesse, de la distance sur smartphone
{                       
  Bluetooth.println(Tension)         ;                                           // Affichage de la tension sur le téléphone portable
                        
  Bluetooth.println(Intensite)       ;                                           // Affichage de l'intensité sur le téléphone portable
                        
  Bluetooth.println(Puissance)       ;                                           // Affichage de la puissance délivrée sur le téléphone portable
                                       
  Bluetooth.println(Vitesse)         ;                                           // Affichage de la vitesse sur le téléphone portable
                                    
  Bluetooth.println(Distance / 1000) ;                                           // Affichage de la distance parcourue sur le téléphone portable

  Bluetooth.println(Charge)          ;                                           // Affichage de la charge de la batterie sur le téléphone portable
}

/**
 * @brief Methode qui permet l'affichage des informations sur l'écran LCD
 * @param Tension est la tension délivrée par la batterie
 * @param Intensite est le courant délivré par la batterie
 * @param Vitesse est la vitesse du Vélomobile
 * @param Distance est la distance parcourue par l'utilisateur
 */

void AfficherInfo(float Tension, float Intensite, float Distance, float Vitesse)      // Fonction d'affichage de la tension, de l'intensité, de la distance et de la vitesse sur l'écran LCD
{
  MonEcran.setCursor(0, 0);// set the cursor to (0,0):
  MonEcran.print(Tension);
  MonEcran.print(" V ");
  MonEcran.setCursor(8, 0);// 8ème Caractère de la ligne 0
  MonEcran.print(Intensite);
  MonEcran.print(" A ");

  MonEcran.setCursor(0, 1);//  curseur Colonne 0 et Ligne 1
  MonEcran.print(Distance/1000);
  MonEcran.print("Km ");
  MonEcran.setCursor(7, 1);//  curseur Colonne 9 de la ligne 1
  MonEcran.print(Vitesse, 1);
  MonEcran.print(" Km/h ");
}

/**
 * @brief Methode qui permet l'affichage des informations sur l'écran LCD
 * @param Puissance est la puissance délivrée par la batterie
 * @param Capacite est la capacité de la batterie restante
 * @param PuissanceConsommee est la puissance consommée par la batterie en Wattheure
 */

void AfficherInfo2(int Puissance, float Capacite, float PuissanceConsommee)                           // Fonction d'affichage de la puisance délivrée et de la capacité de la batterie sur l'écran LCD
{
  MonEcran.setCursor(0, 0);// set the cursor to (0,0):
  MonEcran.print(Puissance);
  MonEcran.print(" W ");
  MonEcran.setCursor(8, 0);// 8ème Caractère de la ligne 0
  MonEcran.print(Capacite);
  MonEcran.print(" Ah ");

  MonEcran.setCursor(0, 1) ;
  MonEcran.print(PuissanceConsommee) ;
  MonEcran.print(" Wh ") ;
}

/**
 * @li Creation de l'objet BatterieVelo de type Batterie
 * @li Creation de l'objet CapteurVitesse
 * @li Creation de l'objet CapteurTemperature
 */

Batterie BatterieVelo                 ;
CapteurVitesse CapteurVitesse         ;
CapteurTemperature CapteurTemperature ;

/**
 * @brief Definition des couleurs d'affichage de l'écran LCD
 */

const int colorR = 255 ;                                               // Intensité de la couleur Rouge de l'écran LCD
const int colorG = 255 ;                                               // Intensité de la couleur Vert de l'écran LCD
const int colorB = 255 ;                                               // Intensité de la couleur Bleu de l'écran LCD

/**
 * @brief Définition des entrées / sorties de la carte Arduino, démarrage des librairies utilisées
 */

void setup()
{
  Serial.begin(9600)                      ;                            // Paramètre de la vitesse de transmission USB
  Wire.begin()                            ;   
  RTC.begin()                             ;                            // Démarrage de la librairie RTClib.h                          
  Serial.println("\nInitialisation de la carte SD...") ;               
  Compteur.begin()                        ;
  pinMode (0, INPUT)                      ;                            // Déclaration de la broche où est câblé la batterie en Entrée
  pinMode (4, INPUT)                      ;                            // Déclaration de la broche où est câblé le bouton de changement d'affichage LCD
  pinMode (2, INPUT)                      ;                            // Déclaration de la broche où est câblé le capteur de courant de la batterie principale
  Bluetooth.begin(9600)                   ;                            // Vitesse de transmission du bluetooth
  MonEcran.begin(16, 2)                   ;                            // Déclaration de l'affichage de l'écran LCD
  MonEcran.setRGB(colorR, colorG, colorB) ;                            // Couleur d'affichage de l'écran LCD
  pinMode(53, OUTPUT)                     ;
  SD.begin(10,11,12,13)                   ;
 
  
  if (! RTC.isrunning())                                                    // Si RTC ne fonctionne pas
  {
    Serial.println("RTC ne fonctionne pas !") ;
    RTC.adjust(DateTime(__DATE__, __TIME__))  ;                             // Met à l'heure à date à laquelle le sketch est compilé
  }

  Test = SD.remove("Rapport.CSV") ;                                         // Si le fichier existe alors il est supprimé

  Rapport = SD.open("Rapport.CSV", FILE_WRITE) ;                            // Création du fichier Rapport.CSV
  Rapport.println("Tension;Intensite;Puissance;Vitesse;Distance;Charge;Puissance Consommee (Wh);Puissance Consommee par Km (Wh/km);Capacite (Ah)") ;
  Rapport.close();
}

/**
 * @fn void loop (void)
 * @brief Entrée du programme
 */

void loop()
{
  //Affichage de l'heure
  DateTime now = RTC.now();
  
  Bouton = digitalRead(4) ;                                            // Lecture de l'état du bouton de blocage de l'écran
  
  if (Bouton == 1 && Bouton != ValeurPrecedente)                       // Condition de changement d'état
  {
    BoutonChoixEcran = 1 ;
  }
  
  ValeurPrecedente = Bouton ;                                          // Enregistrement de l'état actuel du bouton

  Tension = BatterieVelo.Get_Tension()                           ;     // Enregitrement du retour de la méthode Get_Tension dans la variable Tension
  Intensite = BatterieVelo.Get_Intensite()                       ;     // Enregitrement du retour de la méthode Get_Intensite dans la variable Intensite
  Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;     // Enregitrement du retour de la méthode CalculerPuissance dans la variable Puissance
  Vitesse = CapteurVitesse.Get_Vitesse()                         ;     // Enregitrement du retour de la méthode Get_Vitesse dans la variable Vitesse
  Charge = BatterieVelo.ChargeBatterie(Tension)                  ;     // Enregistrement du retour de la méthode ChargeBatterie dans la variable Charge

  Capacite = Capacite + Intensite * (LOG_INTERVAL / 1000) / 3600 ;
  PuissanceConsommee = Capacite * Tension                        ;
  PuissanceConsommeeKM = PuissanceConsommee / Distance           ;
  
  ChangementEtat = digitalRead(2) ;                                    // Lecture de l'état du capteur de vitesse

  if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
  {
    Distance = Perimetre + Distance ;                                  // Calcul de la distance parcourue
  }
  
  ValeurPrecedenteDist = ChangementEtat ;                              // Enregistrement de l'état actuel du capteur

  unsigned long TempsContinu = millis() ;
  
  Intervalle = TempsContinu - Temps ;

  if (Intervalle >= 1000)
  {
    EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance, Charge) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

    Rapport = SD.open("Rapport.CSV", FILE_WRITE)            ;
    Rapport.print(Tension), Rapport.print(';')              ;
    Rapport.print(Intensite), Rapport.print(';')            ;
    Rapport.print(Puissance), Rapport.print(';')            ;
    Rapport.print(Vitesse), Rapport.print(';')              ;
    Rapport.print(Distance/1000), Rapport.print(';')        ;
    Rapport.print(Charge), Rapport.print(';')               ;
    Rapport.print(PuissanceConsommee), Rapport.print(';')   ;
    Rapport.print(PuissanceConsommeeKM), Rapport.print(';') ;
    Rapport.print(Capacite), Rapport.print(';')             ;
    Rapport.println()                                       ;
    Rapport.close()                                         ;
    
    Temps = TempsContinu ;
  }
  

  if (CompteurBoucle < 200)
  {
    AfficherInfo(Tension, Intensite, Distance, Vitesse) ;                 // Appel de la méthode d'affichage de la Tension, de l'intensité, de la distance et de la vitesse

    while (BoutonChoixEcran == 1)                                         // Si le bouton de blocage à été appuyer alors l'affichage reste sur les informations en cours
    {
      Bouton = digitalRead(4) ;                                           // Lecture de l'état du bouton de blocage de l'écran
  
      if (Bouton == 1 && Bouton != ValeurPrecedente)                      // Condition de changement d'état
      {
        BoutonChoixEcran = 0  ;
        CompteurBoucle = 200  ;
      }
  
      ValeurPrecedente = Bouton ;  
  
      Tension = BatterieVelo.Get_Tension()                           ;     // Enregitrement du retour de la méthode Get_Tension dans la variable Tension
      Intensite = BatterieVelo.Get_Intensite()                       ;     // Enregitrement du retour de la méthode Get_Intensite dans la variable Intensite
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;     // Enregitrement du retour de la méthode CalculerPuissance dans la variable Puissance
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;     // Enregitrement du retour de la méthode Get_Vitesse dans la variable Vitesse
      Charge = BatterieVelo.ChargeBatterie(Tension)                  ;     // Enregistrement du retour de la méthode ChargeBatterie dans la variable Charge

      Capacite = Capacite + Intensite * (LOG_INTERVAL / 1000) / 3600 ;
      PuissanceConsommee = Capacite * Tension                        ;
      PuissanceConsommeeKM = PuissanceConsommee / Distance           ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     
      }
      ValeurPrecedenteDist = ChangementEtat ;

      unsigned long TempsContinu = millis() ;
      Intervalle = TempsContinu - Temps     ;

      if (Intervalle >= 1000)
      {
        EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance, Charge) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

        Rapport = SD.open("Rapport.CSV", FILE_WRITE)            ;
        Rapport.print(Tension), Rapport.print(';')              ;
        Rapport.print(Intensite), Rapport.print(';')            ;
        Rapport.print(Puissance), Rapport.print(';')            ;
        Rapport.print(Vitesse), Rapport.print(';')              ;
        Rapport.print(Distance/1000), Rapport.print(';')        ;
        Rapport.print(Charge), Rapport.print(';')               ;
        Rapport.print(PuissanceConsommee), Rapport.print(';')   ;
        Rapport.print(PuissanceConsommeeKM), Rapport.print(';') ;
        Rapport.print(Capacite), Rapport.print(';')             ;
        Rapport.println()                                       ;
        Rapport.close()                                         ;
        
        Temps = TempsContinu ;
      }

      AfficherInfo(Tension, Intensite, Distance, Vitesse) ;
    }
  }

  if (CompteurBoucle == 200)
  {
    MonEcran.clear() ;
  }


  if (CompteurBoucle > 200 && CompteurBoucle < 400)
  {
    AfficherInfo2(Puissance, Capacite, PuissanceConsommee) ;                                       // Appel de la méthode d'affichage de la puisance et de la charge de la batterie

    while (BoutonChoixEcran == 1)                                            // Si le bouton de blocage à été appuyer alors l'affichage reste sur les informations en cours
    {
      Bouton = digitalRead(4) ;                                              // Lecture de l'état du bouton de blocage de l'écran
  
      if (Bouton == 1 && Bouton != ValeurPrecedente)                         // Condition de changement d'état
      {
        BoutonChoixEcran = 0  ;
        CompteurBoucle = 400  ;
      }
  
      ValeurPrecedente = Bouton ; 
      
      Tension = BatterieVelo.Get_Tension()                           ;     // Enregitrement du retour de la méthode Get_Tension dans la variable Tension
      Intensite = BatterieVelo.Get_Intensite()                       ;     // Enregitrement du retour de la méthode Get_Intensite dans la variable Intensite
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;     // Enregitrement du retour de la méthode CalculerPuissance dans la variable Puissance
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;     // Enregitrement du retour de la méthode Get_Vitesse dans la variable Vitesse
      Charge = BatterieVelo.ChargeBatterie(Tension)                  ;     // Enregistrement du retour de la méthode ChargeBatterie dans la variable Charge

      Capacite = Capacite + Intensite * (LOG_INTERVAL / 1000) / 3600 ;
      PuissanceConsommee = Capacite * Tension                        ;
      PuissanceConsommeeKM = PuissanceConsommee / Distance           ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     
      }
      ValeurPrecedenteDist = ChangementEtat ;

      unsigned long TempsContinu = millis() ;
      Intervalle = TempsContinu - Temps     ;

      if (Intervalle >= 1000)
      {
        EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance, Charge) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

        Rapport = SD.open("Rapport.CSV", FILE_WRITE)            ;
        Rapport.print(Tension), Rapport.print(';')              ;
        Rapport.print(Intensite), Rapport.print(';')            ;
        Rapport.print(Puissance), Rapport.print(';')            ;
        Rapport.print(Vitesse), Rapport.print(';')              ;
        Rapport.print(Distance/1000), Rapport.print(';')        ;
        Rapport.print(Charge), Rapport.print(';')               ;
        Rapport.print(PuissanceConsommee), Rapport.print(';')   ;
        Rapport.print(PuissanceConsommeeKM), Rapport.print(';') ;
        Rapport.print(Capacite), Rapport.print(';')             ;
        Rapport.println()                                       ;
        Rapport.close()                                         ;
        
        Temps = TempsContinu ;
      }

      AfficherInfo2(Puissance, Capacite, PuissanceConsommee) ; 
    }
  }

  
  if (CompteurBoucle > 400)
  {
    CompteurBoucle = 0 ;                                                  // Le CompteurBoucle est remis à zéro une fois que toutes les données ont été affichées
    MonEcran.clear()   ;
  }
  CompteurBoucle++ ;                                                      // Incrémentation du CompteurBoucle
}
