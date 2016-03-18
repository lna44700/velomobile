#include <SoftwareSerial.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Ticks.h>

SoftwareSerial Bluetooth(10, 11) ;                                     // RX | TX
rgb_lcd MonEcran                 ;

float Batterie = 0            ;                                        // potentiometre simulant la tension de la batterie
float BatterieAux = 2         ;                                        // potentiometre simulant la tension de la batterie auxiliaire
float Tension (0)             ;                                        // tension de la batterie 
float Tension2 (0)            ;                                        // tension de la batterie auxiliaire
float Intensite  (0)          ;                                        // Intensite
float Vitesse (0)             ;                                        // Vitesse du vélo
float Distance (0.0)          ;                                        // Distance parcourue par le vélo
float Distance1 (0.0)         ;                                        // Variable tampon de la distance                                       
float Temps  (0.0)            ;                                        // Variable de Temps
float Perimetre (0.0)         ;                                        // Périmètre de la roue du vélo
float RayonRoue (0.254)       ;                                        // Rayon de la roue du vélo 
int Tac (0)                   ;                                        // Passage de l'aimant du compteur de vitesse
const byte ValeurCapteur = 2  ;                                        // Le capteur du compteur de vitesse est câblé à la broche numéro 2
const byte numInterrupt = 0   ;                                        // Numéro de la broche d'interruption
const int periode = 1000      ;                                        // Période en milliseconde, permet d'avoir la fréquence instantanée du passage de l'aimant
int ChoixEcran = 4            ;                                        // Bouton permettant d'afficher sur l'écran LCD les tensions, l'intensité, la puissance et la distance parcourue
float Courant = 4             ;                                        // potentiometre simulant l'intensité délivrée par la batterie principale
float Puissance (0.0)         ;                                        // Puissance délivrée par la batterie principale

Ticks Capteur (numInterrupt, ValeurCapteur, periode) ;                 // Appel du constructeur de la librairie Ticks permettant d'avoir accès aux fonctions associées

const int colorR = 255 ;                                               // Intensité de la couleur Rouge de l'écran LCD
const int colorG = 255 ;                                               // Intensité de la couleur Vert de l'écran LCD
const int colorB = 255 ;                                               // Intensité de la couleur Bleu de l'écran LCD

void AfficherEcranTension(float Tension)                               // Fonction d'affichage de la tension sur l'écran LCD
{
  MonEcran.clear() ;
  MonEcran.print("    Tension : ")   ;
  MonEcran.setCursor(2,16)           ;
  MonEcran.print("   ")              ;
  MonEcran.print(Tension)            ;
  MonEcran.print(" V")               ;
  delay (3000)                       ;
  
  MonEcran.clear() ;
}

void AfficherEcranTensionAuxiliaire(float Tension2)                    // Fonction d'affichage de la tension batterie auxiliaire sur l'écran LCD
{
  MonEcran.print("  Tension aux : ") ;
  MonEcran.setCursor(2,16)           ;
  MonEcran.print("   ")              ;
  MonEcran.print(Tension2)           ;
  MonEcran.print(" V")               ;
  delay (3000)                       ;
  
  MonEcran.clear() ;
}

void AfficherEcranIntensite(float Intensite)                           // Fonction d'affichage de l'intensité sur l'écran LCD
{
  MonEcran.print("   Intensite : ")  ;
  MonEcran.setCursor(2,16)           ;
  MonEcran.print("   ")              ;
  MonEcran.print(Intensite)          ;
  MonEcran.print(" A")               ;
  delay (3000)                       ;
  
  MonEcran.clear() ;
}

void AfficherEcranPuissance(float Puissance)                           // Fonction d'affichage de la puisance délivrée sur l'écran LCD
{
  MonEcran.print("   Puissance : ")  ;
  MonEcran.setCursor(2,16)           ;
  MonEcran.print("  ")               ;
  MonEcran.print(Puissance)          ;
  MonEcran.print(" W")               ;
  delay (3000)                       ;
  
  MonEcran.clear() ;
}

void AfficherEcranDistance(float Distance)                             // Fonction d'affichage de la distance sur l'écran LCD
{
  MonEcran.print("   Distance : ") ;
  MonEcran.setCursor(2,16)         ;
  MonEcran.print(" ")              ;
  MonEcran.print(Distance)         ;
  MonEcran.print(" km")            ;
  delay (3000)                     ;

  MonEcran.clear() ;
}


void EnvoyerBluetooth(float Tension, float Tension2, float Intensite, float Puissance, float Vitesse, float Distance)            // Fonction d'affichage de la tension, de l'intensité, de la vitesse, de la distance sur smartphone
{
  Bluetooth.print("La tension de la batterie est de " ) ;                                                       // Affichage de la tension sur le téléphone portable
  Bluetooth.print(Tension)                              ;
  Bluetooth.println(" V")                               ;

  Bluetooth.print("L'intensite de la batterie est de ") ;                                                       // Affichage de l'intensité sur le téléphone portable
  Bluetooth.print(Intensite)                            ;
  Bluetooth.println(" A")                               ;  

  Bluetooth.print("La puissance délivrée est de ")      ;                                                       // Affichage de la puissance délivrée sur le téléphone portable
  Bluetooth.print(Puissance)                            ;
  Bluetooth.println(" W")                               ;  

  Bluetooth.print("La tension de la batterie auxiliaire est de " ) ;                                            // Affichage de la tension sur le téléphone portable
  Bluetooth.print(Tension2)                                        ;
  Bluetooth.println(" V")                                          ;

  Bluetooth.print("La vitesse est de ") ;                                                                       // Affichage de la vitesse sur le téléphone portable
  Bluetooth.print(Vitesse)              ;
  Bluetooth.println(" km/h")            ;

  Bluetooth.print("Distance parcourue : ") ;                                                                    // Affichage de la distance parcourue sur le téléphone portable                              
  Bluetooth.print(Distance)                ;
  Bluetooth.println(" km")                 ;
}

void setup()
{
  Serial.begin(9600)                      ;                               // Paramètre de la vitesse de transmission USB
  pinMode (Batterie, INPUT)               ;                               // Déclaration de la broche où est câblé la batterie en Entrée
  Bluetooth.begin(9600)                   ;                               // Vitesse de transmission du bluetooth
  MonEcran.begin(16, 2)                   ;                               // Déclaration de l'affichage de l'écran LCD
  MonEcran.setRGB(colorR, colorG, colorB) ;                               // Couleur d'affichage de l'écran LCD
  Capteur.begin()                         ;                               // Capteur de vitesse
  pinMode (BatterieAux, INPUT)            ;                               // Déclaration de la broche où est câblé la batterie auxiliaire en entrée
  pinMode (ChoixEcran, INPUT)             ;                               // Déclaration de la broche où est câblé le bouton de changement d'affichage LCD
  pinMode (Courant, INPUT)                ;                               // Déclaration de la broche où est câblé le capteur de courant de la batterie principale
}

void loop() 
{
  Temps = millis() ;                                                      // Variable qui utilise la fonction millis()                                                     
  
  Tension = analogRead (Batterie) ;                                       // Lire la Tension délivré par la batterie
  Tension = (Tension * 40)/1023   ;                                       // Calcul permettant d'afficher la tension en volt

  Tension2 = analogRead (BatterieAux) ;                                   // Lire la Tension délivré par la batterie auxiliaire
  Tension2 = (Tension2 * 12)/1023     ;                                   // Calcul permettant d'afficher la tension en volt

  Intensite = analogRead (Courant)  ;                                     // Lire l'intensité délivré par la batterie principal
  Intensite = (Intensite * 25)/1023 ;                                     // Calcul permettant d'afficher l'intensité en ampère

  Puissance = Tension * Intensite ;                                       // Calcul de la puissance délivrée par la batterie principale

  Temps = Temps / 1000 / 3600          ;

  Distance = Vitesse * Temps           ;                                  // Calcul de la distance parcourue
  Distance1 = Distance1 + Distance     ;                                  // Enregistrement de la distance actuelle sur une variable Tampon car Distance retourne à 0 si la vitesse est nul
  

  Capteur.operate()                     ;                                 // Fonction qui met à jour la fréquence instantanée
  Tac = Capteur.TickRate1Period()       ;                                 // Nombre de passage de l'aimant par seconde
  Perimetre = 2 * 3.14 * RayonRoue      ;                                 // Périmètre de la roue
  Vitesse = Tac*Perimetre*3.6           ;                                 // Calcul de la vitesse du vélo
  
  MonEcran.print("   Vitesse : ") ;                                       /////////////////////////////////////////////////
  MonEcran.setCursor(2,16)        ;                                       /////////////////////////////////////////////////
  MonEcran.print("  ")            ;                                       /////////////Affichage de la vitesse/////////////
  MonEcran.print(Vitesse)         ;                                       //////////sur l'ecran LCD en temps réel//////////
  MonEcran.print(" km/h")         ;                                       /////////////////////////////////////////////////

  EnvoyerBluetooth (Tension, Tension2, Intensite, Puissance, Vitesse, Distance1) ;     // Appel de la fonction permettant d'envoyer les données via bluetooth

  ChoixEcran = digitalRead(4)     ;                                                    // Lecture de l'état du bouton de changement d'affichage

  if (ChoixEcran == 1)
  {
    AfficherEcranTension(Tension)               ;                                     ///////////////////////////////////////////
    AfficherEcranIntensite(Intensite)           ;                                     ///////////////////////////////////////////
    AfficherEcranPuissance(Puissance)           ;                                     //////Appel des fonctions d'affichage////// 
    AfficherEcranTensionAuxiliaire(Tension2)    ;                                     /////////////sur l'écran LCD///////////////
    AfficherEcranDistance(Distance1)            ;                                     // ////////////////////////////////////////
  }
  
  Temps = 0.0 ;
}

