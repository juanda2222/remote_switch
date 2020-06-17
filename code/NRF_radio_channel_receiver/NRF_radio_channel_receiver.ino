#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

 #include <EEPROM.h> //libreria para el manejo de la eprom

 
//Declaremos los pines CE y el CSN
//#define CE_PIN 9 //este venia por defecto
//#define CSN_PIN 10 //este tambien venia por defecto

#define CE_PIN 9
#define CSN_PIN 8
#define BUTTON_ON_PIN 19 // A5 pin also capable of digital operation
#define BUTTON_OFF_PIN 18 // A4 pin also capable of digital operation
#define RELAY_LED_PIN 17 // A3 pin also capable of digital operation
#define LED_PIN 16 // A2 pin also capable of digital operation

#define RELAY_PIN 3 // D3



//EEPROM vars:
int direccionEEPROM; //tiene 1k de direcciones internas
uint8_t saved_state;

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
char datos[4]="";

//Variable con la dirección del canal que se va a leer
uint32_t CLIENT_ADDRESS = 506;
uint32_t SERVER_ADDRESS = 1000000 - CLIENT_ADDRESS; // 255 is the broadcast address 


void setup()
{

  //INITIALIZE EPROM MEMORY ON 1 (ONLY NEEDED ONCE)
  //EEPROM.write(direccionEEPROM, 1);
  
  //inicializamos el puerto serie
  Serial.begin(9600); 
  
  Serial.println(">> Configurtion finished");
  char string_address[12];         //the ASCII of the integer will be stored in this char array
  ultoa(SERVER_ADDRESS, string_address, 10); //(integer, yourBuffer, base)
  Serial.print(">> Address: ");
  Serial.println(SERVER_ADDRESS);
  
  //inicializamos el NRF24L01 
  radio.begin();
  
  //radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(1,10); // delay, count
  
  //Abrimos el canal de Lectura
  radio.openReadingPipe(1, SERVER_ADDRESS);
  
  //empezamos a escuchar por el canal
  radio.startListening();  
  
  //manual configuration of the used pins
  pinMode(BUTTON_ON_PIN, INPUT);
  pinMode(BUTTON_OFF_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // set state based on the state saved on the eprom
  saved_state = EEPROM.read(direccionEEPROM); //inicializo los watts con el valor en la eeprom
  if (saved_state == 0){
    digitalWrite(RELAY_LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);    
  } else {
    digitalWrite(RELAY_LED_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
  }
 
}
 
void loop() {
 uint8_t numero_canal;
 //if ( radio.available(&numero_canal) )

 //check if a button was pressed, action the relay and save the state
 int on_button = digitalRead(BUTTON_ON_PIN);
 int off_button = digitalRead(BUTTON_OFF_PIN);
 if (on_button == HIGH){
    EEPROM.write(direccionEEPROM, 1);
    Serial.println(">> Switch turned ON");
    digitalWrite(RELAY_LED_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    delay(500)
 } else if (off_button == HIGH) {
    EEPROM.write(direccionEEPROM, 0);
    Serial.println(">> Switch turned OFF");
    digitalWrite(RELAY_LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    delay(500)
 }
 
 if ( radio.available(1) )
 {
     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(datos,sizeof(datos));

     //reportamos por el puerto serial los datos recibidos
     Serial.print("Datos= " );
     Serial.println(datos);

     //blink once to indicate a received message
     digitalWrite(LED_PIN, HIGH);
     delay(150);
     digitalWrite(LED_PIN, LOW);
     delay(150);

     if(strcmp((const char *)datos, "on") == 0){
        EEPROM.write(direccionEEPROM, 1);
        Serial.println(">> Switch turned ON");
        digitalWrite(RELAY_LED_PIN, HIGH);
        digitalWrite(RELAY_PIN, HIGH);
        
      }else if(strcmp((const char *)datos, "off") == 0){
        EEPROM.write(direccionEEPROM, 0);
        Serial.println(">> Switch turned OFF");
        digitalWrite(RELAY_LED_PIN, LOW);
        digitalWrite(RELAY_PIN, LOW);
        
      }else{
        Serial.println("-- Message not recognized");
      }
     
 }
 else
 {
     Serial.println("No hay datos de radio disponibles");
 }
 delay(100);
}
