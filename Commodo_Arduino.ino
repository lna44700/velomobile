/*  
 *   
 *  *********************** Code permettant à l'aide d'un commodo, de piloter les organes de la signalisation
 *  routière (clignotants, warnings, phares, klaxon et allumage d'un bandeau de DEL) situés sur un vélomobile
 *  et pilotés à l'aide d'un module arduino équipé d'une platine grove base shield v2.0. ********************
 *  
 *  *********************************************************************************************************
 *  ************* Par Valentin DOUET ************************************************************************
 *  ************* SN-IR 2 - 2016 **************************************************************************** 
 *  ************* Lycée Nicolas Appert (44) *****************************************************************
 *  *********************************************************************************************************
 *  
*/



// --------------------------------------------------------------------------------------------------------------- \\
//                                             ENTETE DECLARATIVE                                                  \\
// --------------------------------------------------------------------------------------------------------------- \\

#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd                ;
int iBoutonPhares = 3      ;                                         // Déclaration du bouton des phares sur la broche 3
int iPhares = 3            ;                                         // Déclaration des phares sur la broche 7
int iBoutonClignotants = 0 ;                                         // Déclaration du bouton du clignotant sur la broche 0
int iClignotantGauche = 6  ;                                         // Déclaration du clignotant gauche sur la broche 5
int iClignotantDroit = 7   ;                                         // Déclaration du clignotant droit sur la broche 6
int iBoutonWarnings = 2    ;                                         // Déclaration du bouton des warnings sur la broche 2
bool bEtatDel = true       ;                                         // Définition de l'état de la DEL à l'état haut
unsigned long lTimer = 0   ;                                         // Initialisation d'un timer 1 pour le calcul du temps
const int colorR = 50      ;                                         // Quantité de ROUGE sur la couleur de l'écran LCD
const int colorG = 255     ;                                         // Quantité de VERT sur la couleur de l'écran LCD
const int colorB = 255     ;                                         // Quantité de BLEU sur la couleur de l'écran LCD



// --------------------------------------------------------------------------------------------------------------- \\
//                                  FONCTION SETUP = Code d'initialisation                                         \\
// --------------------------------------------------------------------------------------------------------------- \\

void setup()
{
  Serial.begin(9600)                 ;
  pinMode(iBoutonPhares, INPUT)      ;                                // Bouton des phares en ENTREE
  pinMode(iBoutonClignotants, INPUT) ;                                // Bouton des clignotants en ENTREE
  pinMode(iPhares, OUTPUT)           ;                                // Phares en SORTIE
  pinMode(iClignotantGauche, OUTPUT) ;                                // Clignotant gauche en SORTIE
  pinMode(iClignotantDroit, OUTPUT)  ;                                // Clignotant droit en SORTIE  
  lcd.begin(16, 2)                   ;                                // Nombre de colonnes et de lignes de l'écran LCD
  lcd.setRGB(colorR, colorG, colorB) ;                                // Définition des couleurs de l'écran
}



// --------------------------------------------------------------------------------------------------------------- \\
//                                                  FONCTIONS                                                      \\
// --------------------------------------------------------------------------------------------------------------- \\

bool allumerClignotantGauche()                                            // Fonction qui sert à allumer clignotant gauche
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un timer 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers
  digitalWrite(iClignotantDroit, false)                 ;                 // On éteint le clignotant droit
 
  if (bEtatDel == true && lIntervalle > 500)                              // Si la variable bEtatDel est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {                                                              
    bEtatDel = false                                    ;                 // La variable bEtatDel passe à l'état bas
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On éteint le clignotant gauche
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (bEtatDel == false && lIntervalle > 500)                        // Sinon si la variable bEtatDel est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = true                                     ;                 // La variable bEtatDel passe à l'état haut
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On allume le clignotant gauche
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable bEtatDel)
}



bool allumerClignotantDroit()                                             // Fonction qui sert à allumer clignotant droit
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un timer 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers
  digitalWrite(iClignotantGauche, false)                ;                 // On éteint le clignotant gauche
 
  if (bEtatDel == true && lIntervalle > 500)                              // Si la variable bEtatDel est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = false                                    ;                 // La variable bEtatDel passe à l'état bas
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On éteint le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (bEtatDel == false && lIntervalle > 500)                        // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = true                                     ;                 // La variable bEtatDel passe à l'état haut
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On allume le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable bEtatDel)
}



bool allumerWarnings()                                                    // Fonction qui sert à allumer les warnings
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un timer 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers

  if (bEtatDel == true && lIntervalle > 500)                              // Si la variable bEtatDel est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = false                                    ;                 // La variable bEtatDel passe à l'état bas
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On éteint le clignotant gauche
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On éteint le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (bEtatDel == false && lIntervalle > 500)                        // Sinon si la variable bEtatDel est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = true                                     ;                 // La variable bEtatDel passe à l'état haut
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On allume le clignotant gauche
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On allume le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  Serial.println(lTimerCourant)                         ;
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable bEtatDel)
}



int afficherPublicite()                                                   // Fonction qui gère l'affichage de la publicité sur l'écran LCD
{
  lcd.setCursor(0, 0)                                   ;                 // On se place sur la ligne 0 (1ère ligne) et la colonne 0
  lcd.print("Velomobile Evoz")                          ;                 // Affichage du message "Velomobile Evoz"
  lcd.setCursor(2, 1)                                   ;                 // On se place sur le ligne 1 (2ème ligne) et la colonne 2
  lcd.print("Essayez-le !")                             ;                 // Affichage du message "Essayez-le !"
}



// --------------------------------------------------------------------------------------------------------------- \\
//                             FONCTION LOOP = Boucle sans fin = coeur du programme                                \\
// --------------------------------------------------------------------------------------------------------------- \\

void loop()
{
  int iEtatBoutonPhares = digitalRead(iBoutonPhares)          ;         // Lire l'état du bouton des phares et le stocker dans "iEtatBoutonPhares"
  int iEtatBoutonClignotants = analogRead(iBoutonClignotants) ;         // Lire l'état du bouton des clignotants et le stocker dans "iEtatBoutonClignotants"
  int iEtatBoutonWarnings = digitalRead(iBoutonWarnings)      ;         // Lire l'état du bouton des warnings et le stocker dans "iEtatBoutonWarnings"

  

  afficherPublicite()                                         ;         // Affichage de la publicité sur l'écran LCD

  

  // PHARES
  if (iEtatBoutonPhares == 1)                                           // Si le bouton des phares est enfoncé
    analogWrite(iPhares,1)      ;                                       // Les phares s'allument
  else                                                                  // Sinon
    analogWrite(iPhares,0)      ;                                       // Les phares ne s'allument pas



  // CLIGNOTANTS
  if ((iEtatBoutonClignotants >= 716) && (iEtatBoutonWarnings == 0))            // Si le potentiometre est tout au bout dans le sens des aiguilles d'une montre +- 30% ET les warnings sont éteints
    allumerClignotantGauche() ;                                                 // Allumage du clignotant gauche
  else if ((iEtatBoutonClignotants <= 307) && (iEtatBoutonWarnings == 0))       // Si le potentiometre est tout au bout dans le sens inverse des aiguilles d'une montre +- 30% ET les warnings sont éteints
    allumerClignotantDroit()  ;                                                 // Le clignotant droit s'allume



  // WARNINGS
  if ((iEtatBoutonWarnings == 1) || ((iEtatBoutonWarnings == 1)&& (iEtatBoutonClignotants >= 716)) || ((iEtatBoutonWarnings == 1) && (iEtatBoutonClignotants <= 307)))  // Si le bouton des warnings est appuyé OU (Warnings ET Clignotant Gauche) OU (Warnings ET Clignotant Droit)
  {
    allumerWarnings()                 ;                                                                                                                                 // Les warnings s'allument
  }
    if ((iEtatBoutonWarnings == 0) && (iEtatBoutonClignotants > 307) && (iEtatBoutonClignotants < 716))                                                                 // Si les warnings sont éteints ET les boutons des clignotants sont éteints
  {
    digitalWrite(iClignotantGauche,0)  ;                                                                                                                                // On éteint le clignotant gauche
    digitalWrite(iClignotantDroit,0)   ;                                                                                                                                // On éteint le clignotant droit
  }
}



// --------------------------------------------------------------------------------------------------------------- \\
//                                                   FIN DU PROGRAMME                                              \\
// --------------------------------------------------------------------------------------------------------------- \\
