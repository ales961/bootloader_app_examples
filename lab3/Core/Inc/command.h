#ifndef COMAND_H
#define COMAND_H

typedef struct Command Command;

typedef const char * (*CommandAction)(const void *);

Command * commandCreate(const char *name, CommandAction action);
const char * commandExecute(const Command *command);
void commandSetAction(Command *command, CommandAction action);
const char * commandGetName(const Command *command);
CommandAction commandGetAction(const Command * command);


#endif //COMAND_H
