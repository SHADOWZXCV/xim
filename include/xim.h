#ifndef XIM_H_
#define XIM_H_
#include "Windows.h"
#include "console.h"

enum XIM_MODES {
    RAW_MODE = 0,
    SELECTION_MODE,
    COMMAND_MODE
};

typedef struct {
    CHAR_INFO *cells;

    struct {
        int width;
        int height;
    } size;

    struct {
        int x;
        int y;
    } startLoc;

    int cursor;
    short dirty;
} Buffer;

struct {
    enum XIM_MODES mode; // default: command mode
    Buffer editorBuffer;
    Buffer commandBuffer;
} Xim;

int initVirtualBuffer();
int killVirtualBuffer();
int renderVirtualBuffer();

#endif