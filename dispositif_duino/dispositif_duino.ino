/*********************************************************************************
 *                    PARTIE 1 : Intégrer des bibliothèques                      *
 *********************************************************************************/
#include "messagerieTB.h"
#include <Adafruit_NeoPixel.h> //librairie pour les led
#include <Servo.h>

/*********************************************************************************
 *                    PARTIE 2 : Définir des valeurs, des objets, des variables  *
 *********************************************************************************/
//pour debuguer le programme, mettre à true
#define DEBUG true
//à décommenter pour utiliser le bandeau de LEDS
#define UTILISE_LEDS true

//CONFIGURATION DES PORTS UTILISES SUR L'ARDUINO UNO
/**
 * POUR INFO en utilisant le motor shield v2:
 * la masse (GND) ainsi que le 5v (par defaut) ou le 3.3v sont nécessaire pour faire fonctionner le shield. (le 5v ou le 3v peuvent être choisis par un jumper sur la carte)
 * Le shield utilise le signal SDA et SCL pour le protocole i2c utilisé pour contrôler les moteurs. Sur l'arduino UNO ce sont les pinoches analogiques A4 et A5.
 * Sur l'arduino Mega ces pinoches sont plutôt les pinoches numériques 20 et 21.
 * Il ne faut donc pas utiliser ces pinoches sur ces arduinos avec ce shield pour quoique ce soit d'autre que des capteurs ou actionneurs i2c.
 *
 * Etant donné que le shield utilise l'i2c pour communiquer, vous pouvez connecter d'autres capteurs ou actionneurs i2c sur les signaux SDA et SCL tant qu'ils n'utilisent pas l'adresse 0x60
 * (l'adresse par défaut du shield) ou l'adresse 0x070 (l'adresse utilisable par le shield pour contrôler un groupe de shield)
 *
 * Si vous voulez utiliser les broches du shield déddiées aux servos, elles sont reliées aux pinoches 9 et 10.
 * Si vous n'utilisez pas ces broches, les pinoches 9 et 10 sont libres d'utilisation.
 * Vous pouvez utiliser toutes les autres pinoches non mentionnées ci-dessus.
 */
#define PIN_CONTACTEUR_01     A0 //pour déclencher le dispositif
#define PIN_CONTACTEUR_02     A1 //pour arreter la cremaillere
#define PIN_A_A2              A2 //pas encore utilisé
#define PIN_A_A3              A3 //pas encore utilisé
#define PIN_A_A4              A4 //pas encore utilisé
#define PIN_A_A5              A5 //pas encore utilisé
//#define PIN_00              0 //utilisé en cas de débugage
//#define PIN_01              1 //utilisé en cas de débugage
//#define PIN_02              2 //pas encore utilisé mais peut perturber le chrono pour la fin de match
#define PIN_LEDS            3 //PWM - utilisé pour les leds
#define PIN_SERVO_02        4 //PWM - servo cremaillere
#define PIN_SERVO_03        5 //PWM - servo qui libere le contrepoids de la glissiere
#define PIN_SERVO_04        6 //PWM - pas encore utilisé
#define PIN_SERVO_05        7 //PWM - pas encore utilisé
#define PIN_SERVO_06        8 //PWM - pas encore utilisé
#define PIN_SERVO_07        9 //PWM - pas encore utilisé
#define PIN_SERVO_08        10 //PWM - pas encore utilisé
#define PIN_SERVO_09        11 //PWM - pas encore utilisé
#define PIN_SERVO_10        12 //PWM - pas encore utilisé
#define PIN_SERVO_11        13 //PWM - pas encore utilisé


//parametrage du match
#define DUREE_MATCH               100000 //en millisecondes
#define EQUIPE_JAUNE              0
#define EQUIPE_BLEUE              1

//parametrage divers
#define NBRE_DE_LEDS              11 //nombre de leds
#define LEDS_BLEU                 1 //couleur bleue pour les leds
#define LEDS_ORANGE               2 //couleur orange pour les leds

//valeurs des servo_moteurs
#define CREMAILLERE_STOP    96
#define CREMAILLERE_BOUGE   5
#define CONTREPOIDS_BLOQUE  90
#define CONTREPOIDS_LIBERE  5

//----------------------------------------------------------------------------------------------
// DECLARATION VARIABLES: MOTEUR, CAMERA, LEDS,...
//----------------------------------------------------------------------------------------------

//Création variables globales
bool bProgrammeDemarre; // variable qui indique si le programme est demarre (demarre==true), utilisé par la tirette
bool phare_leve;

//Création des servos
Servo servo_cremaillere;
Servo servo_contrepoids;

//création des leds
#ifdef UTILISE_LEDS
  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NBRE_DE_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
#endif

/*********************************************************************************
 *                    PARTIE 3 : Définition des fnctions                         *
 *********************************************************************************/

#ifdef UTILISE_LEDS
  /*
   * FONCTION METTRE_LEDS_A
   * mets toutes les leds à une certaine couleur
   */
void LED_PHARE(int latence,bool ar)
{
  int i,j;
  for(i=0;i<NBRE_DE_LEDS-1;i++)
  {
  
    for(j=0;j<NBRE_DE_LEDS;j++)
    {
      pixels.setPixelColor(j, pixels.Color(0,0,0));
    }
    
    /*if (DEBUG)
    {
      Serial.print("On allume led ");
      Serial.print(i);
      Serial.print(" et ");
      Serial.println(i+1);
    }*/
    
    pixels.setPixelColor(i, pixels.Color(255,50,0));
    pixels.setPixelColor(i+1, pixels.Color(255,50,0)); 
    
    pixels.show(); //ça applique la couleur programmée
    delay(latence); //on attend 500 millisecondes
  }

  if(ar)
  {
    for(i=NBRE_DE_LEDS;i>0;i--)
    {
    
      for(j=0;j<NBRE_DE_LEDS;j++)
      {
        pixels.setPixelColor(j, pixels.Color(0,0,0));
      }
      
      pixels.setPixelColor(i, pixels.Color(255,50,0));
      pixels.setPixelColor(i-1, pixels.Color(255,50,0)); 
      
      pixels.show(); //ça applique la couleur programmée
      delay(latence); //on attend 500 millisecondes
    }
  }
}
  
#endif





/*********************************************************************************
 *                    PARTIE 4 : Fonction setup() de l'arduino                   *
 *********************************************************************************/

void setup() 
{
  if(DEBUG)
  {
    Serial.begin(9600);
    Serial.println("Debug activé\n\n");
  }

#ifdef UTILISE_LEDS
  //initialisation du bandeau de leds
  pixels.begin(); //Pour utiliser les leds
  for(int j=0;j<NBRE_DE_LEDS;j++)
  {
    pixels.setPixelColor(j, pixels.Color(0,0,0));
  }
  pixels.show(); //ça applique la couleur programmée
#endif

    //initialisation du début de programme     
    bProgrammeDemarre=false; // le programme n'est pas demarre quand l'arduino s'allume
  
  pinMode(LED_BUILTIN, OUTPUT); // initialisation de la led interne de l'arduino
  digitalWrite(LED_BUILTIN, LOW);    // led eteinte
  
  pinMode(PIN_CONTACTEUR_01,INPUT_PULLUP); // contacteur pour allumer le dispositif
  pinMode(PIN_CONTACTEUR_02,INPUT_PULLUP); // contacteur pour arreter la cremaillere
  
  //Initialisation des servos
  servo_cremaillere.attach(PIN_SERVO_02);
  servo_cremaillere.write(CREMAILLERE_STOP);

  servo_contrepoids.attach(PIN_SERVO_03);
  servo_contrepoids.write(CONTREPOIDS_BLOQUE);

  phare_leve=false; 
}

/*********************************************************************************
 *                    PARTIE 4 : Fonction loo() de l'arduino                     *
 *********************************************************************************/
//boucle qui est répétée indéfiniment
void loop()
{
  
  int contacteur_01 =analogRead(PIN_CONTACTEUR_01);
  //---------------------------------------------
  //LANCEMENT PROGRAMME
  //---------------------------------------------
  if ((contacteur_01<800)&&(bProgrammeDemarre==false))
  {
    bProgrammeDemarre=true; // le programme est demarre
    if (DEBUG)
      Serial.println("début programme");
    while(1)
    {
      if (DEBUG)
        Serial.println("séquence phare");
        
      if(!phare_leve)
      {
        servo_contrepoids.write(CONTREPOIDS_LIBERE);
        delay(4000);
        while(analogRead(PIN_CONTACTEUR_02)>800)
        {
          servo_cremaillere.write(CREMAILLERE_BOUGE);
          delay(200);
        }
        servo_cremaillere.write(CREMAILLERE_STOP);
        phare_leve=true;
      }
      
      //on allume le phare
      LED_PHARE(75,true);
    }
  }


}
