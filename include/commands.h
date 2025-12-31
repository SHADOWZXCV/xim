#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "xim.h"
#define MAX_COMMAND_LEN 256

enum SIGNALS parseCommandFromBuffer(Vector *buffer);

#endif