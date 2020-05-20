#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
//Declaremos los pines CE y el CSN
//#define CE_PIN 9 //este venia por defecto
//#define CSN_PIN 10 //este tambien venia por defecto

#define CE_PIN 9
#define CSN_PIN 8
#define LED_PIN 19 // A5 pin also capable of digital operation
 
//Variable con la dirección del canal que se va a leer
const byte direccion[5] ={'c','a','n','a','l'}; 

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
float datos[3];
bool blink_now = false;

void setup()
{
  //inicializamos el puerto serie
  Serial.begin(9600); 

 //inicializamos el NRF24L01 
  radio.begin();

  //Abrimos el canal de Lectura
  radio.openReadingPipe(1, direccion);
  
  radio.setPALevel(RF24_PA_MAX);
    
  //empezamos a escuchar por el canal
  radio.startListening();  
  
  //manual configuration of the used pins
  pinMode(LED_PIN, OUTPUT);
 
}
 
void loop() {
 uint8_t numero_canal;
 //if ( radio.available(&numero_canal) )
 if ( radio.available() )
 {
     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(datos,sizeof(datos));

     blink_now = true;
     
     //reportamos por el puerto serial los datos recibidos
     Serial.print("Dato0= " );
     Serial.print(datos[0]);
     Serial.print(" V, ");
     Serial.print("Dato1= " );
     Serial.print(datos[1]);
     Serial.print(" ms, ");
     Serial.print("Dato2= " );
     Serial.println(datos[2]);

     //blink once to indicate a received message
     digitalWrite(LED_PIN, HIGH);
     delay(50);
     // turn off the led
     digitalWrite(LED_PIN, LOW);
     
 }
 else
 {
     Serial.println("No hay datos de radio disponibles");
 }
 delay(300);
}
