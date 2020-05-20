#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
//Declaremos los pines CE y el CSN
//#define CE_PIN 9 //este venia por defecto
//#define CSN_PIN 10 //este tambien venia por defecto

#define CE_PIN 9
#define CSN_PIN 8
#define BUTTON1_PIN 19 // A5 pin also capable of digital operation
#define BUTTON2_PIN 18 // A4 pin also capable of digital operation
#define RELAY_LED_PIN 17 // A3 pin also capable of digital operation
#define LED_PIN 16 // A2 pin also capable of digital operation

#define RELAY_PIN 3 // D3



//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
char datos[4]="";

//Variable con la dirección del canal que se va a leer
uint64_t CLIENT_ADDRESS = 1;
uint64_t SERVER_ADDRESS = 1000000 - CLIENT_ADDRESS; // 255 is the broadcast address 


void setup()
{
  //inicializamos el puerto serie
  Serial.begin(9600); 

 //inicializamos el NRF24L01 
  radio.begin();

  //Abrimos el canal de Lectura
  radio.openReadingPipe(1, SERVER_ADDRESS);
  
  radio.setPALevel(RF24_PA_MIN);
    
  //empezamos a escuchar por el canal
  radio.startListening();  
  
  //manual configuration of the used pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  digitalWrite(RELAY_LED_PIN, HIGH);
  digitalWrite(RELAY_PIN, HIGH);
}
 
void loop() {
 uint8_t numero_canal;
 //if ( radio.available(&numero_canal) )
 if ( radio.available() )
 {
     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(datos,sizeof(datos));

     //reportamos por el puerto serial los datos recibidos
     Serial.print("Datos= " );
     Serial.println(datos);

     //blink once to indicate a received message
     digitalWrite(LED_PIN, HIGH);
     delay(100);
     digitalWrite(LED_PIN, LOW);

     if(strcmp((const char *)datos, "on") == 0){
        Serial.println(">> Switch turned ON");
        digitalWrite(RELAY_LED_PIN, HIGH);
        digitalWrite(RELAY_PIN, HIGH);
        
      }else if(strcmp((const char *)datos, "off") == 0){
        Serial.println(">> Switch turned OFF");
        digitalWrite(RELAY_LED_PIN, LOW);
        digitalWrite(RELAY_PIN, LOW);
        
      }else{
        Serial.println("-- Message not recognized");
      }
     
 }
 else
 {
     Serial.println("No hay datos de radio disponibles");
 }
 delay(1000);
}
