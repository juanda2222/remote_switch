// nrf24_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_NRF24 driver to control a NRF24 radio.
// It is designed to work with the other example nrf24_reliable_datagram_client
// Tested on Uno with Sparkfun WRL-00691 NRF24L01 module
// Tested on Teensy with Sparkfun WRL-00691 NRF24L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CE_PIN 9
#define CSN_PIN 8
#define LED_PIN 19 // A5 pin also capable of digital operation

//important variables
const uint8_t CLIENT_ADDRESS = 1;
const uint64_t SERVER_ADDRESS = 255 - CLIENT_ADDRESS; // 255 is the broadcast address 

// Singleton instance of the radio driver
RH_NRF24 driver(CE_PIN, CSN_PIN);
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  if (!driver.setChannel(1))
    Serial.println("setChannel failed");
  if (!driver.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::RFM73TransmitPower5dBm))
    Serial.println("setRF failed");    

  //manual configuration of the used pins
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println(">> Configuration finished");
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void loop()
{
  /*
  Serial.println("---loop started---");
  // blink twice and start the sending
  digitalWrite(LED_PIN, LOW);   
  delay(200);
  digitalWrite(LED_PIN, HIGH);  
  delay(200);
  digitalWrite(LED_PIN, LOW); 
  delay(200);
  digitalWrite(LED_PIN, HIGH);  
  delay(200);
  digitalWrite(LED_PIN, LOW); 
  delay(500);
  */
  
  if (manager.available()){
    
    Serial.println(">> message available");
    digitalWrite(LED_PIN, HIGH);
    delay(20);
    digitalWrite(LED_PIN, LOW);
    
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      digitalWrite(LED_PIN, HIGH);
      
      Serial.print("got request from : DEC");
      Serial.print(from, DEC);
      Serial.print(": ");
      Serial.println((char*)buf);

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from)){
        Serial.println("--- sendtoWait failed");
      } else {
        Serial.println(">> Response message sended");
      }
      
      digitalWrite(LED_PIN, LOW);
      
    }else{
      Serial.println("trying to read the response mesage but not found");
      
    }
  }
}
