#define SKETCH_VERSION "1.0.01"
#define DATE_VERSION "16/01/2024 14:00:00"
#define AUTEUR "JFS59"


#include "RFID.h"
#include "parseur.h"


unsigned char writeBlockData[16];
byte bufferReadLen = 18;
unsigned char readBlockData[18];

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

void setup() {

  Serial.begin(115200);
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  detection_Carte();
}


void loop() {

}

void serialEvent() {
  lireCommande();
}
