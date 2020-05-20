/*
  ESP8266 test program
  Serial used
  and blink used

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

*/

#define RF_RX_PIN 4
#define RF_TX_PIN 12 // MOSI slave output
#define RF_ENABLE_PIN 5

// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>

// Include dependant SPI Library 
#include <SPI.h>

// Create Amplitude Shift Keying Object
RH_ASK rf_driver(2000, RF_RX_PIN, RF_TX_PIN, RF_ENABLE_PIN); // ESP8266: do not use pin 11

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize ASK Object
  if (!rf_driver.init()){
    Serial.println(">> init failed");
  }

  // prints title with ending line break
  Serial.println(">> Setup finished");
}

// the loop function runs over and over again forever
void loop() {

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

  Serial.println(">> sending started");
  const char *msg = "Hello World";
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
  Serial.println(">> sending finished");
  
  Serial.println("---loop finished---");
  
}
