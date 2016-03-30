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

int iBoutonPhares = 3      ;                                         // Déclaration du bouton des phares sur la broche 3
int iPhares = 3            ;                                         // Déclaration des phares sur la broche 7
int iBoutonClignotants = 0 ;                                         // Déclaration du bouton du clignotant sur la broche 0
int iClignotantGauche = 6  ;                                         // Déclaration du clignotant gauche sur la broche 5
int iClignotantDroit = 7   ;                                         // Déclaration du clignotant droit sur la broche 6
int iBoutonWarnings = 2    ;                                         // Déclaration du bouton des warnings sur la broche 2
bool bEtatDel = HIGH       ;                                         // Définition de l'état de la DEL à l'état haut
unsigned long lTimer = 0   ;                                         // Initialisation d'un timer 1 pour le calcul du temps
int iIntensitePhares = 1   ;

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
  pinMode(iIntensitePhares, INPUT)   ;
}

// --------------------------------------------------------------------------------------------------------------- \\
//                                                  FONCTIONS                                                      \\
// --------------------------------------------------------------------------------------------------------------- \\

bool allumerClignotantGauche()                                            // Fonction qui sert à allumer clignotant gauche
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un timer 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers
  digitalWrite(iClignotantDroit, LOW)                   ;                 // On éteint le clignotant droit
 
  if (bEtatDel == HIGH && lIntervalle > 500)                              // Si la variable etatDEL est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {                                                              
    bEtatDel = LOW                                      ;                 // La variable etatDEL passe à l'état bas
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On éteint le clignotant gauche
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (bEtatDel == LOW && lIntervalle > 500)                          // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = HIGH                                     ;                 // La variable etatDEL passe à l'état haut
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On allume le clignotant gauche
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable etatDEL)
}




bool allumerClignotantDroit()                                             // Fonction qui sert à allumer clignotant droit
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un timer 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers
  digitalWrite(iClignotantGauche, LOW)                  ;                 // On éteint le clignotant gauche
 
  if (bEtatDel == HIGH && lIntervalle > 500)                              // Si la variable etatDEL est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = LOW                                      ;                 // La variable etatDEL passe à l'état bas
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On éteint le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (bEtatDel == LOW && lIntervalle > 500)                          // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = HIGH                                     ;                 // La variable etatDEL passe à l'état haut
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On allume le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable etatDEL)
}





bool allumerWarnings()                                                    // Fonction qui sert à allumer les warnings
{
  unsigned long lTimerCourant = millis()                ;                 // Initialisation d'un timer 2
  unsigned long lIntervalle = lTimerCourant - lTimer    ;                 // Temps écoulé entre les deux timers

  if (bEtatDel == HIGH && lIntervalle > 500)                              // Si la variable etatDEL est à l'état haut et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = LOW                                      ;                 // La variable etatDEL passe à l'état bas
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On éteint le clignotant gauche
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On éteint le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  else if (bEtatDel == LOW && lIntervalle > 500)                          // Sinon si la variable etatDEL est à l'état bas et que le temps écoulé est supérieur à 500 ms
  {
    bEtatDel = HIGH                                     ;                 // La variable etatDEL passe à l'état haut
    digitalWrite(iClignotantGauche, bEtatDel)           ;                 // On allume le clignotant gauche
    digitalWrite(iClignotantDroit, bEtatDel)            ;                 // On allume le clignotant droit
    lTimer = lTimerCourant                              ;                 // Le timer 1 prend la valeur du timer 2
  }
  Serial.println(lTimerCourant) ;
  return bEtatDel                                       ;                 // On retourne l'état de la DEL (variable etatDEL)
}

// --------------------------------------------------------------------------------------------------------------- \\
//                             FONCTION LOOP = Boucle sans fin = coeur du programme                                \\
// --------------------------------------------------------------------------------------------------------------- \\

void loop()
{
  int iEtatBoutonPhares = digitalRead(iBoutonPhares)          ;         // Lire l'état du bouton des phares et le stocker dans "etatBoutonPhares"
  int iEtatBoutonClignotants = analogRead(iBoutonClignotants) ;         // Lire l'état du bouton des clignotants et le stocker dans "etatBoutonClignotants"
  int iEtatBoutonWarnings = digitalRead(iBoutonWarnings)      ;         // Lire l'état du bouton des warnings et le stocker dans "etatBoutonWarnings"
  int iEtatIntensitePhares = analogRead(iIntensitePhares)     ;



  // PHARES
  if (iEtatBoutonPhares == 1)                                     // Si le bouton des phares est enfoncé
    analogWrite(iPhares,1);                                       // Les phares s'allument
  else                                                            // Sinon
    analogWrite(iPhares,0) ;                                      // Les phares ne s'allument pas
    /*for (;i<255;i++) // i varie de 1 à 255
    {
    analogWrite(phares,i); // génère une impulsion sur la broche de largeur i => la luminosité augmente
    }
  for (;i>0;i--) // i varie de 1 à 255
    {
    analogWrite(phares,i); // génère une impulsion sur la broche de largeur i => la luminosité augmente
    }*/


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
