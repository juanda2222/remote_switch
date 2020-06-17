#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Declaremos los pines CE y el CSN
#define CSN_PIN 4 // D2 used for the nrf24 control (chip select)
#define CE_PIN 0 // D3 (chip enable)

//Variable con la dirección del canal por donde se va a transmitir
uint64_t CLIENT_ADDRESS = 507;
uint64_t SERVER_ADDRESS = 1000000 - CLIENT_ADDRESS; // 255 is the broadcast address 
//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector con los datos a enviar
float datos[3];

void setup()
{
  //inicializamos el NRF24L01 
  radio.begin();
  //inicializamos el puerto serie
  Serial.begin(9600); 
 
  radio.begin();
  //radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(1,10); // delay, count

  //Abrimos un canal de escritura
  radio.openWritingPipe(SERVER_ADDRESS);


  //stop the readings
  radio.stopListening();
 
}
 
void loop()
{ 
 //cargamos los datos en la variable datos[]
 datos[0]=analogRead(0)* (5.0 / 1023.0);;
 datos[1]=millis();
 datos[2]=3.14;
 //enviamos los datos
 bool ok = radio.write(datos, sizeof(datos));
  //reportamos por el puerto serial los datos enviados 
  if(ok)
  {
     Serial.print("Datos enviados: "); 
     Serial.print(datos[0]); 
     Serial.print(" , "); 
     Serial.print(datos[1]); 
     Serial.print(" , "); 
     Serial.println(datos[2]); 
  }
  else
  {
     Serial.println("no se ha podido enviar");
  }
  delay(1000);
}
