#ifndef XIM_H_
#define XIM_H_
#include <Windows.h>
#include <assert.h>
#include "console.h"
#include "structures/vector.h"
#include "commands.h"
#include "types.h"

enum XIM_MODES {
    NO_MODE = 0,
    RAW_MODE,
    SELECTION_MODE,
    OPERATOR_MODE,
    EX_MODE
};

enum XIM_BUFFER_TYPES {
    CURRENT = 0,
    COMMAND_BUFFER,
    EDITOR_BUFFER
};

enum SIGNALS {
    EXIT_SIGNAL = 99,
    NOP_SIGNAL = 0,
};

typedef struct {
    CHAR_INFO *cells;
    Size2s size;
    int cursor;
    short dirty;
} Buffer;

typedef struct {
    Vector2d startLoc;
    Size2s size;
} Area;

struct {
    enum XIM_MODES mode; // default: command mode
    Buffer editorBuffer;
    Buffer commandBuffer;
    Vector *writtenCommand;
    Area editorArea;
    Area commandArea;
    enum SIGNALS signal;
} Xim;

struct {
    void(*handler)();
} WindowsKeyPresses [];

int initVirtualBuffer();
int killVirtualBuffer();
int renderVirtualBuffer(unsigned short flush);
int addBufferToBuffer(enum XIM_BUFFER_TYPES type, char *text, int at, unsigned short relocate_cursor);
int recalculateScreenBuffers();

#endif
