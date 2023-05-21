#include "command.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Command {
    char *name;
    CommandAction action;
};

Command * commandCreate(const char * const name, const CommandAction action) {
    size_t nameLen = strlen(name);
    char *nameCopy = (char *) malloc(sizeof(char) * (nameLen + 1));
    strcpy(nameCopy, name);
    Command *command = (Command *) malloc(sizeof(Command));
    command->name = nameCopy;
    command->action = action;
    return command;
}

const char * commandExecute(const Command * const command) {
    if (command == NULL || command->action == NULL)
        return NULL;
    return (*command->action)(NULL);
}

void commandSetAction(Command * const command, const CommandAction action) {
    if (command == NULL)
        return;
    command->action = action;
}

const char * commandGetName(const Command * const command) {
    if (command == NULL)
        return NULL;
    return command->name;
}

CommandAction commandGetAction(const Command * const command) {
    return command->action;
}
