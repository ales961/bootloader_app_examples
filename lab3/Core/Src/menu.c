#include "menu.h"

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"

Command **menuCommands = NULL;
uint16_t menuSize;

void menuInit(Command ** const commands, const uint16_t commandsSize) {
    if (menuCommands != NULL)
        return;
    menuCommands = (Command **) malloc(sizeof(Command *) * commandsSize);
    for (uint16_t i = 0; i < commandsSize; i++) {
        menuCommands[i] = commands[i];
    }
    menuSize = commandsSize;
}

const char * menuExecuteCommand(char * const commandName) {
    for (uint16_t i = 0; i < menuSize; i++) {
        const Command * const command = menuCommands[i];
        if (strcmp(commandGetName(menuCommands[i]), commandName) == 0) {
            return commandExecute(command);
        }
    }
    return "Command not found\n";
}


