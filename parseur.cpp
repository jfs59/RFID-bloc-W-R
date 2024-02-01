
#include "Arduino.h"
#include "parseur.h"

/*DETECTION ET DECOMPOSITION D'UNE COMMANDE*/

#define MAX_CMD_LEN 80
#define MAX_ARGS_LEN 6
#define MAX_ARGS 40

const char NCard_Str[] PROGMEM = "NCard"; // Détecter nouvelle carte

// écriture avec vérification du bloc

const char WHexa_Str[] PROGMEM = "WHexa"; // Ecrire un bloc en héxadécimal
const char WDeci_Str[] PROGMEM = "WDeci"; // Ecrire un bloc en décimal
const char WText_Str[] PROGMEM = "WText"; // Ecrire un bloc en texte

// Lecture d'un bloc

const char RHexa_Str[] PROGMEM = "RHexa"; // Lire un bloc en héxadécimal
const char DHexa_Str[] PROGMEM = "DHexa"; // Dump des blocs en héxadécimal

// opération sur Key

const char KSwap_Str[] PROGMEM = "KSwap"; // Changer l'identification
const char KUtil_Str[] PROGMEM = "KUtil"; // Modifier la clé
const char KInit_Str[] PROGMEM = "KInit"; // Retour clé FF FF FF FF FF FF
const char WFact_Str[] PROGMEM = "WFact"; // Changer factory

const char WBlaz_Str[] PROGMEM = "WBlaz"; // procedure rapide

const char *const Command_List[] PROGMEM = {NCard_Str, WHexa_Str, WDeci_Str, WText_Str, RHexa_Str, DHexa_Str, KSwap_Str, KUtil_Str, KInit_Str, WFact_Str,WBlaz_Str};

const byte NB_COMMANDS = sizeof(Command_List) / sizeof(Command_List[0]);

enum Command_IDs : byte {
  NCard = 0,
  WHexa = 1,
  WDeci = 2,
  WText = 3,
  RHexa = 4,
  DHexa = 5,
  KSwap = 6,
  KUtil = 7,
  KInit = 8,
  WFact = 9,
  WBlaz = 10
};

//Reception commande

int Command_ID;
char arg_list[MAX_ARGS][MAX_ARGS_LEN];
char incomingByte;
int command_index = 0;
char command_line[MAX_CMD_LEN];
int iPrintConfirmation;

void(* resetFunc) (void) = 0; // adresse de fonction reset;

void lireCommande() {
  static byte CmdOk = 0;
  if (Serial.available()) {
    incomingByte = Serial.read();
    //Serial.println(incomingByte);
    switch (incomingByte)
    {
      case '<':
        CmdOk = 1;
        command_index = 0;
        memset(command_line, 0, MAX_CMD_LEN - 1);
        break;
      case '>':
        CmdOk = 0;
        command_line[command_index + 1] = 0;
        parse_command();
        exec_command();
        break;
      default :
        if ((CmdOk == 1) && (command_index < MAX_CMD_LEN)) {
          command_line[command_index++] = incomingByte;
        }
    }
  } // Serial.println(command_line);
}

//Accusé de reception commande

void ack_command(char *message)
{
  //iPrintConfirmation =1;
  if (iPrintConfirmation > 0)
  {
    Serial.print(F("Command executée : "));
    Serial.print((const __FlashStringHelper *)pgm_read_word(&Command_List[Command_ID]));
    Serial.print("->");
    Serial.print(message);
    Serial.println();
  }
}

//execution des commandes

void exec_command()
{
  byte bloc;
  switch (Command_ID)
  {
    case NCard:
      {
        Serial.println(F("Détecter nouvelle carte"));
        liberer_Carte();
        delay(500);
        detection_Carte();
      }
      break;

    case WHexa:
      {
        //Serial.println(F("Ecrire bloc HEXA"));
        bloc = atoi(arg_list[1]);
        if (autorisationBloc(bloc)) {
          //Serial.println(F("Ecriture autorisée"));
          clear_Write_Buffer ();
          for (int i = 0; i < 16; i++) {
            byte val = strtoul (arg_list[i + 2], nullptr, 16);
            writeBlockData[i] = val;
          }
          dump_byte_array(writeBlockData, 16);
          // transfert et ecriture ....
          ecrire_Bloc(bloc);
          //Serial.println(F("Relecture du bloc"));
          //lire_Bloc(bloc);
          //dump_byte_array(readBlockData, 16);
        } else {
          Serial.print(F("Ecriture interdite bloc : "));
          Serial.println(bloc);
        }
      }
      break;

    case WDeci:
      {
        Serial.println(F("Ecrire bloc DECI"));
        bloc = atoi(arg_list[1]);
        if (autorisationBloc(bloc)) {
          Serial.println(F("Ecriture autorisée"));
          for (int i = 0; i < 16; i++) {
            byte val = atoi(arg_list[i + 2]);
            writeBlockData[i] = val;
          }
          dump_byte_array(writeBlockData, 16);
          // transfert et ecriture ....
          ecrire_Bloc(bloc);
          Serial.println(F("Relecture du bloc"));

          lire_Bloc(bloc);
          dump_byte_array(readBlockData, 16);
        } else {
          Serial.print(F("Ecriture interdite bloc : "));
          Serial.println(bloc);
        }
      }
      break;

    case WText:
      {
        Serial.println(F("Ecrire bloc TEXT"));
        bloc = atoi(arg_list[1]);
        if (autorisationBloc(bloc)) {
          Serial.println(F("Ecriture autorisée"));
          // transfert et ecriture ....
          clear_Write_Buffer ();
          for (int i = 0; i < 16; i++) {
            writeBlockData[i] = (byte) * arg_list[i + 2];
          }
          dump_byte_array(writeBlockData, 16);
          ecrire_Bloc(bloc);
          Serial.println(F("Relecture du bloc"));
          lire_Bloc(bloc);
          dump_byte_array(readBlockData, 16);

        } else {
          Serial.print(F("Ecriture interdite bloc : "));
          Serial.println(bloc);
        }
      }
      break;

    case RHexa:
      {
        Serial.println(F("Lecture bloc HEXA"));
        bloc = atoi(arg_list[1]);
        lire_Bloc(bloc);
        dump_byte_array(readBlockData, 16);
      }
      break;

    case DHexa:
      {
        Serial.println(F("Dump bloc HEXA"));
        byte blocd = atoi(arg_list[1]);
        byte bloca = atoi(arg_list[2]);
        for (byte bloc = blocd; bloc < bloca + 1; bloc++) {
          lire_Bloc(bloc);
          dump_byte_array(readBlockData, 16);
        }
      }
      break;

    case KSwap:
      {
        Serial.print(F("Authentification avec clé: "));
        key_util_A = !key_util_A;
        if (key_util_A) Serial.println ("A");
        else Serial.println ("B");
      }
      break;

    case KUtil:
      {
        Serial.print(F("Modification de la clé: "));
        for (byte i = 0; i < 6; i++)
        {
          byte val = strtoul (arg_list[i + 1], nullptr, 16);
          key.keyByte[i] = val;
        }
        dump_byte_array(key.keyByte, 6);
      }
      break;

    case KInit:
      {
        Serial.print(F("Modification de la clé: "));
        factory_key();
      }
      break;

    case WFact:
      {
        Serial.println(F("Modification usine"));
        lire_Bloc(0);
        clear_Write_Buffer ();
        for (int i = 0; i < 7; i++) {
          writeBlockData[i] = readBlockData[i];
        }
        for (int i = 7; i < 16; i++) {
          writeBlockData[i] = (byte) * arg_list[i + 1];
        }
        ecrire_Bloc(0);
      }
      break;

    case WBlaz: // procedure rapide lasarus
      {
        bloc = atoi(arg_list[1]);
        clear_Write_Buffer ();
        for (int i = 0; i < 16; i++) {
          byte val = strtoul (arg_list[i + 2], nullptr, 16);
          writeBlockData[i] = val;
        }
        ecrire_Bloc(bloc);
      }
      break;

    default :
      {
        Serial.print(F("Erreur commande -> "));
        Serial.println(command_line);
      }
      break;
  }
  Command_ID = -1;
  //Serial.println(F("*******************"));
  //Serial.println(F("Attente nouvelle commande"));
}

//Parsing identifiant commande et arguments

void parse_command()
{
  char * pch;
  int ii = 0;

  //parsing et Identification des attributs

  pch = strtok (command_line, "|");
  while (pch != NULL)
  {
    strcpy(arg_list[ii], pch);
    pch = strtok (NULL, "|");
    //Serial.print(arg_list[ii]);Serial.print("--");Serial.println(ii);
    ii++;
  }

  //identification de l'ID
  Command_ID = -1;
  for (ii = 0; ii < NB_COMMANDS; ii++)
  {

    //Serial.print("Comparaison de <"); Serial.print((const __FlashStringHelper *)pgm_read_word(&Command_List[ii])); Serial.print("> et de <");
    //Serial.print(arg_list[0]); Serial.println(">");

    if (strcmp_P(arg_list[0], pgm_read_word(&Command_List[ii])) == 0)

    {
      Command_ID = ii;
      //Serial.println("Commande autorisée");
      //Serial.println(ii);
      break;
    }
  }
}
