/**
 * @file Commodo_Arduino.c
 * @brief Programme de tests.
 * @author Valentin.D
 * @version 0.25
 * @date 02 juin 2016
 *
 * Programme de commande des éléments de la signalisation routière du vélomobile. (Phares, clignotants, warnings, bandeau DEL et recupération tension/intensité de la abtterie auxiliaire)
 *
 */



// --------------------------------------------------------------------------------------------------------------- \\
//                                             ENTETE DECLARATIVE                                                  \\
// --------------------------------------------------------------------------------------------------------------- \\

#include <Wire.h>
#include "rgb_lcd.h"                                                  // Inclusion de la librairie de l'éran LCD
rgb_lcd lcd                 ;
int iBoutonPhares = 3       ;                                         // Déclaration du bouton des phares sur la broche 3
int iPhares = 3             ;                                         // Déclaration des phares sur la broche 3
int iBoutonClignotantG = 13 ;                                         // Déclaration du bouton du clignotant gauche sur la broche 13
int iBoutonClignotantD = 12 ;                                         // Déclaration du bouton du clignotant droit sur la broche 12
int iClignotantGauche = 6   ;                                         // Déclaration du clignotant gauche sur la broche 6
int iClignotantDroit = 7    ;                                         // Déclaration du clignotant droit sur la broche 7
int iBoutonWarnings = 8     ;                                         // Déclaration du bouton des warnings sur la broche 8
bool bEtatDel = true        ;                                         // Définition de l'état de la DEL à l'état haut
unsigned long lTimer = 0    ;                                         // Initialisation d'un chronomètre 1 pour le calcul du temps
const int colorR = 50       ;                                         // Quantité de ROUGE sur la couleur de l'écran LCD
const int colorG = 255      ;                                         // Quantité de VERT sur la couleur de l'écran LCD
const int colorB = 255      ;                                         // Quantité de BLEU sur la couleur de l'écran LCD
int bandeauLed = 10         ;                                         // Déclaration du bandeau de DEL sur la broche 10
int intensite = 1           ;                                         // Déclaration du potentiomètre qui gère l'intensité du bandeau de DEL sur la broche 1
int potentiometre = 0       ;
int intensiteHex = 0        ;


// --------------------------------------------------------------------------------------------------------------- \\
//                                  FONCTION SETUP = Code d'initialisation                                         \\
// --------------------------------------------------------------------------------------------------------------- \\

/**
 * @brief Définition des entrées / sorties de la carte Arduino, démarrage des librairies utilisées
 */
void setup()
{
  Serial.begin(38400)                ;                                // Démarrage d'une connexion avec 38 400 bits/seconde
  pinMode(iBoutonPhares, INPUT)      ;                                // Bouton des phares en ENTREE
  pinMode(iPhares, OUTPUT)           ;                                // Phares en SORTIE
  pinMode(iClignotantGauche, OUTPUT) ;                                // Clignotant gauche en SORTIE
  pinMode(iClignotantDroit, OUTPUT)  ;                                // Clignotant droit en SORTIE  
  lcd.begin(16, 2)                   ;                                // Nombre de colonnes et de lignes de l'écran LCD
  lcd.setRGB(colorR, colorG, colorB) ;                                // Définition des couleurs de l'écran
  pinMode(bandeauLed, OUTPUT)        ;                                // Déclaration du bandeau de DEL en sortie
  pinMode(iBoutonClignotantG, INPUT) ;                                // Déclaration du bouton clignotant gauche en entrée
  pinMode(iBoutonClignotantD, INPUT) ;                                // Déclaration du bouton clignotant droit en entrée
}



// --------------------------------------------------------------------------------------------------------------- \\
//                                                  FONCTIONS                                                      \\
// --------------------------------------------------------------------------------------------------------------- \\

/**
 * @brief Fonction qui permet d'allumer le clignotant gauche
 * @returnbEtatDel
 */
bool allumerClignotantGauche()                                            // Fonction qui sert à allumer clignotant gauche
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un chronomètre 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux chronomètres
  digitalWrite(iClignotantDroit, false)                 ;                 // On éteint le clignotant droit
 
  if (bEtatDel == true && lIntervalle > 500)                              // Si la variable bEtatDel est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {                                                              
    bEtatDel = false                                    ;                 // La variable bEtatDel passe à l'état bas
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On éteint le clignotant gauche
    lTimer = lTimerCourant                              ;                 // Le chronomètre 1 prend la valeur du chronomètre 2
  }
  else if (bEtatDel == false && lIntervalle > 500)                        // Sinon si la variable bEtatDel est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = true                                     ;                 // La variable bEtatDel passe à l'état haut
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On allume le clignotant gauche
    lTimer = lTimerCourant                              ;                 // Le chronomètre 1 prend la valeur du chronomètre 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable bEtatDel)
}


/**
 * @brief Fonction qui permet d'allumer le clignotant droit
 * @returnbEtatDel
 */
bool allumerClignotantDroit()                                             // Fonction qui sert à allumer clignotant droit
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un chronomètre 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux chronomètres
  digitalWrite(iClignotantGauche, false)                ;                 // On éteint le clignotant gauche
 
  if (bEtatDel == true && lIntervalle > 500)                              // Si la variable bEtatDel est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = false                                    ;                 // La variable bEtatDel passe à l'état bas
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On éteint le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le chronomètre 1 prend la valeur du chronomètre 2
  }
  else if (bEtatDel == false && lIntervalle > 500)                        // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = true                                     ;                 // La variable bEtatDel passe à l'état haut
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On allume le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le chronomètre 1 prend la valeur du chronomètre 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable bEtatDel)
}


/**
 * @brief Fonction qui permet d'allumer les warnings
 * @returnbEtatDel
 */
bool allumerWarnings()                                                    // Fonction qui sert à allumer les warnings
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un chronomètre 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers

  if (bEtatDel == true && lIntervalle > 500)                              // Si la variable bEtatDel est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = false                                    ;                 // La variable bEtatDel passe à l'état bas
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On éteint le clignotant gauche
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On éteint le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le chronomètre 1 prend la valeur du chronomètre 2
  }
  else if (bEtatDel == false && lIntervalle > 500)                        // Sinon si la variable bEtatDel est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = true                                     ;                 // La variable bEtatDel passe à l'état haut
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On allume le clignotant gauche
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On allume le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le chronomètre 1 prend la valeur du chronomètre 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable bEtatDel)
}


/**
 * @brief Méthode qui affiche de la publicité sur écran LCD
 */
void afficherPublicite()                                                  // Fonction qui gère l'affichage de la publicité sur l'écran LCD
{
  lcd.setCursor(0, 0)                                   ;                 // On se place sur la ligne 0 (1ère ligne) et la colonne 0
  lcd.print("Velomobile Evoz")                          ;                 // Affichage du message "Velomobile Evoz"
  lcd.setCursor(2, 1)                                   ;                 // On se place sur le ligne 1 (2ème ligne) et la colonne 2
  lcd.print("Essayez-le !")                             ;                 // Affichage du message "Essayez-le !"
}



// --------------------------------------------------------------------------------------------------------------- \\
//                             FONCTION LOOP = Boucle sans fin = coeur du programme                                \\
// --------------------------------------------------------------------------------------------------------------- \\

/**
 * @fn void loop (void)
 * @brief Programme principal
 */
void loop()
{
  int iEtatBoutonPhares = digitalRead(iBoutonPhares)            ;         // Lire l'état du bouton des phares et le stocker dans "iEtatBoutonPhares"
  int iEtatBoutonClignotantG = digitalRead(iBoutonClignotantG)  ;         // Lire l'état du bouton du clignotant gauche et le stocker dans "iEtatBoutonClignotantG"
  int iEtatBoutonClignotantD = digitalRead(iBoutonClignotantD)  ;         // Lire l'état du bouton du clignotant droit et le stocker dans "iEtatBoutonClignotantD"
  int iEtatBoutonWarnings = digitalRead(iBoutonWarnings)        ;         // Lire l'état du bouton des warnings et le stocker dans "iEtatBoutonWarnings"

  
  // PUBLICITE
  afficherPublicite()                                           ;         // Affichage de la publicité sur l'écran LCD


  // GESTION DU BANDEAU DE DEL
  potentiometre = analogRead(intensite)                         ;         // Récupère la valeur du potentiomètre qui gère l'intensité du bandeau de DEL (valeur de 0 a 1023)
  intensiteHex = potentiometre / 4                              ;         // Divise la valeur relevée du potentiomètre par 4 pour récupérer une valeur entre 0 et 255
  unsigned long lTimerCourant = millis()                        ;         // Initialisation d'un chronomètre 2
  unsigned long lIntervalle = lTimerCourant - lTimer            ;         // Temps écoulé entre les deux chronomètres
    
  if (intensiteHex <= 32)                                                 // Si l'intensité du potentiomètre se situe entre 0 et 32
  {
    if (bEtatDel == true && lIntervalle < 200)                            // Si le bandeau de DEL est allumé depuis plus de 200 ms
    {
      bEtatDel == false                                         ;         // La variable bEtatDel passe à l'état bas
      analogWrite(bandeauLed,0)                                 ;         // Le bandeau de DEL s'éteint
      lTimer = lTimerCourant                                    ;         // Le chronomètre 1 prend la valeur du chronomètre 2
    }
    else if (bEtatDel == false && lIntervalle > 1000)                     // Sinon si le bandeau de DEL est éteint depuis plus de 1000 ms
    {
      bEtatDel == true                                          ;         // La variable bEtatDel passe à l'état haut
      analogWrite(bandeauLed,255)                               ;         // Le bandeau de DEL s'allume
      lTimer = lTimerCourant                                    ;         // Le chronomètre 1 prend la valeur du chronomètre 2
    }
  }
  else                                                                    // Sinon si l'intensité du potentimètre se situe entre 33 et 255
  {
    analogWrite(bandeauLed, intensiteHex)                       ;         // Le bandeau de DEL reste allumé avec l'intensité du potentiomètre
  }



  // CLIGNOTANTS
  if ((digitalRead(iBoutonClignotantG) == true) && (iEtatBoutonWarnings == false))           // Si le bouton du clignotant gauche est enfoncé et celui des warnings est éteint
  { 
    Serial.println("Clignotant Gauche")                                                 ;    // Affichage du message "Clignotant Gauche" sur la console                                       
    allumerClignotantGauche()                                                           ;    // Exécution de la fonction qui allume le clignotant gauche
    digitalWrite(iClignotantDroit,0)                                                    ;    // Le clignotant droit s'éteint
  }    
  else if ((digitalRead(iBoutonClignotantD) == true) && (iEtatBoutonWarnings == false))      // Sinon si le bouton du clignotant droit est enfoncé et celui des warnings est éteint
  {
    Serial.println("Clignotant Droit")                                                  ;    // Affichage du message "Clignotant Droit" sur la console
    allumerClignotantDroit()                                                            ;    // Le clignotant droit s'allume
    digitalWrite(iClignotantGauche,0)                                                   ;    // Le clignotant gauche s'éteint
  }

 
  // WARNINGS
  if ((iEtatBoutonWarnings == true) ||                                                       // Si le bouton des warnings est appuyé OU
  ((iEtatBoutonWarnings == true) && (iEtatBoutonClignotantG == true)) ||                     // le bouton des warnings est appuyé ET le bouton du clignotant gauche est appuyé OU
  ((iEtatBoutonWarnings == true) && (iEtatBoutonClignotantD == true)))                       // le bouton des warnings est appuyé ET le bouton du clignotant droit est appuyé
  {
    Serial.println("Warnings")                                                          ;    // Affichage du message "Warnings" sur la console
    allumerWarnings()                                                                   ;    // Exécution de la fonction qui allume les warnings
  }

  
  if ((iEtatBoutonWarnings == false) && (iEtatBoutonClignotantG == false) && (iEtatBoutonClignotantD == false))    // Si le bouton des warnings n'est pas appuyé ET les boutons des clignotants ne sont pas appuyés
  {
    Serial.println("Aucun bouton n'est appuyé")                                         ;    // Affichage du message "Aucun bouton n'est appuyé" sur la console
    digitalWrite(iClignotantGauche,0)                                                   ;    // Extinction du clignotant gauche
    digitalWrite(iClignotantDroit,0)                                                    ;    // Extinction du clignotant droit
  }
}



// --------------------------------------------------------------------------------------------------------------- \\
//                                                   FIN DU PROGRAMME                                              \\
// --------------------------------------------------------------------------------------------------------------- \\
