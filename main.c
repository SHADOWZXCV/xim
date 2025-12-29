#include <stdio.h>
#include <Windows.h>
#include "console.h"
#include "xim.h"

char *buffer;

int main(int argc, char **argv) {
    initializeConsole();
    initVirtualBuffer(console);

    INPUT_RECORD record;

    while(1) {
        // read keystrokes
        ReadConsoleInput(console.hInput, &record, 1, &console.state.Input.lastReadCharsCount);

        if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
            rerenderScreen(record.Event.WindowBufferSizeEvent.dwSize);
        }

        if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
            console.state.Input.lastKeyCode = record.Event.KeyEvent.wVirtualKeyCode;
            console.state.Input.character = record.Event.KeyEvent.uChar.AsciiChar;

            if (console.state.Input.character) {
                addToCurrentBuffer((char) console.state.Input.character);

                if (console.state.Input.character == 'I') {
                    // enable insert mode if it wasn't, otherwise just type the character to the screen
                }
            } else {
                if (console.state.Input.lastKeyCode == VK_ESCAPE) {
                    // enable command mode if it wasn't already
                }
            }

        }

        renderVirtualBuffer();
    }

    killVirtualBuffer();
    killConsole();

    return 0;
}

// x Own the command line, raw terminal mode
// x clear the command line page
// x show cursor
// make the button i open insert, and esc to allow command mode
// make a reserved row for commands down below
// add :q to quit, and command parsing