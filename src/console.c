#include "console.h"
#include "xim.h"
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
    // SetConsoleCtrlHandler(NULL, 1);

    if (!GetConsoleScreenBufferInfo(hAlt, &console.Windows.csbi)) {
        return 1;
    }

    console.state.Size.width = console.Windows.csbi.srWindow.Right - console.Windows.csbi.srWindow.Left + 1;
    console.state.Size.height = console.Windows.csbi.srWindow.Bottom - console.Windows.csbi.srWindow.Top + 1;

    return 0;
}

int killConsole() {
    SetConsoleActiveScreenBuffer(console.hOldConsole);
    CloseHandle(console.windowsConsoleHandle);
    CloseHandle(console.hOldConsole);
    CloseHandle(console.hInput);

    // SetConsoleCtrlHandler(NULL, 0);

    return 0;
}

int rerenderScreen() {
    if (!GetConsoleScreenBufferInfo(console.windowsConsoleHandle, &console.Windows.csbi)) {
        return 1;
    }

    console.state.Size.width = console.Windows.csbi.srWindow.Right - console.Windows.csbi.srWindow.Left + 1;
    console.state.Size.height = console.Windows.csbi.srWindow.Bottom - console.Windows.csbi.srWindow.Top + 1;
    // do other stuff

    recalculateScreenBuffers();
    renderVirtualBuffer(1);

    // Relocate cursor after resize and
    if (Xim.mode == COMMAND_MODE) {
        setCursorPosition(Xim.commandArea.startLoc, Xim.commandBuffer.cursor);
    } else {
        setCursorPosition(Xim.editorArea.startLoc, Xim.editorBuffer.cursor);
    }

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
        where.X + size.X - 1,
        where.Y + size.Y - 1,
    };

    BOOL x = WriteConsoleOutput(
        console.windowsConsoleHandle,
        buffer,
        size,
        (COORD) {0,0},
        &rect
    );

    if (!x) {
        abort(); // for now
    }

    return 0;
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
        rerenderScreen();
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
