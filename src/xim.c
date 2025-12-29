#include "xim.h"

int initVirtualBuffer(Console console) {
    Xim.mode = COMMAND_MODE;

    Xim.editorBuffer.size.width = console.state.Size.width;
    Xim.commandBuffer.size.width = console.state.Size.width;
    Xim.editorBuffer.size.height = console.state.Size.height - 1;
    Xim.commandBuffer.size.height = 1;

    Xim.editorBuffer.startLoc.x = Xim.editorBuffer.startLoc.y = 0;
    Xim.commandBuffer.startLoc.x = Xim.editorBuffer.startLoc.x;
    Xim.commandBuffer.startLoc.y = console.state.Size.height - 1;
    
    Xim.editorBuffer.dirty = 0;
    Xim.editorBuffer.cursor = 0;
    Xim.commandBuffer.dirty = 0;
    Xim.commandBuffer.cursor = 0;

    size_t bufferSize = sizeof(*(Xim.editorBuffer.cells)) * Xim.editorBuffer.size.width * Xim.editorBuffer.size.height;
    Xim.editorBuffer.cells = malloc(bufferSize);
    
    bufferSize = sizeof(*(Xim.commandBuffer.cells)) * Xim.commandBuffer.size.width * Xim.commandBuffer.size.height;
    Xim.commandBuffer.cells = malloc(bufferSize);

    if (Xim.editorBuffer.cells == NULL || Xim.commandBuffer.cells == NULL) {
        free(Xim.editorBuffer.cells);
        free(Xim.commandBuffer.cells);

        return 1;
    }

    for (size_t i = 0; i < Xim.editorBuffer.size.width * Xim.editorBuffer.size.height; i++) {
        Xim.editorBuffer.cells[i].Char.AsciiChar = ' ';
        Xim.editorBuffer.cells[i].Attributes = 0;
    }

    for (size_t i = 0; i < Xim.commandBuffer.size.width * Xim.commandBuffer.size.height; i++) {
        Xim.commandBuffer.cells[i].Char.AsciiChar = ' ';
        Xim.commandBuffer.cells[i].Attributes = 0;
    }

    return 0;
}


int killVirtualBuffer() {
    free(Xim.editorBuffer.cells);
    free(Xim.commandBuffer.cells);

    return 0;
}

int addToBuffer(char character) {
    Buffer *buffer;

    if (Xim.mode == COMMAND_MODE) {
        buffer = &Xim.commandBuffer;
    } else if (Xim.mode == RAW_MODE) {
        buffer = &Xim.editorBuffer;
    } else {
        return 1;
    }

    buffer->cells[buffer->cursor].Char.AsciiChar = character;
    buffer->cells[buffer->cursor].Attributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    
    if (buffer->cursor < buffer->size.width * buffer->size.height) {
        buffer->cursor += 1;
    }

    buffer->dirty = 1;

    return 0;
}

int renderVirtualBuffer() {
    if (!(Xim.editorBuffer.dirty || Xim.commandBuffer.dirty))
        return 0;

    if (Xim.mode == COMMAND_MODE) {
        writeWindowsBuffer(Xim.commandBuffer.cells, (COORD) {.X = Xim.commandBuffer.startLoc.x, .Y = Xim.commandBuffer.startLoc.y }, (COORD) {.X = Xim.commandBuffer.size.width, .Y = Xim.commandBuffer.size.height });
        Xim.commandBuffer.dirty = 0;
    } else if (Xim.mode == RAW_MODE) {
        writeWindowsBuffer(Xim.editorBuffer.cells, (COORD) {.X = Xim.editorBuffer.startLoc.x, .Y = Xim.editorBuffer.startLoc.y }, (COORD) {.X = Xim.editorBuffer.size.width, .Y = Xim.editorBuffer.size.height });
        Xim.editorBuffer.dirty = 0;
    }

    return 0;
}
