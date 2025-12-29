#include <stdio.h>
#include <Windows.h>
#include "console.h"
#include "xim.h"

char *buffer;

int main(int argc, char **argv) {
    COORD coords = { 0, 0 };
    int written = 0;
    printf("Hello world!\n");
    printf(argv[0]);

    initializeConsole();
    initVirtualBuffer(console);

    // int screen_size = console.Windows.csbi.dwSize.X * console.Windows.csbi.dwSize.Y;

    // buffer = malloc(screen_size);

    // for(int i = 0; i < screen_size; i++)
    // buffer[i] = ' ';

    // buffer[screen_size] = '\0';

    // if (buffer == NULL) {
    //     return 1;
    // }

    // // Clear the page
    // if(write(TEXT, buffer, coords) != 0) {
    //     return 1;
    // } else {
        // char buf[16];
        // snprintf(buf, sizeof(buf), "%d", console.state.Output.lastWrittenCharsCount);
        // write(TEXT, buf, coords);
        // snprintf(buf, sizeof(buf), "%d", console.state.Output.lastWrittenCharsCount);
        // write(TEXT, buf, (COORD) {0, 1});
    INPUT_RECORD record;

    while(1) {
        // read keystrokes
        ReadConsoleInput(console.hInput, &record, 1, &console.state.Input.lastReadCharsCount);

        if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
            renderScreen(record.Event.WindowBufferSizeEvent.dwSize);
        }

        if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
            console.state.Input.lastKeyCode = record.Event.KeyEvent.wVirtualKeyCode;
            console.state.Input.character = record.Event.KeyEvent.uChar.AsciiChar;

            if (console.state.Input.character) {
                addToBuffer(console.state.Input.character);
                // write(TEXT, (char *) &console.state.Input.character, (COORD) {console.state.Size.width - 1, console.state.Size.height - 1});

                if (console.state.Input.character == "I") {
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
    // }

    // free(buffer);

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