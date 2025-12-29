#ifndef XIM_H_
#define XIM_H_
#include <Windows.h>
#include <assert.h>
#include "console.h"
#include "types.h"

enum XIM_MODES {
    RAW_MODE = 0,
    SELECTION_MODE,
    COMMAND_MODE
};

enum XIM_BUFFER_TYPES {
    COMMAND_BUFFER = 0,
    EDITOR_BUFFER
};

typedef struct {
    CHAR_INFO *cells;

    struct {
        int width;
        int height;
    } size;

    Vector2d startLoc;

    int cursor;
    short dirty;
} Buffer;

struct {
    enum XIM_MODES mode; // default: command mode
    short command_started;
    Buffer editorBuffer;
    Buffer commandBuffer;
} Xim;

int initVirtualBuffer();
int killVirtualBuffer();
int renderVirtualBuffer();
int addToBuffer(enum XIM_BUFFER_TYPES type, char character);
int addToCurrentBuffer(char character);

#endif