
#include "RFID.h"
#include "Arduino.h"

MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
bool key_util_A;

void clear_Read_Buffer () {

  for (byte i = 0; i < 17; i++) readBlockData[i] = 0;
}
void clear_Write_Buffer () {

  for (byte i = 0; i < 15; i++) writeBlockData[i] = 0;
}
void factory_key() {
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //Serial.println(F("Factory Key FF FF FF FF FF FF"));
}

bool autorisationBloc(byte bloc_num) {
  bool autorisation = false;
  if (bloc_num > 0 && (bloc_num % 4) < 3 ) {
    autorisation = true;
  }
  return autorisation;
}
void detection_Carte() {

  byte bufferWriteLen = 16;

  factory_key ();
  key_util_A = true;
  //Serial.println(F("Utilisation de la clé A"));
  //Serial.println(F("Détection d'une carte RFID"));

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    Serial.println(F("Pas de nouvelle carte"));
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    Serial.println(F("Pas de serial"));
    return;
  }
  Serial.print(F("Card UID:"));    //Dump UID

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.println();
  Serial.println(F("Attente de commande"));
  //Serial.println("");
}
void change_Factory()
{
  lire_Bloc(0);
  
}
  void lire_Bloc(byte numblock) {
  clear_Read_Buffer ();

  if (key_util_A) status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, numblock, &key, &(mfrc522.uid));
  else status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, numblock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(numblock, readBlockData, &bufferReadLen);
}

void ecrire_Bloc(byte numblock) {

  if (key_util_A) status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, numblock, &key, &(mfrc522.uid));
  else status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, numblock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Write(numblock, writeBlockData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
}

void liberer_Carte() {

  Serial.println();
  Serial.println("Arret lecteur");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}
