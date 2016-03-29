#include <SoftwareSerial.h>                                            // Librairie permetteant de mettre en oeuvre le module Bluetooth
#include <Wire.h>                                                      // Permet d'utiliser la communication I2C
#include "rgb_lcd.h"                                                   // Librairie de l'écran LCD
#include <Ticks.h>                                                     // Compte des impulsions sur un certains laps de temps et en déduis la fréquence

SoftwareSerial Bluetooth(10, 11) ;                                     // RX | TX
rgb_lcd MonEcran                 ;

const byte ValeurCapteur = 2     ;                                    // Le capteur du compteur de vitesse est câblé à la broche numéro 2
const byte numInterrupt = 0      ;                                    // Numéro de la broche d'interruption
const int Periode = 1000         ;                                    // Période en milliseconde, permet d'avoir la fréquence instantanée du passage de l'aimant

float Tension (0.0)              ;                                    // tension de la batterie
float Intensite  (0.0)           ;                                    // Intensite
float Puissance (0.0)            ;                                    // Puissance délivrée par la batterie principale
float Vitesse (0.0)              ;                                    // Vitesse du vélo
float Distance (0.0)             ;                                    // Distance parcourue par le vélo
float RayonRoue (0.254)          ;                                    // Rayon de la roue du vélo
char Aimant (0)                  ;                                    // Passage de l'aimant du compteur de vitesse
int  BoutonChoixEcran (0)        ;                                    // Bouton permettant d'afficher sur l'écran LCD les tensions, l'intensité, la puissance et la distance parcourue
int CompteurBoucle (0)           ;                                    // Variable qui compte le nombre de fois que la boucle loop() à été faite
char ValeurPrecedente(0)         ;                                    // Valeur précédente du bouton permettant de bloquer l'écran
char Bouton (0)                  ;                                    // Variable qui va prendre la valeur de la lecture du bouton de blocage de l'écran
char ChangementEtat (0)          ;                                    // Variable qui enregistre l'état précédant le passage de l'aimant devant le capteur
char ValeurPrecedenteDist(0)     ;                                    // Variable qui va prendre la valeur de la variable ChangementEtat

Ticks  Compteur (numInterrupt, ValeurCapteur, Periode) ;              // Appel du constructeur de la librairie Ticks permettant d'avoir accès aux fonctions associées

float Perimetre = 2 * 3.14159265359 * RayonRoue      ;                // Périmètre de la roue

class Batterie                                                        // Création de la classe Batterie
{
  private:                                                            // Attributs
    float Tension   ;
    float Intensite ;
    float Puissance ;

  public:                                                             // Méthodes
    Batterie()  ;                                                     // Constructeur par défaut
    ~Batterie() ;                                                     // Destructeur

    float Get_Tension ()   ;                                          // Méthode d'acquisition de la tension
    float Get_Intensite () ;                                          // Méthode d'acquisition de l'intensite

    float CalculerPuissance (float Tension, float Intensite) ;        // Méthode de calcul de la puissance
};

class Capteur                                                         // Création de la classe Capteur
{
  private:                                                            // Attributs
    float Vitesse     ;
    float Temperature ;

  public:                                                             // Méthodes
    Capteur()  ;
    ~Capteur() ;

    float Get_Vitesse ()     ;                                        // Méthode d'acquisition de la vitesse
    float Get_Temperature () ;                                        // Méthode d'acquisition de la température
};

Batterie::Batterie():
  Tension(0.0),
  Intensite(0.0),
  Puissance(0.0)
{

}

Batterie::~Batterie()
{
  this->Tension = 0.0   ;
  this->Intensite = 0.0 ;
  this->Puissance = 0.0 ;
}

Capteur::Capteur():
  Vitesse(0.0),
  Temperature(0.0)
{

}

Capteur::~Capteur()
{
  this->Vitesse = 0.0     ;
  this->Temperature = 0.0 ;
}

float Batterie::Get_Tension()
{
  Tension = analogRead (0)        ;                                    // Lire la Tension délivrée par la batterie
  Tension = (Tension * 40) / 1023 ;                                    // Calcul permettant d'afficher la tension en volt

  return Tension ;
}

float Batterie::Get_Intensite()
{
  Intensite = analogRead (2)          ;                                // Lire l'intensité délivrée par la batterie principal
  Intensite = (Intensite * 25) / 1023 ;                                // Calcul permettant d'afficher l'intensité en ampère

  return Intensite ;
}

float Batterie::CalculerPuissance(float Tension, float Intensite)
{
  Puissance = Tension * Intensite   ;                                  // Calcul de la puissance délivrée par la batterie principale

  return Puissance ;
}

float Capteur::Get_Vitesse()
{
  Compteur.operate()                  ;                                // Fonction qui met à jour la fréquence instantanée
  Aimant = Compteur.TickRate1Period() ;                                // Nombre de passage de l'aimant par seconde
  Vitesse = Aimant * Perimetre * 3.6  ;                                // Calcul de la vitesse du vélo

  return Vitesse ;
}

float Capteur::Get_Temperature()
{

}

void EnvoyerBluetooth(float Tension, float Intensite,
                      float Puissance, float Vitesse, float Distance)            // Fonction d'affichage de la tension, de l'intensité, de la vitesse, de la distance sur smartphone
{
  Bluetooth.print("La tension de la batterie est de " ) ;                        // Affichage de la tension sur le téléphone portable
  Bluetooth.print(Tension)                              ;
  Bluetooth.println(" V")                               ;

  Bluetooth.print("L'intensite de la batterie est de ") ;                        // Affichage de l'intensité sur le téléphone portable
  Bluetooth.print(Intensite)                            ;
  Bluetooth.println(" A")                               ;

  Bluetooth.print("La puissance délivrée est de ")      ;                        // Affichage de la puissance délivrée sur le téléphone portable
  Bluetooth.print(Puissance)                            ;
  Bluetooth.println(" W")                               ;

  Bluetooth.print("La vitesse est de ") ;                                        // Affichage de la vitesse sur le téléphone portable
  Bluetooth.print(Vitesse)              ;
  Bluetooth.println(" km/h")            ;

  Bluetooth.print("Distance parcourue : ") ;                                     // Affichage de la distance parcourue sur le téléphone portable
  Bluetooth.print(Distance / 1000)         ;
  Bluetooth.println(" km")                 ;
}

void AfficherVitesse(float Vitesse)                               // Fonction d'affichage de la vitesse sur l'écran LCD
{
  MonEcran.print("    Vitesse : ")   ;
  MonEcran.setCursor(2, 16)          ;
  MonEcran.print("   ")              ;
  MonEcran.print(Vitesse)            ;
  MonEcran.print(" km/h")            ;
}

void AfficherTension(float Tension)                               // Fonction d'affichage de la tension sur l'écran LCD
{
  MonEcran.print("    Tension : ")   ;
  MonEcran.setCursor(2, 16)          ;
  MonEcran.print("   ")              ;
  MonEcran.print(Tension)            ;
  MonEcran.print("  V")              ;
}

void AfficherIntensite(float Intensite)                           // Fonction d'affichage de l'intensité sur l'écran LCD
{
  MonEcran.print("   Intensite : ")  ;
  MonEcran.setCursor(2, 16)          ;
  MonEcran.print("    ")             ;
  MonEcran.print(Intensite)          ;
  MonEcran.print(" A")               ;
}

void AfficherPuissance(float Puissance)                           // Fonction d'affichage de la puisance délivrée sur l'écran LCD
{
  MonEcran.print("   Puissance : ")  ;
  MonEcran.setCursor(2, 16)          ;
  MonEcran.print("   ")              ;
  MonEcran.print(Puissance)          ;
  MonEcran.print(" W")               ;
}

void AfficherDistance(float Distance)                             // Fonction d'affichage de la distance sur l'écran LCD
{
  MonEcran.print("   Distance : ") ;
  MonEcran.setCursor(2, 16)        ;
  MonEcran.print("    ")           ;
  MonEcran.print(Distance)         ;
  MonEcran.print(" km")            ;
}


Batterie BatterieVelo      ;
Capteur CapteurVitesse     ;
Capteur CapteurTemperature ;

const int colorR = 255 ;                                               // Intensité de la couleur Rouge de l'écran LCD
const int colorG = 255 ;                                               // Intensité de la couleur Vert de l'écran LCD
const int colorB = 255 ;                                               // Intensité de la couleur Bleu de l'écran LCD


void setup()
{
  Serial.begin(9600)                      ;                            // Paramètre de la vitesse de transmission USB
  Compteur.begin()                        ;
  pinMode (0, INPUT)                      ;                            // Déclaration de la broche où est câblé la batterie en Entrée
  pinMode (4, INPUT)                      ;                            // Déclaration de la broche où est câblé le bouton de changement d'affichage LCD
  pinMode (2, INPUT)                      ;                            // Déclaration de la broche où est câblé le capteur de courant de la batterie principale
  Bluetooth.begin(9600)                   ;                            // Vitesse de transmission du bluetooth
  MonEcran.begin(16, 2)                   ;                            // Déclaration de l'affichage de l'écran LCD
  MonEcran.setRGB(colorR, colorG, colorB) ;                            // Couleur d'affichage de l'écran LCD
}

void loop()
{
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

  ChangementEtat = digitalRead(2) ;                                    // Lecture de l'état du capteur de vitesse

  if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
  {
    Distance = Perimetre + Distance ;                                  // Calcul de la distance parcourue
  }
  
  ValeurPrecedenteDist = ChangementEtat ;                              // Enregistrement de l'état actuel du capteur

  EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

  if (CompteurBoucle < 15)
  {
    AfficherVitesse(Vitesse) ;                                            // Appel de la méthode d'affichage de la vitesse

    while (BoutonChoixEcran == 1)                                         // Si le bouton de blocage à été appuyer alors l'affichage reste sur la vitesse
    {
      if (digitalRead(4) == 1 && ValeurPrecedente == 1)                   // Si le bouton est de nouveau appuyé, on sort de la boucle pour afficher les autres données
      {
        BoutonChoixEcran = 0 ;
        CompteurBoucle = 15  ;
      }
      Tension = BatterieVelo.Get_Tension()                           ;
      Intensite = BatterieVelo.Get_Intensite()                       ;
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     
      }
      ValeurPrecedenteDist = ChangementEtat ;

      EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance) ;

      AfficherVitesse(Vitesse) ;
    }
  }

  if (CompteurBoucle == 15)
  {
    MonEcran.clear() ;
  }


  if (CompteurBoucle > 15 && CompteurBoucle < 30)
  {
    AfficherTension(Tension) ;                                               // Appel de la méthode d'affichage de la tension

    while (BoutonChoixEcran == 1)                                            // Si le bouton de blocage à été appuyer alors l'affichage reste sur la tension
    {
      if (digitalRead(4) == 1 && ValeurPrecedente == 1)                      // Si le bouton est de nouveau appuyé, on sort de la boucle pour afficher les autres données
      {
        BoutonChoixEcran = 0 ;
        CompteurBoucle = 30  ;
      }
      Tension = BatterieVelo.Get_Tension()                           ;
      Intensite = BatterieVelo.Get_Intensite()                       ;
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     
      }
      ValeurPrecedenteDist = ChangementEtat ;

      EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance) ;

      AfficherTension(Tension) ;
    }
  }

  if (CompteurBoucle == 30)
  {
    MonEcran.clear() ;
  }


  if (CompteurBoucle > 30 && CompteurBoucle < 45)
  {
    AfficherIntensite(Intensite) ;                                          // Appel de la méthode d'affichage de l'intensité

    while (BoutonChoixEcran == 1)                                           // Si le bouton de blocage à été appuyer alors l'affichage reste sur l'intensité
    {
      if (digitalRead(4) == 1 && ValeurPrecedente == 1)                     // Si le bouton est de nouveau appuyé, on sort de la boucle pour afficher les autres données
      {
        BoutonChoixEcran = 0 ;
        CompteurBoucle = 45  ;
      }
      Tension = BatterieVelo.Get_Tension()                           ;
      Intensite = BatterieVelo.Get_Intensite()                       ;
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     
      }
      ValeurPrecedenteDist = ChangementEtat ;

      EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance) ;

      AfficherIntensite(Intensite) ;
    }
  }


  if (CompteurBoucle == 45)
  {
    MonEcran.clear() ;
  }


  if (CompteurBoucle > 45 && CompteurBoucle < 60)
  {
    AfficherPuissance(Puissance) ;                                        // Appel de la méthode d'affichage de la puissance

    while (BoutonChoixEcran == 1)                                         // Si le bouton de blocage à été appuyer alors l'affichage reste sur la puissance
    {
      if (digitalRead(4) == 1 && ValeurPrecedente == 1)                   // Si le bouton est de nouveau appuyé, on sort de la boucle pour afficher les autres données
      {
        BoutonChoixEcran = 0 ;
        CompteurBoucle = 60  ;
      }
      Tension = BatterieVelo.Get_Tension()                           ;
      Intensite = BatterieVelo.Get_Intensite()                       ;
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                    
      }
      ValeurPrecedenteDist = ChangementEtat ;

      EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance) ;

      AfficherPuissance(Puissance) ;
    }
  }


  if (CompteurBoucle == 60)
  {
    MonEcran.clear() ;
  }

  if (CompteurBoucle > 60 && CompteurBoucle < 75)
  {
    AfficherDistance(Distance / 1000) ;                                  // Appel de la méthode d'affichage de la distance

    while (BoutonChoixEcran == 1)                                        // Si le bouton de blocage à été appuyer alors l'affichage reste sur la distance
    {
      if (digitalRead(4) == 1 && ValeurPrecedente == 1)                  // Si le bouton est de nouveau appuyé, on sort de la boucle pour afficher les autres données
      {
        BoutonChoixEcran = 0 ;
        CompteurBoucle = 75  ;
      }
      Tension = BatterieVelo.Get_Tension()                           ;
      Intensite = BatterieVelo.Get_Intensite()                       ;
      Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;
      Vitesse = CapteurVitesse.Get_Vitesse()                         ;

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     // Calcul de la distance parcourue
      }
      ValeurPrecedenteDist = ChangementEtat ;

      EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance) ;

      AfficherDistance(Distance / 1000) ;
    }
  }

  if (CompteurBoucle > 75)
  {
    CompteurBoucle = 0 ;                                                  // Le CompteurBoucle est remis à zéro une fois que toutes les données ont été affichées
    MonEcran.clear()   ;
  }
  CompteurBoucle++ ;                                                      // Incrémentation du CompteurBoucle
}
