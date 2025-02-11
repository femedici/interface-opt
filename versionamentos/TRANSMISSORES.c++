/*
   RECEPTOR !!!!!!!!!!!!!!!!!!
*/

#include <SPI.h>
#include <RF24.h>

// Pinos de conexão do NRF24
#define CE_PIN   7
#define CSN_PIN 8

// Criando o objeto RF24
RF24 radio(CE_PIN, CSN_PIN);

// Endereços de comunicação
uint64_t address = 0x3030303030LL;

void setup() {
  // Iniciar comunicação serial
  Serial.begin(115200);
  
  // Inicializar o módulo NRF24
  radio.begin();
  radio.setChannel(110);
  radio.openReadingPipe(1,address);  // Configura o endereço para o receptor
  radio.setPALevel(RF24_PA_HIGH);      // Configura a potência de recepção
  radio.setDataRate(RF24_250KBPS);     // Configura a taxa de dados
  radio.startListening();              // Começa a ouvir (modo receptor)
  
  Serial.println("Receptor pronto!");
}

// Variáveis para armazenar os dados recebidos
byte origemRecebida;
float temperaturaRecebida, umidadeRecebida;

void loop() {
  // Verifica se há dados disponíveis para leitura
  if (radio.available()) {
    byte payloadRx[9];  // Tamanho do payload (origem + temperatura + umidade)

    // Lê os dados do transmissor
    radio.read(&payloadRx, sizeof(payloadRx));

    // Processa os dados recebidos
    origemRecebida = payloadRx[0];
    memcpy(&temperaturaRecebida, &payloadRx[1], sizeof(float));
    memcpy(&umidadeRecebida, &payloadRx[5], sizeof(float));

    // Exibe os dados recebidos no monitor serial
    Serial.print("Dados recebidos:");
    Serial.print(" Origem: ");
    Serial.print(origemRecebida);
    Serial.print(" | Temp: ");
    Serial.print(temperaturaRecebida);
    Serial.print("°C | Umidade: ");
    Serial.print(umidadeRecebida);
    Serial.println("%");
  }

  delay(500);  // Pequeno delay para não sobrecarregar o loop
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 
  TRANSMISSORES
*/


#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include "DHT.h"

#define CE_PIN 7
#define CSN_PIN 8
#define DHT_PIN 6
#define DHT_TYPE DHT22

RF24 radio(CE_PIN, CSN_PIN);
DHT dht(DHT_PIN, DHT_TYPE);

// Endereço único para a comunicação
uint64_t address = 0x3030303030LL;

byte origem = 9; // Deve ser igual ao origemEsperada no receptor
float temperatura, umidade;

void setup() {
  // Iniciar comunicação serial
  Serial.begin(115200);
  
  // Inicializar o módulo NRF24
  dht.begin();
  Serial.println(F("Modo de transmissão iniciado."));
  radio.begin();
  radio.openWritingPipe(address);  // Configura o endereço para o transmissor
  radio.setChannel(110);
  radio.setPALevel(RF24_PA_HIGH);   // Configura a potência de transmissão
  radio.setDataRate(RF24_250KBPS);  // Configura a taxa de dados
  radio.stopListening();            // Impede o módulo de ouvir (modo transmissor)

  printf_begin();
  Serial.println("Transmissor pronto!");
}

void loop() {
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha ao ler do sensor DHT!");
    return;
  }

  byte payloadTx[9];
  payloadTx[0] = origem;
  memcpy(&payloadTx[1], &temperatura, sizeof(float));
  memcpy(&payloadTx[5], &umidade, sizeof(float));

  Serial.print("Enviando DADOS - Temp: ");
  Serial.print(temperatura);
  Serial.print("°C, Umidade: ");
  Serial.print(umidade);
  Serial.println("%");

  radio.stopListening();
  if (radio.write(&payloadTx, sizeof(payloadTx))) {
    Serial.println("DADOS enviados com sucesso!");
  } else {
    Serial.println("Falha ao enviar DADOS.");
  }
  
  delay(2000); // Pequena pausa entre transmissões
}
