#include "console.h"
#include <assert.h>

Console console = {
    .lastWrittenCharacters = 0
};

inline int write_text(char *buffer, COORD coords);

int initializeConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // alternate buffer
    HANDLE hAlt = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );

    if (hAlt == NULL) {
        return 1;
    }

    SetConsoleActiveScreenBuffer(hAlt);

    console.hMainConsole = hConsole;
    console.windowsConsoleHandle = hAlt;
    
    if (!GetConsoleScreenBufferInfo(hAlt, &console.Windows.csbi)) {
        return 1;
    }

    return 0;
}

int killConsole() {
    SetConsoleActiveScreenBuffer(console.hMainConsole);
    CloseHandle(console.windowsConsoleHandle);

    return 0;
}

int write(enum WriteType type, void *value, COORD where) {
    switch(type) {
        case TEXT:
            write_text((char *) value, where);
            break;
        default:
            assert(0 && "WRITING THIS TYPE IS NOT YET IMPLEMENTED");
            break;
    }

    return 0;
}

inline int write_text(char *buffer, COORD coords) {
    return WriteConsoleOutputCharacter(
        console.windowsConsoleHandle,
        buffer,
        strlen(buffer),
        coords,
        &console.lastWrittenCharacters
    );
}
