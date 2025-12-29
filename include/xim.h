#ifndef XIM_H_
#define XIM_H_
#include "Windows.h"
#include "console.h"

typedef struct {
    CHAR_INFO *cells;

    struct {
        int width;
        int height;
    } size;

    int cursor;
    short dirty;
} Buffer;

extern Buffer mainBuffer;

int initVirtualBuffer();
int killVirtualBuffer();
int renderVirtualBuffer();

#endif