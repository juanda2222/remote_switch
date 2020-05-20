
/*
  ESP8266 transmitter program
  Serial used
  
  #define D0 16 // there is also a LED here. Also used for sleep wake ups
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

  https://lastminuteengineers.com/esp8266-nodemcu-arduino-tutorial/

*/

// nrf24_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_NRF24 driver to control a NRF24 radio.
// It is designed to work with the other example nrf24_reliable_datagram_server
// Tested on Uno with Sparkfun WRL-00691 NRF24L01 module
// Tested on Teensy with Sparkfun WRL-00691 NRF24L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module


//Desconectar el pin de reset para poder programar la tarjeta

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CSN_PIN 4 // D2 used for the nrf24 control (chip select)
#define CE_PIN 0 // D3 (chip enable)
#define MAGNET_PIN 16 //D0 on the nodemcu
#define PIR_PIN 5 //D1 on the nodemcu
//#define SOFT_RESET_PIN 16 //D1 on the nodemcu

//important variables
const uint8_t CLIENT_ADDRESS = 2;
const uint8_t SERVER_ADDRESS = 255 - CLIENT_ADDRESS; // 255 is the broadcast address 
const unsigned long interval_milis = 6000;           // interval at which the sistem will turn off the energy

uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; // Dont put this on the stack (used receive the response message)
int pir_state = 0; // save the pir status
int magnet_state = 0; // save the magnet status

int state = 1;
unsigned long current_millis = 0;
unsigned long previous_millis = 0;

// The states are defined as:
// s0 = off
// s1 = on, pending closed door
// s2 = on, checking pir state (and timer)
// s3 = on 

uint8_t message_on[] = "on";
uint8_t message_off[] = "off";


// Singleton instance of the radio driver
RH_NRF24 driver(CE_PIN, CSN_PIN);;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{

  //manual configuration of the used pins
  pinMode(MAGNET_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   
  Serial.println(">> Configurtion finished");

  //configure the comunication:
  Serial.begin(9600);
  while (!Serial) {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }
  if (!manager.init())
    Serial.println("init failed");
  if (!driver.setChannel(3))
    Serial.println("setChannel failed");
  if (!driver.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::RFM73TransmitPower5dBm))
    Serial.println("setRF failed");  
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm

  //turn on the switch:
  //while (!set_on(true)){} //send until received
  set_on(true);//send once
}

void loop()
{
  
  // blink twice and start the sending
  digitalWrite(LED_BUILTIN, LOW);   
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(200);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(500);


  // measure and print the state of the sensors
  pir_state = digitalRead(PIR_PIN);
  magnet_state = digitalRead(MAGNET_PIN);
  Serial.print(">> sensed pir: ");
  Serial.println(pir_state);
  Serial.print(">> sensed magnet: ");
  Serial.println(magnet_state);

  switch (state) {
    case 0:
      //while (!set_on(false)){}//loop indefinitevly until the message is sended
      set_on(false); //just send once
      Serial.println(">> Going to sleep mode and leaving the switch OFF");
      ESP.deepSleep(0, WAKE_RF_DEFAULT); // the argument time is in us (0 means forever)
      break;
    case 1:
      // waiting for a closed door
      if(magnet_state == HIGH){
        state = 2;
        previous_millis = millis();
      }
      break;
    case 2:
      // waiting for movement or 15 seconds without it
      current_millis = millis();
      if(pir_state == HIGH){
        // movement has been detected
        state = 3;
      }else if (current_millis - previous_millis>= interval_milis) {
        // the time has passed while on this state
        state = 0;
      }else{
        Serial.print(">> Waiting time or movement. current time: ");
        Serial.println(current_millis - previous_millis);
      }
      break;
    case 3:
      //while (!set_on(true)){}//loop indefinitevly until the message is sended
      set_on(true); //just send once
      Serial.println(">> Going to sleep mode and leaving the switch ON");
      //ESP.deepSleep(1e6, WAKE_RF_DEFAULT); // the argument time is in us (0 means forever)
      ESP.deepSleep(0, WAKE_RF_DEFAULT); // the argument time is in us (0 means forever)
      break;
    default:
      Serial.println("-- State not recognized");
      break;
  }

  Serial.print(">> Current state is: S");
  Serial.println(state);
}

//function used to send the on command of the off command to the server (remote switch)
bool set_on(bool set_on){

  // set up internal important variables
  bool message_sended = false; 
  bool result;
  if (set_on) {
    result = manager.sendtoWait(message_on, sizeof(message_on), SERVER_ADDRESS);
  } else{
    result = manager.sendtoWait(message_off, sizeof(message_on), SERVER_ADDRESS);
  }
  
  // Send a message to manager_server
  Serial.println(">> Sending to nrf24_reliable_datagram_server");
  if (result){

    message_sended = true;
    
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from)){
      Serial.print("got reply from : DEC:");
      Serial.print(from, DEC);
      Serial.print(": ");
      Serial.println((char*)buf);

      message_sended = true;
      
    }else{
      Serial.println("--- No reply, is nrf24_reliable_datagram_server running?");
    }
  }else{
    Serial.println("--- SendtoWait failed");
  }
  
  return message_sended;
}
