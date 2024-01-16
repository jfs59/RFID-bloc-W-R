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
