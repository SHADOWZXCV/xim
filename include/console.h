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
    HANDLE windowsConsoleHandle;
    struct {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
    } Windows;

    int lastWrittenCharacters;
} Console;

extern Console console;

int initializeConsole();
int write(enum WriteType type, void *value, COORD where);
int killConsole();
#endif