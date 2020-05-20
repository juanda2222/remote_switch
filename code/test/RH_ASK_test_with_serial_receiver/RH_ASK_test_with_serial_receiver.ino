/*
  ESP8266 test program
  Serial used
  and blink used
*/

// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library 
#include <SPI.h> 

#define RF_RX_PIN 11
#define RF_TX_PIN 4 // MOSI slave output
#define RF_ENABLE_PIN 5

// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000, RF_RX_PIN, RF_TX_PIN, RF_ENABLE_PIN); // ESP8266: do not use pin 11;
 
void setup()
{
    // Setup Serial Monitor
    Serial.begin(9600);
    
    // Initialize ASK Object
    rf_driver.init();
    
}
 
void loop()
{
    // Set buffer to size of expected message
    uint8_t buf[11];
    uint8_t buflen = sizeof(buf);
    // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen))
    {
      
      // Message received with valid checksum
      Serial.print("Message Received: ");
      Serial.println((char*)buf);         
    }
}
