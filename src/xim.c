#include "xim.h"

int resetCommandBuffer() {
    Xim.command_started = 0;
    Xim.commandBuffer.cursor = 0;

    for (size_t i = 0; i < (size_t) (Xim.commandBuffer.size.width * Xim.commandBuffer.size.height); i++) {
        Xim.commandBuffer.cells[i].Char.AsciiChar = ' ';
        Xim.commandBuffer.cells[i].Attributes = 0;
    }
    Xim.commandBuffer.dirty = 1;

    return 0;
}

int initVirtualBuffer() {
    Xim.mode = COMMAND_MODE;
    Xim.signal = NOP_SIGNAL;

    //! TODO: Change these later
    Xim.editorBuffer.size.width = 500;
    Xim.editorBuffer.size.height = 1000;
    Xim.commandBuffer.size.width = 500;
    Xim.commandBuffer.size.height = 1;

    Xim.editorBuffer.cursor = 0;
    Xim.commandBuffer.cursor = 0;

    Xim.command_started = 0;

    size_t bufferSize =  Xim.editorBuffer.size.width * Xim.editorBuffer.size.height;
    Xim.editorBuffer.cells = calloc(bufferSize, sizeof(*(Xim.editorBuffer.cells)));
    
    bufferSize = Xim.commandBuffer.size.width * Xim.commandBuffer.size.height;
    Xim.commandBuffer.cells = calloc(bufferSize, sizeof(*(Xim.commandBuffer.cells)));

    if (Xim.editorBuffer.cells == NULL || Xim.commandBuffer.cells == NULL) {
        free(Xim.editorBuffer.cells);
        free(Xim.commandBuffer.cells);

        return 1;
    }

    recalculateScreenBuffers();
    renderVirtualBuffer(1);

    return 0;
}

int recalculateScreenBuffers() {
    Xim.editorArea.size.width = console.state.Size.width;
    Xim.commandArea.size.width = console.state.Size.width;
    Xim.editorArea.size.height = console.state.Size.height - 1;
    Xim.commandArea.size.height = 1;

    Xim.editorArea.startLoc.x = Xim.editorArea.startLoc.y = 0;
    Xim.commandArea.startLoc.x = 0;
    Xim.commandArea.startLoc.y = console.state.Size.height - 1;

    Xim.editorBuffer.dirty = 1;
    Xim.commandBuffer.dirty = 1;

    return 0;
}


int killVirtualBuffer() {
    free(Xim.editorBuffer.cells);
    free(Xim.commandBuffer.cells);

    return 0;
}

int addToBuffer(enum XIM_BUFFER_TYPES type, char character) {
    Buffer *buffer;

    switch (type) {
        case COMMAND_BUFFER: {
            buffer = &Xim.commandBuffer;
        } break;

        case EDITOR_BUFFER: {
            buffer = &Xim.editorBuffer;
        } break;

        default: 
            assert(0 && "THIS TYPE OF BUFFER DOES NOT EXIST");
        break;
    }

    if (buffer->cursor < buffer->size.width * buffer->size.height) {
        buffer->cells[buffer->cursor].Char.AsciiChar = character;
        buffer->cells[buffer->cursor].Attributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

        buffer->cursor += 1;
        buffer->dirty = 1;
    }

    return 0;
}

int addBufferToBuffer(enum XIM_BUFFER_TYPES type, char *text, int at) {
    char character = '\0';

    Buffer *buffer;

    switch (type) {
        case COMMAND_BUFFER: {
            buffer = &Xim.commandBuffer;
        } break;

        case EDITOR_BUFFER: {
            buffer = &Xim.editorBuffer;
        } break;

        default: 
            assert(0 && "THIS TYPE OF BUFFER DOES NOT EXIST");
        break;
    }

    buffer->cursor = at;

    while (character = *text++) {
        if (buffer->cursor < buffer->size.width * buffer->size.height) {
            buffer->cells[buffer->cursor].Char.AsciiChar = character;
            buffer->cells[buffer->cursor].Attributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            buffer->cursor++;
        } else { 
            break;
        }
    }

    if (at != buffer->cursor) {
        buffer->dirty = 1;
    }

    return 0;
}

int addToCurrentBuffer(char character) {
    Buffer *buffer;
    Area *area;

    if (Xim.mode == COMMAND_MODE) {
        buffer = &Xim.commandBuffer;
        area = &Xim.commandArea;
    } else if (Xim.mode == RAW_MODE) {
        buffer = &Xim.editorBuffer;
        area = &Xim.editorArea;
    } else {
        return 1;
    }

    if (character == ':' && !Xim.command_started && Xim.mode == COMMAND_MODE) {
        Xim.command_started = 1; // get it to 0 if the command is processed or the escape button is pressed
    }

    if (Xim.mode == COMMAND_MODE) {
        if (!Xim.command_started) {
            return 0; // ignore the character
        }
    }
    if (buffer->cursor < buffer->size.width * buffer->size.height) {
        buffer->cells[buffer->cursor].Char.AsciiChar = character;
        buffer->cells[buffer->cursor].Attributes |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        buffer->dirty = 1;
        buffer->cursor++;
    }

    setCursorPosition(area->startLoc, buffer->cursor);

    return 0;
}

int flushScreenBuffer(Area *area) {
    Buffer flushBuffer;

    flushBuffer.cells = malloc(sizeof(*(flushBuffer.cells)) * area->size.width * area->size.height);
    flushBuffer.size = (Size2s) { area->size.width, area->size.height };
    flushBuffer.cursor = 0;

    for (size_t i = 0; i < (size_t) (flushBuffer.size.width * flushBuffer.size.height); i++) {
        flushBuffer.cells[i].Char.AsciiChar = ' ';
        flushBuffer.cells[i].Attributes = 0;
    }

    writeWindowsBuffer(
        flushBuffer.cells,
        (COORD){ .X = area->startLoc.x, .Y = area->startLoc.y },
        (COORD){ .X = area->size.width, .Y = area->size.height }
    );

    free(flushBuffer.cells);

    return 0;
}

int renderVirtualBuffer(unsigned short flush) {
    if (!(Xim.editorBuffer.dirty || Xim.commandBuffer.dirty))
        return 0;

    Buffer *buffer;
    Area *area;

    if (flush != 0) {
        if (Xim.editorBuffer.dirty) {
            flushScreenBuffer(&Xim.editorArea);
        }
        Sleep(1);
        if (Xim.commandBuffer.dirty) {
            flushScreenBuffer(&Xim.commandArea);
        }
        // A hack: wait before writing to the buffers
        Sleep(1);
    }

    if (Xim.editorBuffer.dirty) {
        buffer = &(Xim.editorBuffer);
        area = &Xim.editorArea;
        writeWindowsBuffer(
            buffer->cells,
            (COORD){ .X = area->startLoc.x, .Y = area->startLoc.y },
            (COORD){ .X = area->size.width, .Y = area->size.height }
        );
        buffer->dirty = 0;
    }
    if (Xim.commandBuffer.dirty) {
        // A hack: wait before writing to the 2nd buffer
        Sleep(1);
        buffer = &(Xim.commandBuffer);
        area = &Xim.commandArea;
        writeWindowsBuffer(
            buffer->cells,
            (COORD){ .X = area->startLoc.x, .Y = area->startLoc.y },
            (COORD){ .X = area->size.width, .Y = area->size.height }
        );
        buffer->dirty = 0;
    }

    return 0;
}


int initializeXim() {
    KeyCode key;

    while(Xim.signal != EXIT_SIGNAL) {
        key = pollInputFromConsole();

        if (!key.character && !key.keyCode)
            continue;

        //! TODO: Tightly coupled to windows bruh.
        //! VK_ESCAPE, are you blind ??
        if (key.keyCode == VK_ESCAPE) {
            resetCommandBuffer();
            setCursorPosition(Xim.editorArea.startLoc, Xim.editorBuffer.cursor);
            Xim.mode = COMMAND_MODE;
            // enable command mode if it wasn't already
            //! TODO: These keys have multiple purposes inside raw mode, implement them you lazy fossil
        } else if (Xim.mode == COMMAND_MODE && !Xim.command_started &&
               (key.character == 's' || key.character == 'S' ||
                key.character == 'i' || key.character == 'I' ||
                key.character == 'o' || key.character == 'O' ||
                key.character == 'a' || key.character == 'A')) {
            // enable insert mode if it wasn't, otherwise just type the character to the screen
            addBufferToBuffer(COMMAND_BUFFER, "-- INSERT --", 0);
            Xim.mode = RAW_MODE;
        } else if (key.character) {
            addToCurrentBuffer((char) key.character);
        }

        renderVirtualBuffer(0);
    }

    return 0;
}
