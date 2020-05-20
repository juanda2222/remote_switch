// nrf24_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_NRF24 driver to control a NRF24 radio.
// It is designed to work with the other example nrf24_reliable_datagram_server
// Tested on Uno with Sparkfun WRL-00691 NRF24L01 module
// Tested on Teensy with Sparkfun WRL-00691 NRF24L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CSN_PIN 4 // D2 used for the nrf24 control (chip select)
#define CE_PIN 0 // D3 (chip enable)
#define MAGNET_PIN 16 //D0 on the nodemcu
#define PIR_PIN 5 //D1 on the nodemcu

//important variables
const uint8_t CLIENT_ADDRESS = 1;
const uint64_t SERVER_ADDRESS = 255 - CLIENT_ADDRESS; // 255 is the broadcast address 
uint8_t data[] = "Hello World!";
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; // Dont put this on the stack:
int pir_state = 0; // save the pir status
int magnet_state = 0; // save the magnet status


// Singleton instance of the radio driver
RH_NRF24 driver(CE_PIN, CSN_PIN);;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  while (!Serial) {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }
  if (!manager.init())
    Serial.println("init failed");
  if (!driver.setChannel(1))
    Serial.println("setChannel failed");
  if (!driver.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::RFM73TransmitPower5dBm))
    Serial.println("setRF failed");  
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm

  //manual configuration of the used pins
  pinMode(MAGNET_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println(">> Configurtion finished");
}

void loop()
{
  
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
  delay(1500);

  // measure and print the state of the sensors
  pir_state = digitalRead(PIR_PIN);
  magnet_state = digitalRead(MAGNET_PIN);
  Serial.print(">> sensed pir: ");
  Serial.println(pir_state);
  Serial.print(">> sensed magnet: ");
  Serial.println(magnet_state);

  
  // Send a message to manager_server
  Serial.println(">> Sending to nrf24_reliable_datagram_server");
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from)){
      Serial.print("got reply from : DEC:");
      Serial.print(from, DEC);
      Serial.print(": ");
      Serial.println((char*)buf);
    }else{
      Serial.println("--- No reply, is nrf24_reliable_datagram_server running?");
    }
  }else{
    Serial.println("--- SendtoWait failed");
  }
  delay(50);
}
