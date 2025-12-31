#include "commands.h"

enum SIGNALS parseCommandFromBuffer(Vector *buffer) {
    if (!strcmp((char *) buffer->base, "q")) {
        return EXIT_SIGNAL;
    }

    return NOP_SIGNAL;
}
