/*
  DETECTION ET DECOMPOSITION D'UNE COMMANDE
*/

#include <stdlib.h>
#include <string.h>
#include "RFID.h"

void lireCommande();

//Accusé de reception commande
void ack_command(char *message);

//This function execute a command. Amend this function with your own commands
void exec_command();

//Parsing identifiant commande et arguments
void parse_command(); 
