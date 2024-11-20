/*
    CÓDIGO DE REFERENCIA - SABO/AULA
*/
#include <SPI.h>
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

#define CE_PIN 7
#define CSN_PIN 8

#define TIMEOUTACK 2000 //us
#define TIMEOUTSEND 6000 //us

#define ACK 1
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

char payloadT[5]="Hello";
char payloadR[5];
uint8_t origem=11;


bool enviaTrem(char* pacote, uint8_t tamanho, uint8_t destino){
  pacote[0]=destino;
  pacote[1]=origem;
  char pacoteR[3];
  bool enviou=false;
  unsigned long start_timer = micros();                // start the timer
  while(micros()-start_timer<TIMEOUTSEND){
    radio.startListening();
    delayMicroseconds(70);
    radio.stopListening();
    if (!radio.testCarrier()) {
      radio.write(&pacote[0], tamanho);
      enviou=true;
      Serial.println("E");
      break;
    }else{
      Serial.println("O");
      delayMicroseconds(150);
    }
  }
  if(!enviou){
    return false;
  }
  unsigned long start_timer_ack = micros();  
  radio.startListening();
  while(micros()-start_timer_ack<TIMEOUTACK){
    if (radio.available()) { 
      radio.read(&pacoteR[0],3);
      if(pacoteR[0]==origem && pacoteR[1]==destino && pacoteR[2] == ACK){
        unsigned long end_timer = micros();                  // end the timer
        Serial.print(end_timer - start_timer);  // print the timer result

        return true;
      }
      radio.flush_rx();
    }
  }
  return false;
}

bool recebeTrem(char* pacote, uint8_t tamanho, int TIMEOUT){
  unsigned long start_timer = micros();                // start the timer
  bool recebeu = false;
  bool enviou = false;
  radio.startListening();
  while(micros()-start_timer<TIMEOUT){
    if (radio.available()) { 
      radio.read(&pacote[0],tamanho);
      if(pacote[0]==origem){
        recebeu=true;
        Serial.println("R");
        break;
      }
      radio.flush_rx();
    }
  }
  radio.flush_rx();
  if(!recebeu){
    return false;
  }
  char pacoteACK[3];
  pacoteACK[0]=pacote[1];
  pacoteACK[1]=origem;
  pacoteACK[2]=ACK;
  start_timer = micros();                // start the timer
  
  while(micros()-start_timer<TIMEOUTACK){
    radio.startListening();
    delayMicroseconds(70);
    radio.stopListening();
    if (!radio.testCarrier()) {
      radio.write(&pacoteACK[0], 3);
      Serial.print(int(pacoteACK[0]));
      Serial.print(int(pacoteACK[1]));
      Serial.println(int(pacoteACK[2]));
      enviou=true;
      Serial.println("E");
      break;
    }else{
      delayMicroseconds(100);
    }
  }
  if(!enviou){
    return false;
  }else{
    return true;
  }
}


void setup(void) {

  Serial.begin(500000);

  // Setup and configure rf radio
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
    while (true) {
      // hold in an infinite loop
    }
  }
  
  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setAutoAck(false);   // Don't acknowledge arbitrary signals
  radio.disableCRC();        // Accept any signal we find
  radio.setDataRate(RF24_1MBPS);
  
  
  radio.setPayloadSize(sizeof(payloadT));

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  radio.setChannel(100);
  
  printf_begin();
  radio.printPrettyDetails();
  
  radio.startListening();
  radio.stopListening();
  radio.flush_rx();


}

void loop(void) {
  bool sucesso = recebeTrem(&payloadR[0],5,1000000);
  if(sucesso){
    printAula(&payloadR[0],5);
  }
 
}

void printAula(char *texto, byte tamanho){
  Serial.print(int(texto[0]));
  Serial.print(int(texto[1]));
  for(byte i=2;i<tamanho;i++){
    Serial.print(char(texto[i]));
  }
  Serial.println();

}

