#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#include "Command.h"

void menuInit(Command **commands, uint16_t commandsSize);
const char * menuExecuteCommand(char *commandName);


#endif //MENU_H
