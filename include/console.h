#ifndef CONSOLE_H_
#define CONSOLE_H_
#include <Windows.h>

enum WriteType {
    TEXT = 0,
    DIGIT,
    TYPE_FLOAT
};

typedef struct {
    // hMainConsole:
    // This is the main console, after the program is done, we go back to it.
    // That's its only use.
    HANDLE hMainConsole;
    HANDLE hInput;
    HANDLE windowsConsoleHandle;
    struct {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
    } Windows;

    struct {
        struct {
            DWORD lastReadCharsCount;
            wchar_t lastKeyCode;
            wchar_t character;
        } Input;

        struct {
            DWORD lastWrittenCharsCount;
        } Output;

        struct {
            short width;
            short height;
        } Size;
    } state;

} Console;

extern Console console;

int initializeConsole();
int write(enum WriteType type, void *value, COORD where);
int killConsole();
int writeWindowsBuffer(CHAR_INFO *buffer, COORD where);
int renderScreen();
#endif