#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Declaremos los pines CE y el CSN
#define CSN_PIN 4 // D2 used for the nrf24 control (chip select)
#define CE_PIN 0 // D3 (chip enable)

#define CSN_PIN 4 // D2 used for the nrf24 control (chip select)
#define CE_PIN 0 // D3 (chip enable)
#define MAGNET_PIN 16 //D0 on the nodemcu
#define PIR_PIN 5 //D1 on the nodemcu
//#define SOFT_RESET_PIN 16 //D1 on the nodemcu

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//consts and important vars
//consts:
const uint8_t message_on[] = "on";
const uint8_t message_off[] = "off";
const unsigned long interval_milis = 6000; // interval at which the sistem will turn off the energy
//vars:
int state = 4;
int pir_state = 0; // save the pir status
int magnet_state = 0; // save the magnet status
unsigned long current_millis = 0;
unsigned long previous_millis = 0;

uint32_t CLIENT_ADDRESS = 506;
uint32_t SERVER_ADDRESS = 1000000 - CLIENT_ADDRESS; // 255 is the broadcast address 


// The states are defined as:
// s0 = off
// s1 = on, pending closed door
// s2 = on, checking pir state (and timer)
// s3 = on 
// s4 = on/off IDLE


void setup()
{
  //inicializamos el puerto serie
  Serial.begin(9600); 

  //manual configuration of the used pins
  pinMode(MAGNET_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, LOW);   
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   
  
  Serial.println(">> Configurtion finished");
  char string_address[12];         //the ASCII of the integer will be stored in this char array
  ultoa(SERVER_ADDRESS, string_address, 10); //(integer, yourBuffer, base)
  Serial.print(">> Address: ");
  Serial.println(string_address);
  
  //inicializamos el NRF24L01 
  radio.begin();

  //radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(10,10); // delay, count

  //Abrimos un canal de escritura
  radio.openWritingPipe(SERVER_ADDRESS);

  //stop the readings
  radio.stopListening();

  //turn on the switch:
  while (!set_on(true)){} //send until received
  //set_on(true);//send once
 
}
 
void loop()
{ 

  // blink twice and start the sending
  digitalWrite(LED_BUILTIN, LOW);   
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(50);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(100);


  // measure and print the state of the sensors
  pir_state = digitalRead(PIR_PIN);
  magnet_state = digitalRead(MAGNET_PIN);
  Serial.print(">> sensed pir: ");
  Serial.println(pir_state);
  Serial.print(">> sensed magnet: ");
  Serial.println(magnet_state);

  switch (state) {
    case 0:
      while (!set_on(false)){}//loop indefinitevly until the message is sended
      //set_on(false); //just send once
      state = 4; // go to iddle
      Serial.println(">> Going to sleep mode and leaving the switch OFF");
      //ESP.deepSleep(0, WAKE_RF_DEFAULT); // the argument time is in us (0 means forever)
      break;
    case 1:
      // waiting for a closed door
      if(magnet_state == HIGH){
        delay(3000);//this delay os for the pir time
        //if after the wait the magnet is still on high change state
        if(magnet_state == HIGH){
          state = 2;
          previous_millis = millis();
        }
      }
      break;
    case 2:
      // waiting for movement or 15 seconds without it
      current_millis = millis();
      if(pir_state == HIGH){
        // movement has been detected

        //blink for one second
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW); 
        delay(1000);
        state = 3;
      }else if (current_millis - previous_millis>= interval_milis) {
        // the time has passed while on this state
        
        //blink for two second
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(2000);
        digitalWrite(LED_BUILTIN, LOW); 
        delay(2000);
        state = 0;

      // or the door was opened again:
      }else if (magnet_state  == LOW){
        //set_on(true); //just send once
        state = 1;  
      }
      else{
        Serial.print(">> Waiting time or movement. current time: ");
        Serial.println(current_millis - previous_millis);
      }
      break;
    case 3:
      while (!set_on(true)){}//loop indefinitevly until the message is sended
      //set_on(true); //just send once
      Serial.println(">> Going to sleep mode and leaving the switch ON");
      state = 4; // go to iddle
      //ESP.deepSleep(1e6, WAKE_RF_DEFAULT); // the argument time is in us (0 means forever)
      //ESP.deepSleep(0, WAKE_RF_DEFAULT); // the argument time is in us (0 means forever)
      break;
    case 4:
      // if the door is opened
      if (magnet_state  == LOW){
        while (!set_on(true)){}//loop indefinitevly until the message is sended
        //set_on(true); //just send once
        state = 1;  
      }
      Serial.println(">> waiting an opened door... ");
      break;
    default:
      Serial.println("-- State not recognized");
      break;
  }

  Serial.print(">> Current state is: S");
  Serial.println(state);
  //delay(1000);
}

bool set_on(bool set_on){

  // set up internal important variables
  bool message_sended = false; 
  
  if (set_on) {
    message_sended = radio.write(message_on, sizeof(message_on));
  } else{
    message_sended = radio.write(message_off, sizeof(message_off));
  }

  if (message_sended){
    Serial.println(">> Message sended");
  }else{
    Serial.println(">> Error sending message");
  }
  // Send a message to manager_server
  delay(1200);
  return message_sended;
}
