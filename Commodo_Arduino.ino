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

int boutonPhares = 3      ;                                         // Déclaration du bouton des phares sur la broche 3
int phares = 3            ;                                         // Déclaration des phares sur la broche 7
int boutonClignotants = 0 ;                                         // Déclaration du bouton du clignotant sur la broche 0
int clignotantGauche = 6  ;                                         // Déclaration du clignotant gauche sur la broche 5
int clignotantDroit = 7   ;                                         // Déclaration du clignotant droit sur la broche 6
int boutonWarnings = 2    ;                                         // Déclaration du bouton des warnings sur la broche 2
byte etatDEL = HIGH       ;                                         // Définition de l'état de la DEL à l'état haut
unsigned long timer = 0   ;                                         // Initialisation d'un timer 1 pour le calcul du temps
int intensitePhares = 1   ;

// --------------------------------------------------------------------------------------------------------------- \\
//                                  FONCTION SETUP = Code d'initialisation                                         \\
// --------------------------------------------------------------------------------------------------------------- \\

void setup()
{
  Serial.begin(9600)                ;
  pinMode(boutonPhares, INPUT)      ;                                // Bouton des phares en ENTREE
  pinMode(boutonClignotants, INPUT) ;                                // Bouton des clignotants en ENTREE
  pinMode(phares, OUTPUT)           ;                                // Phares en SORTIE
  pinMode(clignotantGauche, OUTPUT) ;                                // Clignotant gauche en SORTIE
  pinMode(clignotantDroit, OUTPUT)  ;                                // Clignotant droit en SORTIE  
  pinMode(intensitePhares, INPUT)   ;
}

// --------------------------------------------------------------------------------------------------------------- \\
//                                                  FONCTIONS                                                      \\
// --------------------------------------------------------------------------------------------------------------- \\

byte allumerClignotantGauche()                                        // Fonction qui sert à allumer clignotant gauche
{
  unsigned long timerCourant = millis()             ;                 // Initialisation d'un timer 2
  unsigned long intervalle = timerCourant - timer   ;                 // Temps écoulé entre les deux timers
  digitalWrite(clignotantDroit, LOW)                ;                 // On éteint le clignotant droit
 
  if (etatDEL == HIGH && intervalle > 500)                            // Si la variable etatDEL est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {                                                              
    etatDEL = LOW                                   ;                 // La variable etatDEL passe à l'état bas
    digitalWrite(clignotantGauche, etatDEL)         ;                 // On éteint le clignotant gauche
    timer = timerCourant                            ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (etatDEL == LOW && intervalle > 500)                        // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    etatDEL = HIGH                                  ;                 // La variable etatDEL passe à l'état haut
    digitalWrite(clignotantGauche, etatDEL)         ;                 // On allume le clignotant gauche
    timer = timerCourant                            ;                 // Le timer 1 prend la valeur du timer 2
  }
  return etatDEL                                    ;                 // On retourne l'état de la DEL (variable etatDEL)
}




byte allumerClignotantDroit()                                         // Fonction qui sert à allumer clignotant droit
{
  unsigned long timerCourant = millis()             ;                 // Initialisation d'un timer 2
  unsigned long intervalle = timerCourant - timer   ;                 // Temps écoulé entre les deux timers
  digitalWrite(clignotantGauche, LOW)               ;                 // On éteint le clignotant gauche
 
  if (etatDEL == HIGH && intervalle > 500)                            // Si la variable etatDEL est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    etatDEL = LOW                                   ;                 // La variable etatDEL passe à l'état bas
    digitalWrite(clignotantDroit, etatDEL)          ;                 // On éteint le clignotant droit
    timer = timerCourant                            ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (etatDEL == LOW && intervalle > 500)                        // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    etatDEL = HIGH                                  ;                 // La variable etatDEL passe à l'état haut
    digitalWrite(clignotantDroit, etatDEL)          ;                 // On allume le clignotant droit
    timer = timerCourant                            ;                 // Le timer 1 prend la valeur du timer 2
  }
  return etatDEL                                    ;                 // On retourne l'état de la DEL (variable etatDEL)
}





byte allumerWarnings()                                                // Fonction qui sert à allumer les warnings
{
  unsigned long timerCourant = millis()             ;                 // Initialisation d'un timer 2
  unsigned long intervalle = timerCourant - timer   ;                 // Temps écoulé entre les deux timers

  if (etatDEL == HIGH && intervalle > 500)                            // Si la variable etatDEL est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    etatDEL = LOW                                   ;                 // La variable etatDEL passe à l'état bas
    digitalWrite(clignotantGauche, etatDEL)         ;                 // On éteint le clignotant gauche
    digitalWrite(clignotantDroit, etatDEL)          ;                 // On éteint le clignotant droit
    timer = timerCourant                            ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (etatDEL == LOW && intervalle > 500)                        // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    etatDEL = HIGH                                  ;                 // La variable etatDEL passe à l'état haut
    digitalWrite(clignotantGauche, etatDEL)         ;                 // On allume le clignotant gauche
    digitalWrite(clignotantDroit, etatDEL)          ;                 // On allume le clignotant droit
    timer = timerCourant                            ;                 // Le timer 1 prend la valeur du timer 2
  }
  return etatDEL                                    ;                 // On retourne l'état de la DEL (variable etatDEL)
}

// --------------------------------------------------------------------------------------------------------------- \\
//                             FONCTION LOOP = Boucle sans fin = coeur du programme                                \\
// --------------------------------------------------------------------------------------------------------------- \\

void loop()
{
  int etatBoutonPhares = digitalRead(boutonPhares)          ;         // Lire l'état du bouton des phares et le stocker dans "etatBoutonPhares"
  int etatBoutonClignotants = analogRead(boutonClignotants) ;         // Lire l'état du bouton des clignotants et le stocker dans "etatBoutonClignotants"
  int etatBoutonWarnings = digitalRead(boutonWarnings)      ;         // Lire l'état du bouton des warnings et le stocker dans "etatBoutonWarnings"
  int etatIntensitePhares = analogRead(intensitePhares)     ;



  // PHARES
  /*if (etatBoutonPhares == 1)                                     // Si le bouton des phares est enfoncé
    analogWrite(phares,1);                                      // Les phares s'allument
  else                                                           // Sinon
    analogWrite(phares,0) ;                                   // Les phares ne s'allument pas
    for (;i<255;i++) // i varie de 1 à 255
    {
    analogWrite(phares,i); // génère une impulsion sur la broche de largeur i => la luminosité augmente
    }
  for (;i>0;i--) // i varie de 1 à 255
    {
    analogWrite(phares,i); // génère une impulsion sur la broche de largeur i => la luminosité augmente
    }*/


  // CLIGNOTANTS
  if ((etatBoutonClignotants >= 716) && (etatBoutonWarnings == 0))            // Si le potentiometre est tout au bout dans le sens des aiguilles d'une montre +- 30% ET les warnings sont éteints
    allumerClignotantGauche() ;                                               // Allumage du clignotant gauche
  else if ((etatBoutonClignotants <= 307) && (etatBoutonWarnings == 0))       // Si le potentiometre est tout au bout dans le sens inverse des aiguilles d'une montre +- 30% ET les warnings sont éteints
    allumerClignotantDroit()  ;                                               // Le clignotant droit s'allume


  // WARNINGS
  if ((etatBoutonWarnings == 1) || ((etatBoutonWarnings == 1)&& (etatBoutonClignotants >= 716)) || ((etatBoutonWarnings == 1) && (etatBoutonClignotants <= 307))) // Si le bouton des warnings est appuyé OU (Warnings ET Clignotant Gauche) OU (Warnings ET Clignotant Droit)
  {
    allumerWarnings()                 ;                                                                                                                           // Les warnings s'allument
  }
    if ((etatBoutonWarnings == 0) && (etatBoutonClignotants > 307) && (etatBoutonClignotants < 716))                                                              // Si les warnings sont éteints ET les boutons des clignotants sont éteints
  {
    digitalWrite(clignotantGauche,0)  ;                                                                                                                           // On éteint le clignotant gauche
    digitalWrite(clignotantDroit,0)   ;                                                                                                                           // On éteint le clignotant droit
  }
}

// --------------------------------------------------------------------------------------------------------------- \\
//                                                   FIN DU PROGRAMME                                              \\
// --------------------------------------------------------------------------------------------------------------- \\
