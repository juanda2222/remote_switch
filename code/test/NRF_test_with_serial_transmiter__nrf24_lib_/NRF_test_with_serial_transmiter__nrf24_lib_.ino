
/*
  ESP8266 transmitter program
  Serial used
  
  #define D0 16
  #define D1 5 // I2C Bus SCL (clock)
  #define D2 4 // I2C Bus SDA (data)
  #define D3 0
  #define D4 2 // Same as "LED_BUILTIN", but inverted logic
  #define D5 14 // SPI Bus SCK (clock)
  #define D6 12 // SPI Bus MISO 
  #define D7 13 // SPI Bus MOSI
  #define D8 15 // SPI Bus SS (CS)
  #define D9 3 // RX0 (Serial console)
  #define D10 1 // TX0 (Serial console)
  #define SD3 12 //SDD3 

  

*/

#define CSN_pin 4 // D2 used for the nrf24 control (chip select)
#define CE_pin 0 // D3 (chip enable)

#define magnet_pin 16 //D0 on the nodemcu
#define pir_pin 5 //D1 on the nodemcu

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(CE_pin, CSN_pin);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
int pir_state = 0; // save the pir status
int magnet_state = 0; // save the magnet status

void setup()
{
  //initialize the comunication:
  Serial.begin(9600);
  while (!Serial) {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }
  radio.begin();
  
  //manual configuration of the used pins
  pinMode(magnet_pin, INPUT);
  pinMode(pir_pin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  //configure the nrf24 
  radio.openWritingPipe(address); //set the address
  radio.stopListening(); //Set module as transmitter
}
void loop()
{
  Serial.println("---loop started---");
  
  // blink twice and start the sending
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(200);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(200);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);


  //Send message to receiver
  Serial.println(">> sending started");

  // measure and print the state of the sensors
  pir_state = digitalRead(pir_pin);
  magnet_state = digitalRead(magnet_pin);
  Serial.print(">> sensed pir: ");
  Serial.println(pir_state);
  Serial.print(">> sensed magnet: ");
  Serial.println(magnet_state);
  
  // send the radio data
  const char text[] = "1";
  radio.write(&text, sizeof(text));
  Serial.println(">> sending finished");


  //Repeat on a separated interval
  Serial.println("---loop finished---");
  delay(50);
}
