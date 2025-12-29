#include "xim.h"

Buffer mainBuffer = {0};

int initVirtualBuffer(Console console) {
    mainBuffer.size.width = console.state.Size.width;
    mainBuffer.size.height = console.state.Size.height;
    mainBuffer.dirty = 0;

    int bufferSize = sizeof(*(mainBuffer.cells)) * mainBuffer.size.width * mainBuffer.size.height;
    mainBuffer.cells = malloc(bufferSize);

    for (int i = 0; i < mainBuffer.size.width * mainBuffer.size.height; i++) {
        mainBuffer.cells[i].Char.AsciiChar = ' ';
        mainBuffer.cells[i].Attributes = 0;
    }

    return 0;
}


int killVirtualBuffer() {
    free(mainBuffer.cells);

    return 0;
}

int addToBuffer(char character) {
    mainBuffer.cells[mainBuffer.cursor].Char.AsciiChar = console.state.Input.character;
    mainBuffer.cells[mainBuffer.cursor].Attributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    
    if (mainBuffer.cursor < mainBuffer.size.width * mainBuffer.size.height) {
        mainBuffer.cursor += 1;
    }

    mainBuffer.dirty = 1;

    return 0;
}

int renderVirtualBuffer() {
    if (!mainBuffer.dirty)
        return 0;

    writeWindowsBuffer(mainBuffer.cells, (COORD) {0,0});

    mainBuffer.dirty = 0;

    return 0;
}
