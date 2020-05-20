/*
  Arduino nano receiver program
*/

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_pin 9
#define CSN_pin 8

#define led_pin 28 // A5 pin also capable of digital operation

//create an RF24 object
RF24 radio(CE_pin, CSN_pin);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";

void setup()
{
  Serial.begin(9600);
  while (!Serial){
    continue;
  }
  radio.begin();
  Serial.println(">> Comunication initialized");
  
  //manual configuration of the used pins
  pinMode(led_pin, OUTPUT);
  

  // configure the radio module
  radio.openReadingPipe(0, address); //set the address
  radio.startListening(); //Set module as receiver
  Serial.println(">> Configuration finished");
}

void loop()
{
  /*
  // blink twice and start the sending
  digitalWrite(led_pin, LOW);   
  delay(200);
  digitalWrite(led_pin, HIGH);  
  delay(200);
  digitalWrite(led_pin, LOW); 
  delay(200);
  digitalWrite(led_pin, HIGH);  
  delay(200);
  digitalWrite(led_pin, LOW); 
  delay(500);
  */
  
  //Read the data if available in buffer
  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
