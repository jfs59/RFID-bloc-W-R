
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

extern MFRC522 mfrc522;


extern unsigned char writeBlockData[16];
extern  byte bufferReadLen;
extern  unsigned char readBlockData[18];

bool autorisationBloc(byte bloc_num);
void factory_key();
void detection_Carte();
void lire_Bloc(byte numblock);
void ecrire_Bloc(byte numblock);
void liberer_Carte();




void dump_byte_array(byte *buffer, byte bufferSize);
