#include "console.h"
#include <assert.h>

Console console = {
    .state = {
        .Output = {
            .lastWrittenCharsCount = 0
        },
        .Input = {
            .lastReadCharsCount = 0
        }
    }
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

    console.hOldConsole = hConsole;
    console.windowsConsoleHandle = hAlt;
    
    if (!GetConsoleScreenBufferInfo(hAlt, &console.Windows.csbi)) {
        return 1;
    }

    console.state.Size.width = console.Windows.csbi.dwSize.X;
    console.state.Size.height = console.Windows.csbi.dwSize.Y;

    // Get input buffer
    console.hInput = GetStdHandle(STD_INPUT_HANDLE);

    if (console.hInput == INVALID_HANDLE_VALUE) {
        return 1;
    }

    DWORD mode;
    GetConsoleMode(console.hInput, &mode);

    // disables Enter to get new data, and adding character to screen after writing it
    mode |= ENABLE_WINDOW_INPUT;
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

    SetConsoleMode(console.hInput, mode);

    return 0;
}

int killConsole() {
    SetConsoleActiveScreenBuffer(console.hOldConsole);
    CloseHandle(console.windowsConsoleHandle);
    CloseHandle(console.hOldConsole);
    CloseHandle(console.hInput);

    return 0;
}

int rerenderScreen(COORD size) {
    console.state.Size.width = size.X;
    console.state.Size.height = size.Y;
    // do other stuff
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
        &console.state.Output.lastWrittenCharsCount
    );
}

int writeWindowsBuffer(CHAR_INFO *buffer, COORD where, COORD size) {
    SMALL_RECT rect = {
        where.X, where.Y,
        where.X + size.X,
        where.Y + size.Y,
    };

    return WriteConsoleOutput(
        console.windowsConsoleHandle,
        buffer,
        size,
        (COORD) {0,0},
        &rect
    );
}

int setCursorPosition(Vector2d start, int next) {
    int x = start.x + next;
    COORD position = {
        .X = x % console.state.Size.width,
        .Y = start.y + (x / console.state.Size.width)
    };

    return SetConsoleCursorPosition(console.windowsConsoleHandle, position);
}

KeyCode pollInputFromConsole() {
    INPUT_RECORD record;

    // read keystrokes
    ReadConsoleInput(console.hInput, &record, 1, &console.state.Input.lastReadCharsCount);

    if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
        rerenderScreen(record.Event.WindowBufferSizeEvent.dwSize);
    }

     if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
        console.state.Input.lastKeyCode = record.Event.KeyEvent.wVirtualKeyCode;
        console.state.Input.character = record.Event.KeyEvent.uChar.AsciiChar;

        return (KeyCode) {
            .keyCode = record.Event.KeyEvent.wVirtualKeyCode,
            .character = record.Event.KeyEvent.uChar.AsciiChar
        };
    }

    // Essentially null ?
    return (KeyCode) {0};
}