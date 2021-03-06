/**
 * @file CodeBatterie.ino
 * @brief Programme Velomobile Arduino Mega.
 * @author Guillaume.F
 * @version 1.0
 * @date 17 Juin 2016
 *
 * Programme de récupération d'informations. Tension et intensité de la batterie moteur, vitesse du vélo,distance parcourue
 * position GPS, altitude, température moteur. Calcule la puissance délivrée par la batterie moteur, la capacité, les ampères heure.
 * Enregistre toutes ces données sur une carte SD dans un fichier au format CSV. Un fichier de configuration dans la carte SD permet
 * de définir le périmètre de la roue et la capacité de la batterie moteur.
 *
 */

/**
 * @brief implémentation des librairies
 */
 
#include <Adafruit_Sensor.h>                                          // Librairie permettant d'uriliser les capteur adafruif
#include <Adafruit_TMP006.h>                                          // Librairie du capteur de température
#include <RTClib.h>                                                   // Ulisation de l'horloge temps réel
#include <SD.h>                                                       // Librairie permettant de sauvegarder des informations sur une carte SD
#include <SPI.h>                                                      // Permet d'utiliser la communication via le bus SPI de l'arduino
#include <SoftwareSerial.h>                                           // Librairie permetteant de mettre en oeuvre le module Bluetooth
#include <Wire.h>                                                     // Permet d'utiliser la communication I2C
#include "rgb_lcd.h"                                                  // Librairie de l'écran LCD
#include <Ticks.h>                                                    // Compte des impulsions sur un certains laps de temps et en déduis la fréquence
#include <TinyGPS.h>                                                  // Librairie permettant d'utiliser le module MonGPS

/**
 * @brief déclaration des objets
 */                                

SoftwareSerial Bluetooth(10, 11) ;                                    // RX | TX
SoftwareSerial GPS(12, 13)       ;                                    // Déclaration des broches RX | TX pour le module MonGPS
rgb_lcd MonEcran                 ;                                    // Création de l'objet MonEcran de type rgb_lcd
Adafruit_TMP006 Temp             ;                                    // Création de l'objet tmp006 pour le capteur de température
TinyGPS MonGPS                   ;                                    // Création de l'objet MonGPS
RTC_DS1307 RTC;                                                       // Classe RTC_DS1307 
File Rapport ;                                                        // Permet de créer le fichier de rapport .CSV
File Config  ;                                                        // Permet de créerle fichier de configuration .ini

const byte ValeurCapteur = 2     ;                                    // Le capteur du compteur de vitesse est câblé à la broche numéro 2
const byte numInterrupt = 0      ;                                    // Numéro de la broche d'interruption
const int Periode = 500          ;                                    // Période en milliseconde, permet d'avoir la fréquence instantanée du passage de l'aimant
const byte BUFFER_SIZE = 32      ;                                    // Taille du buffer (de la mémoire tampon) pour lire le fichier de configuration

Ticks  Compteur (numInterrupt, ValeurCapteur, Periode) ;              // Appel du constructeur de la librairie Ticks permettant d'avoir accès aux fonctions associées

/**
 * @brief déclaration des différentes variables
 */

float Tension (0.0)              ;                                    // tension de la batterie
float Intensite  (0.0)           ;                                    // Intensite
float Capacite (0.0)             ;                                    // Capacité de la batterie en Ah
float CapRAZ (0.0)               ;
int Puissance (0.0)              ;                                    // Puissance délivrée par la batterie principale
float PuissanceConsommee (0.0)   ;                                    // Puissance consommée par la batterie en Wh
float PuissanceConsommeeKM (0.0) ;                                    // Puissance consommée par kilomètre par la batterie en Wh/km
float Vitesse (0.0)              ;                                    // Vitesse du vélo
float Distance (0.0)             ;                                    // Distance parcourue par le vélo
float Aimant (0)                 ;                                    // Passage de l'aimant du compteur de vitesse
int  BoutonChoixEcran (0)        ;                                    // Bouton permettant d'afficher sur l'écran LCD les tensions, l'intensité, la puissance et la distance parcourue
int CompteurBoucle (0)           ;                                    // Variable qui compte le nombre de fois que la boucle loop() à été faite
char ValeurPrecedente(0)         ;                                    // Valeur précédente du bouton permettant de bloquer l'écran
char Bouton (0)                  ;                                    // Variable qui va prendre la valeur de la lecture du bouton de blocage de l'écran
char ChangementEtat (0)          ;                                    // Variable qui enregistre l'état précédant le passage de l'aimant devant le capteur
char ValeurPrecedenteDist(0)     ;                                    // Variable qui va prendre la valeur de la variable ChangementEtat
unsigned long Temps = 0L         ;                                    // Variable de temps qui prendra la valeur du temps actuel
unsigned long Intervalle = 0L    ;                                    // Variable qui permet de définir un intervalle de temps
float Perimetre (0.0)            ;                                    // Périmètre de la roue
const int chipSelect = 10        ;                                    // Selection de la broche pour utiliser la librairie RTC
float Temperature (0.0)          ;                                    // Température du moteur
float Altitude (0.0)             ;                                    // Altitude mesurée par le GPS
float VitesseGPS (0.0)           ;                                    // Vitesse donnée par le GPS
char Latitude[32]                ;                                    // Latitude
char Longitude[32]               ;                                    // Longitude
float Ah (0.0)                   ;                                    // Ampère Heure consommée
float Lat (0.0)                  ;                                    // Variable qui contient la valeur de la latitude mesuré par le GPS
float Lon (0.0)                  ;                                    // Variable qui contient la valeur de la longitude mesuré par le GPS
char NomCSV[16]                  ;                                    // Tableau qui permet la construction du nom du fichier CSV
int Jour                         ;                                    // Jour actuel
int Mois                         ;                                    // Mois actuel
int Annee                        ;                                    // Année Actuelle
int Heure                        ;                                    // Heure actuelle
int Minute                       ;                                    // Minute Actuelle


static void gpsdump(TinyGPS &MonGPS)                                     ;   // Fonction où les données sont récupérées par le GPS
static bool feedgps()                                                    ;   // Vérifie la présence du module GPS
static void print_float(float val, float invalid, int len, int prec)     ;   // Fabrique les nombres flotant                                 

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

class CapteurVitesse                                                 // Création de la classe CapteurVitesse
{
  private:                                                           // Attributs
    float Vitesse     ;

  public:                                                            // Méthodes
    CapteurVitesse()  ;
    ~CapteurVitesse() ;

    float Get_Vitesse () ;                                           // Méthode d'acquisition de la vitesse                                     
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

Batterie::Batterie():                                                // Constructeur
  Tension(0.0),
  Intensite(0.0),
  Puissance(0.0)
{

}

Batterie::~Batterie()                                                // Destructeur
{
  this->Tension = 0.0   ;
  this->Intensite = 0.0 ;
  this->Puissance = 0.0 ;
}

CapteurVitesse::CapteurVitesse():                                    // Constructeur
  Vitesse(0.0)
{

}

CapteurVitesse::~CapteurVitesse()                                    // Destructeur
{
  this->Vitesse = 0.0     ;
}

CapteurTemperature::CapteurTemperature():                            // Constructeur
  Temperature(0.0)
{

}

CapteurTemperature::~CapteurTemperature()                            // Destructeur
{
  this->Temperature = 0.0 ;
}

/**
 * @brief Fonction qui mesure la tension délivrée par la batterie
 * @return Tension
 */

float Batterie::Get_Tension()
{
  Tension = analogRead (0)               ;                              // Lire la Tension délivrée par la batterie
  Tension = Tension * 5 / 1023 * 10.2854 ;                              // Calcul permettant d'afficher la tension en volt

  return Tension ;
}

/**
 * @brief Fonction qui mesure le courant délivré par la batterie
 * @return Intensite
 */

float Batterie::Get_Intensite()
{
  Intensite = analogRead (2)                           ;               // Lire l'intensité délivrée par la batterie principal
  Intensite = (Intensite * 5 / 1023) * (73.3 / 5) - 37.4 ;               // Calcul permettant d'afficher l'intensité en ampère

  if (Intensite <= 0.5)
  {
    Intensite = 0.0
  }

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

/**
 * @fn float CapteurVitesse::Get_Vitesse(void)
 * @brief Fonction qui calcule la vitesse du Vélomobile
 * @return Vitesse
 */

float CapteurVitesse::Get_Vitesse()
{
  Compteur.operate()                  ;                                // Fonction qui met à Jour la fréquence instantanée
  Aimant = Compteur.TickRate25Period() ;                                // Nombre de passage de l'aimant par seconde
  Vitesse = Aimant * Perimetre * 3.6  ;                                // Calcul de la vitesse du vélo

  return Vitesse ;
}

/**
 * @fn float CapteurTemperature::Get_Temperature(void)
 * @brief Fonction qui calcule la température du moteur
 * @return Temperature
 */

float CapteurTemperature::Get_Temperature()
{
  Temperature = Temp.readObjTempC() ;                                // Mesure de la température

  return Temperature ;
}

/**
 * @fn void EnvoyerBluetooth(float, float, float, float, float, float)
 * @brief Methode qui permet d'envoyer de toutes les informations récupérées en bluetooth vers le smartphone Android
 * @param Tension est la tension délivrée par la batterie
 * @param Intensite est le courant délivré par la batterie
 * @param Puissance est la puissance délivrée par la batterie
 * @param Vitesse est la vitesse du Vélomobile
 * @param Distance est la distance parcourue par l'utilisateur
 */

void EnvoyerBluetooth(float Tension, float Intensite, float Puissance,
                      float Vitesse, float Distance, float Capacite)             // Fonction d'affichage de la tension, de l'intensité, de la vitesse, de la distance sur smartphone
{                       
  Bluetooth.println(Tension)         ;                                           // Affichage de la tension sur le téléphone portable
                        
  Bluetooth.println(Intensite)       ;                                           // Affichage de l'intensité sur le téléphone portable
                        
  Bluetooth.println(Puissance)       ;                                           // Affichage de la puissance délivrée sur le téléphone portable
                                       
  Bluetooth.println(Vitesse)         ;                                           // Affichage de la vitesse sur le téléphone portable
                                    
  Bluetooth.println(Distance / 1000) ;                                           // Affichage de la distance parcourue sur le téléphone portable

  Bluetooth.println(Capacite)        ;                                           // Affichage de la charge de la batterie sur le téléphone portable
}

/**
 * @fn void AfficherInfo(float, float, float, float)
 * @brief Methode qui permet l'affichage des informations sur l'écran LCD
 * @param Tension est la tension délivrée par la batterie
 * @param Intensite est le courant délivré par la batterie
 * @param Vitesse est la vitesse du Vélomobile
 * @param Distance est la distance parcourue par l'utilisateur
 */
 
void AfficherInfo(float Tension, float Intensite, float Distance, float Vitesse)      // Fonction d'affichage de la tension, de l'intensité, de la distance et de
{                                                                                     // la vitesse sur l'écran LCD
  MonEcran.setCursor(0, 0)     ;                       // Positionne le curseur à la colonne 0 et à la ligne 0
  MonEcran.print(Tension, 1)   ;
  MonEcran.print(" V ")        ;
  MonEcran.setCursor(8, 0)     ;                       // 8ème Caractère de la ligne 0
  MonEcran.print(Intensite, 1) ;
  MonEcran.print(" A ")        ;

  MonEcran.setCursor(0, 1)         ;
  MonEcran.print(Distance/1000, 1) ;
  MonEcran.print("Km ")            ;
  MonEcran.setCursor(7, 1)         ;  
  MonEcran.print(Vitesse, 1)       ;
  MonEcran.print(" Km/h ")         ;
}

/**
 * @fn void AfficherInfo2(int, float, float, float)
 * @brief Methode qui permet l'affichage des informations sur l'écran LCD
 * @param Puissance est la puissance délivrée par la batterie
 * @param Capacite est la capacité de la batterie restante
 * @param PuissanceConsommee est la puissance consommée par la batterie en WattHeure
 * @param Temperature est la température du moteur
 */

void AfficherInfo2(int Puissance, float Capacite, float PuissanceConsommee, float Temperature)   // Fonction d'affichage de la puisance délivrée et de la capacité de la batterie 
{                                                                                                // sur l'écran LCD
  MonEcran.setCursor(0, 0)     ;                       
  MonEcran.print(Puissance)    ;
  MonEcran.print(" W ")        ;
  MonEcran.setCursor(8, 0)     ;                       
  MonEcran.print(Capacite, 1)  ;
  MonEcran.print(" Ah ")       ;

  MonEcran.setCursor(0, 1)              ;
  MonEcran.print(PuissanceConsommee, 1) ;
  MonEcran.print(" Wh ")                ;
  MonEcran.setCursor(9, 1)              ;
  MonEcran.print(Temperature)           ;
  MonEcran.print(" C")                  ;
}

static void gpsdump(TinyGPS &MonGPS)
{
  MonGPS.f_get_position(&Lat, &Lon)                        ;                         // Obtient les coordonnées du GPS
  print_floatLat(Lat, TinyGPS::GPS_INVALID_F_ANGLE, 9, 5)  ;                         // Permet d'obtenir la valeur de la latitude
  print_floatLon(Lon, TinyGPS::GPS_INVALID_F_ANGLE, 10, 5) ;                         // Permet d'obtenir la valeur de la longitude

  Altitude = MonGPS.f_altitude() ;                                                   // Obtient l'altitude à laquelle ce trouve le module GPS
  if (Altitude = 1000.00000)
  {
    Altitude = 0.0 ;
  }
  VitesseGPS = MonGPS.f_speed_kmph() ;                                                // Obtient la vitesse à laquelle se déplace le module GPS
  if (VitesseGPS <= 0)
  {
    VitesseGPS = 0.0 ;
  }
}

/**
 * @brief Méthode permettant de transformer la Latitude en type float qui est de base sous forme de tableau de chaîne de caractère
 * @param val est la valeur de la variable Lat obtenue dans la méthode gpsdump
 * @param invalid permet de savoir si la variable val est correcte
 * @param len 
 * @param prec 
 */

static void print_floatLat(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    strcpy(Latitude, "*******") ;
    Latitude[len] = 0           ;
        if (len > 0) 
          Latitude[len-1] = ' ' ;
    for (int i=7; i<len; ++i)
        Latitude[i] = ' ' ;
  }
  else
  {
    int vi = abs((int)val)                                    ;
    int flen = prec + (val < 0.0 ? 2 : 1)                     ;
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1 ;
  }
  feedMonGPS() ;
}

/**
 * @brief Méthode permettant de transformer la Longitude en type float qui est de base sous forme de tableau de chaîne de caractère
 * @param val est la valeur de la variable Lon obtenue dans la méthode gpsdump
 * @param invalid permet de savoir si la variable val est correcte
 * @param len 
 * @param prec 
 */

static void print_floatLon(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    strcpy(Longitude, "*******") ;
    Longitude[len] = 0           ;
        if (len > 0) 
          Longitude[len-1] = ' ' ;
    for (int i=7; i<len; ++i)
        Longitude[i] = ' ' ;
  }
  else
  {
    int vi = abs((int)val)                                    ;
    int flen = prec + (val < 0.0 ? 2 : 1)                     ;
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1 ;
  }
  feedMonGPS() ;
}

static bool feedMonGPS()
{
  while (GPS.available())
  {
    if (MonGPS.encode(GPS.read()))
      return true ;
  }
  return false ;
}

/**
 * @li Creation de l'objet BatterieVelo de type Batterie
 */
Batterie BatterieVelo                 ;
/**
 * @li Creation de l'objet CapteurVitesse de type CapteurVitesse
 */
CapteurVitesse CapteurVitesse         ;
/**
 * @li Creation de l'objet CapteurTemperature de type CapteurTemperature
 */
CapteurTemperature CapteurTemperature ;

/**
 * @brief Definition des couleurs d'affichage de l'écran LCD
 */

const int colorR = 255 ;                                               // Intensité de la couleur Rouge de l'écran LCD
const int colorG = 255 ;                                               // Intensité de la couleur Vert de l'écran LCD
const int colorB = 255 ;                                               // Intensité de la couleur Bleu de l'écran LCD

/**
 * @fn void setup()
 * @brief Définition des entrées / sorties de la carte Arduino, démarrage des librairies utilisées
 */

void setup()
{
  Serial.begin(9600)                      ;                            // Paramètre de la vitesse de transmission USB
  Wire.begin()                            ;                            // Permet d'utiliser le bus I2C (Inter-Integrated Circuit)
  RTC.begin()                             ;                            // Démarrage de la librairie RTClib.h                          
  Serial.println("\nInitialisation de la carte SD...") ;               // Affichage sur le moniteur
  Compteur.begin()                        ;                            // Démarre le processus de la librairie Ticks pour le compteur de vitesse
  pinMode (0, INPUT)                      ;                            // Déclaration de la broche où est câblé la batterie en Entrée
  pinMode (4, INPUT)                      ;                            // Déclaration de la broche où est câblé le bouton de changement d'affichage LCD
  pinMode (2, INPUT)                      ;                            // Déclaration de la broche où est câblé le capteur de courant de la batterie principale
  pinMode (6, INPUT)                      ;
  Bluetooth.begin(9600)                   ;                            // Vitesse de transmission du bluetooth
  MonEcran.begin(16, 2)                   ;                            // Déclaration de l'affichage de l'écran LCD
  MonEcran.setRGB(colorR, colorG, colorB) ;                            // Couleur d'affichage de l'écran LCD
  pinMode(53, OUTPUT)                     ;                            // Déclaration de la broche 53 en sortie : sauvegarde sur la carte SD
  SD.begin(53, 51, 50, 52)                ;                            // Déclaration des broches à utiliser pour la gestion de la carte SD
  GPS.begin(9600)                         ;
  DateTime now = RTC.now()                ;                            // La date now prend la valeur que donne la la librairie RTC
  
  if (! RTC.isrunning())                                               // Si RTC ne fonctionne pas
  {
    Serial.println("RTC ne fonctionne pas !") ;
    RTC.adjust(DateTime(__DATE__, __TIME__))  ;                        // Met à l'Heure à date à laquelle le sketch est compilé
  }

  Jour = now.day()      ;                                              // Jour prend la valeur du jour actuel
  Mois = now.month()    ;                                              // Mois prend la valeur du mois actuel
  Annee = now.year()    ;                                              // Annee prend la valeur de l'année actuelle
  Heure = now.hour()    ;                                              // Heure prend la valeur de l'heure actuelle
  Minute = now.minute() ;                                              // Minute prend la valeur de la minute actuelle

  sprintf(NomCSV,"%d%02d%d.CSV",Annee,Mois,Jour);                      // %d pour un int, on construit le nom du fichier CSV avec l'année, le mois et le jour auquel
                                                                       // à été créé le fichier
  char buffer[BUFFER_SIZE], *key, *value  ;                            // Déclaration du buffer
  byte i, buffer_lenght, line_counter = 0 ;
  Config = SD.open("config.ini")          ;                            // Ouverture du fichier config.ini à lire
  if(!Config) 
  { // Gère les erreurs
    Serial.println("Erreur d'ouverture du fichier !") ;
    for(;;)                                           ;
  }
 
  /* Tant que non fin de fichier */
  while(Config.available() > 0 )
  { 
    /* Récupère une ligne entière dans le buffer */
    i = 0 ;
    while((buffer[i++] = Config.read()) != '\n') 
    { 
      /* Si la ligne dépasse la taille du buffer */
      if(i == BUFFER_SIZE) 
      { 
        /* On finit de lire la ligne mais sans stocker les données */
        while(Config.read() != '\n') ;
        break                        ; // Et on arrête la lecture de cette ligne
      }
    }
 
    /* On garde de côté le nombre de char stocké dans le buffer */
    buffer_lenght = i ;
 
    /* Gestion des lignes trop grande */
    if(i == BUFFER_SIZE) 
    {
      Serial.print("Ligne trop longue à la ligne ") ;
      Serial.println(line_counter, DEC)             ;
    }
 
    /* Finalise la chaine de caractéres ASCIIZ en supprimant le \n au passage */
    buffer[--i] = '\0' ;
 
    /* Incrémente le compteur de lignes */
    ++line_counter ;
 
    /* Ignore les lignes vides ou les lignes de commentaires */
    if(buffer[0] == '\0' || buffer[0] == '#') continue ;
       
    /* Cherche l'emplacement de la clef en ignorant les espaces et les tabulations en début de ligne */
    i = 0 ;
    while(buffer[i] == ' ' || buffer[i] == '\t') 
    {
      if(++i == buffer_lenght) break ;                                              // Ignore les lignes contenant uniquement des espaces et/ou des tabulations
    }
    if(i == buffer_lenght) continue ;                                               // Gère les lignes contenant uniquement des espaces et/ou des tabulations
    key = &buffer[i]                ;
 
    /* Cherche l'emplacement du séparateur = en ignorant les espaces et les tabulations âpres la clef */
    while(buffer[i] != '=') 
    { 
      /* Ignore les espaces et les tabulations */
      if(buffer[i] == ' ' || buffer[i] == '\t') buffer[i] = '\0' ;
         
      if(++i == buffer_lenght) 
      {
        Serial.print("Ligne mal forme a la ligne ") ;
        Serial.println(line_counter, DEC)           ;
        break;                                                                        // Ignore les lignes mal formé
      }
    }
    if(i == buffer_lenght) continue ;                                                 // Gère les lignes mal formé
 
    /* Transforme le séparateur = en \0 et continue */
    buffer[i++] = '\0';
 
    /* Cherche l'emplacement de la valeur en ignorant les espaces et les tabulations âpres le séparateur */
    while(buffer[i] == ' ' || buffer[i] == '\t') 
    {
      if(++i == buffer_lenght) 
      {
        Serial.print("Ligne mal forme a la ligne ") ;
        Serial.println(line_counter, DEC)           ;
        break;                                                                          // Ignore les lignes mal formé
      }
    }
    if(i == buffer_lenght) continue ;                                                   // Gère les lignes mal formé
    value = &buffer[i]              ;
 
    /* Transforme les données texte en valeur utilisable */
    /* C'est ce morceaux de code qu'il vous faudra adapter pour votre application😉 */
    if(strcmp(key, "Perimetre") == 0) 
    {
      Perimetre = atof(value) ;
    } 
    else if(strcmp(key, "Capacite") == 0) 
    {
      Capacite = atof(value) ;
    } 
    else if(strcmp(key, "Distance") == 0) 
    {
      Distance = atof(value) ;
    }
    else if(strcmp(key, "CapaciteInit") == 0)
    {
      CapRAZ = atof(value) ; 
    }
    else 
    { // Default 
      Serial.print("Clef inconnu ") ;
      Serial.println(key)           ;
    }
 
  }
 
  /* Ferme le fichier de configuration */
  Config.close() ;

  Rapport = SD.open(NomCSV, FILE_WRITE) ;                            // Création du fichier Rapport.CSV
  Rapport.println("Date;Tension;Intensite;Puissance;Vitesse;Distance;"
                  "Puissance Consommee (Wh);Puissance Consommee par Km (Wh/km);"
                  "Capacite (Ah);Altitude (m);VitesseGPS (km/h);Latitude;Longitude") ;
  Rapport.close() ;
}
 
float VitessePrec(0.0);
float VitesseSauv(0.0) ;

/**
 * @fn void loop()
 * @brief Entrée du programme
 */

void loop()
{
  DateTime now = RTC.now() ;                                     // La date now prend la valeur que donne la la librairie RTC
  
  Jour=now.day()        ;
  Mois = now.month()    ;
  Annee= now.year()     ;
  Heure = now.hour()    ;
  Minute = now.minute() ; 
  

  if (digitalRead(6) == HIGH)
  {
    Distance = 0.0 ;
    Capacite = CapRAZ ;
  }

  bool newdata = false ;
  
  if (feedMonGPS())
  {
    newdata = true ;
  }
    
  gpsdump(MonGPS) ;
  
  Bouton = digitalRead(4) ;                                            // Lecture de l'état du bouton de blocage de l'écran
  
  if (Bouton == 1 && Bouton != ValeurPrecedente)                       // Condition de changement d'état
  {
    BoutonChoixEcran = 1 ;
  }
  
  ValeurPrecedente = Bouton ;                                          // Enregistrement de l'état actuel du bouton


  
  Tension = BatterieVelo.Get_Tension()                           ;     // Enregitrement du retour de la méthode Get_Tension dans la variable Tension
  Intensite = BatterieVelo.Get_Intensite()                       ;     // Enregitrement du retour de la méthode Get_Intensite dans la variable Intensite
  Puissance = BatterieVelo.CalculerPuissance(Tension, Intensite) ;     // Enregitrement du retour de la méthode CalculerPuissance dans la variable Puissance
  for (int j = 0; j < 2 ; j++)
  {
    Vitesse = CapteurVitesse.Get_Vitesse()                         ;     // Enregitrement du retour de la méthode Get_Vitesse dans la variable Vitesse
    VitessePrec = CapteurVitesse.Get_Vitesse() ;
    Vitesse = (Vitesse + VitessePrec)/2 ;
  }
  Temperature = CapteurTemperature.Get_Temperature()             ;     // Enregistrement du retour de la méthode Get_Temperature dans la variable Temperature

  PuissanceConsommee = Intensite * Tension                       ;     // Calcul de la puissance consommée
  PuissanceConsommeeKM = PuissanceConsommee / Distance           ;     // Calcul de la puissance consommée par kilomètre
  if (Distance == 0.0)
  {
    PuissanceConsommeeKM = 0.0 ;
  }
  
  ChangementEtat = digitalRead(2) ;                                    // Lecture de l'état du capteur de vitesse

  if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
  {
    Distance = Perimetre + Distance ;                                  // Calcul de la distance parcourue
  }
  //Serial.println(Distance);
  ValeurPrecedenteDist = ChangementEtat ;                              // Enregistrement de l'état actuel du capteur

  unsigned long TempsContinu = millis() ;                              // Variable de temps
  
  Intervalle = TempsContinu - Temps ;                                  // Intervalle de temps qui permet d'effctuer une action de manière régulière et connu.

  if (Intervalle >= 2000)                                              // Toute les secondes s'effectuera les actions présentent dans la condition.
  {
    Ah = Intensite * (Intervalle/1000) / 3600                  ;
    Capacite = Capacite - Ah                                       ;
    EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance, Capacite) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

    Rapport = SD.open(NomCSV, FILE_WRITE)                 ;
    Rapport.print(Annee), Rapport.print("_"), Rapport.print(Mois), Rapport.print("_"), Rapport.print(Jour), Rapport.print(" "),
    Rapport.print(Heure), Rapport.print(":"), Rapport.print(Minute), Rapport.print(';') ;
    Rapport.print(Tension), Rapport.print(';')              ;
    Rapport.print(Intensite), Rapport.print(';')            ;
    Rapport.print(Puissance), Rapport.print(';')            ;
    Rapport.print(Vitesse), Rapport.print(';')              ;
    Rapport.print(Distance/1000), Rapport.print(';')        ;
    Rapport.print(PuissanceConsommee), Rapport.print(';')   ;
    Rapport.print(PuissanceConsommeeKM), Rapport.print(';') ;
    Rapport.print(Capacite), Rapport.print(';')             ;
    Rapport.print(Altitude), Rapport.print(';')             ;
    Rapport.print(VitesseGPS), Rapport.print(';')           ;
    Rapport.print(Lat,5), Rapport.print(';')                ;
    Rapport.print(Lon,5), Rapport.print(';')                ;
    Rapport.println()                                       ;
    Rapport.close()                                         ;

    SD.remove("config.ini") ;
    Config = SD.open("config.ini", FILE_WRITE) ;
    Config.print("Perimetre="), Config.println(Perimetre);
    Config.print("Capacite="), Config.println(Capacite);
    Config.print("Distance="), Config.println(Distance);
    Config.print("CapaciteInit="), Config.println(CapRAZ);
    Config.close();
    
    Temps = TempsContinu ;                                                // Mise à Jour de la variable de Temps (variable tempon)
  }
  

  if (CompteurBoucle < 200)
  {
    AfficherInfo(Tension, Intensite, Distance, Vitesse) ;                 // Appel de la méthode d'affichage de la Tension, de l'intensité, de la distance et de la vitesse

    while (BoutonChoixEcran == 1)                                         // Si le bouton de blocage à été appuyer alors l'affichage reste sur les informations en cours
    {
      if (digitalRead(6) == HIGH)
      {
        Distance = 0.0 ;
        Capacite = CapRAZ ;
      }
      
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
      Temperature = CapteurTemperature.Get_Temperature()             ;     // Enregistrement du retour de la méthode Get_Temperature dans la variable Temperature

   
      PuissanceConsommee = Intensite * Tension                       ;     // Calcul de la puissance consommée
      PuissanceConsommeeKM = PuissanceConsommee / (Distance)         ;     // Calcul de la puissance consommée par kilomètre

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                                     
      }
      ValeurPrecedenteDist = ChangementEtat ;

      unsigned long TempsContinu = millis() ;
      Intervalle = TempsContinu - Temps     ;

      if (Intervalle >= 2000)
      {
        Ah = Intensite * (Intervalle/1000) / 3600                  ;
        Capacite = Capacite - Ah                                       ;
        EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance, Capacite) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

        Rapport = SD.open(NomCSV, FILE_WRITE)            ;
        Rapport.print(Annee), Rapport.print("_"), Rapport.print(Mois), Rapport.print("_"), Rapport.print(Jour), Rapport.print(" "),
        Rapport.print(Heure), Rapport.print(" "), Rapport.print(Minute), Rapport.print(';') ;
        Rapport.print(Tension), Rapport.print(';')              ;
        Rapport.print(Intensite), Rapport.print(';')            ;
        Rapport.print(Puissance), Rapport.print(';')            ;
        Rapport.print(Vitesse), Rapport.print(';')              ;
        Rapport.print(Distance/1000), Rapport.print(';')        ;
        Rapport.print(PuissanceConsommee), Rapport.print(';')   ;
        Rapport.print(PuissanceConsommeeKM), Rapport.print(';') ;
        Rapport.print(Capacite), Rapport.print(';')             ;
        Rapport.print(Altitude), Rapport.print(';')             ;
        Rapport.print(VitesseGPS), Rapport.print(';')           ;
        Rapport.print(Lat,5), Rapport.print(';')                ;
        Rapport.print(Lon,5), Rapport.print(';')                ;
        Rapport.println()                                       ;
        Rapport.close()                                         ;
        
        SD.remove("config.ini") ;
        Config = SD.open("config.ini", FILE_WRITE) ;
        Config.print("Perimetre="), Config.println(Perimetre);
        Config.print("Capacite="), Config.println(Capacite);
        Config.print("Distance="), Config.println(Distance);
        Config.print("CapaciteInit="), Config.println(CapRAZ);
        Config.close();
        
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
    AfficherInfo2(Puissance, Capacite, PuissanceConsommee, Temperature) ;    // Appel de la méthode d'affichage de la puisance et de la charge de la batterie

    while (BoutonChoixEcran == 1)                                            // Si le bouton de blocage à été appuyer alors l'affichage reste sur les informations en cours
    {
      if (digitalRead(6) == HIGH)
      {
        Distance = 0.0 ;
        Capacite = CapRAZ ;
      }
      
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
      Temperature = CapteurTemperature.Get_Temperature()             ;     // Enregistrement du retour de la méthode Get_Temperature dans la variable Temperature

 
      PuissanceConsommee = Capacite * Tension                        ;     // Calcul de la puissance consommée
      PuissanceConsommeeKM = PuissanceConsommee / Distance           ;     // Calcul de la puissance consommée par kilomètre

      ChangementEtat = digitalRead(2) ;

      if (ChangementEtat == 1 && ChangementEtat != ValeurPrecedenteDist)
      {
        Distance = Perimetre + Distance ;                               
      }
      ValeurPrecedenteDist = ChangementEtat ;

      unsigned long TempsContinu = millis() ;
      Intervalle = TempsContinu - Temps     ;

      if (Intervalle >= 2000)
      {
        Ah = Intensite * (Intervalle/1000) / 3600                  ;
        Capacite = Capacite - Ah                                       ;
        EnvoyerBluetooth (Tension, Intensite, Puissance, Vitesse, Distance, Capacite) ;   // Appel de la fonction permettant d'envoyer les données via bluetooth

        Rapport = SD.open(NomCSV, FILE_WRITE)            ;
        Rapport.print(Annee), Rapport.print("_"), Rapport.print(Mois), Rapport.print("_"), Rapport.print(Jour), Rapport.print(" "),
        Rapport.print(Heure), Rapport.print(":"), Rapport.print(Minute), Rapport.print(';') ;
        Rapport.print(Tension), Rapport.print(';')              ;
        Rapport.print(Intensite), Rapport.print(';')            ;
        Rapport.print(Puissance), Rapport.print(';')            ;
        Rapport.print(Vitesse), Rapport.print(';')              ;
        Rapport.print(Distance/1000), Rapport.print(';')        ;
        Rapport.print(PuissanceConsommee), Rapport.print(';')   ;
        Rapport.print(PuissanceConsommeeKM), Rapport.print(';') ;
        Rapport.print(Capacite), Rapport.print(';')             ;
        Rapport.print(Altitude), Rapport.print(';')             ;
        Rapport.print(VitesseGPS), Rapport.print(';')           ;
        Rapport.print(Lat,5), Rapport.print(';')                ;
        Rapport.print(Lon,5), Rapport.print(';')                ;
        Rapport.println()                                       ;
        Rapport.close()                                         ;

        SD.remove("config.ini") ;
        Config = SD.open("config.ini", FILE_WRITE) ;
        Config.print("Perimetre="), Config.println(Perimetre);
        Config.print("Capacite="), Config.println(Capacite);
        Config.print("Distance="), Config.println(Distance);
        Config.print("CapaciteInit="), Config.println(CapRAZ);
        Config.close();
        
        Temps = TempsContinu ;
      }

      AfficherInfo2(Puissance, Capacite, PuissanceConsommee, Temperature) ; 
    }
  }

  if (CompteurBoucle > 400)
  {
    CompteurBoucle = 0 ;                                                  // Le CompteurBoucle est remis à zéro une fois que toutes les données ont été affichées
    MonEcran.clear()   ;
  }
  Serial.println(PuissanceConsommeeKM);
  CompteurBoucle++ ;                                                      // Incrémentation du CompteurBoucle
}
