#include <stdio.h>
#include <Windows.h>
#include "console.h"

char *buffer;

int main(int argc, char **argv) {
    COORD coords = { 0, 0 };
    int written = 0;
    printf("Hello world!\n");
    printf(argv[0]);

    initializeConsole();

    int screen_size = console.Windows.csbi.dwSize.X * console.Windows.csbi.dwSize.Y;

    buffer = malloc(screen_size);

    for(int i = 0; i < screen_size; i++)
    buffer[i] = ' ';

    buffer[screen_size] = '\0';

    if (buffer == NULL) {
        return 1;
    }

    // Clear the page
    if(write(TEXT, buffer, coords) != 0) {
        return 1;
    } else {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", console.lastWrittenCharacters);
        write(TEXT, buf, coords);
        snprintf(buf, sizeof(buf), "%d", console.lastWrittenCharacters);
        write(TEXT, buf, (COORD) {0, 1});
        while(1) {
             Sleep(10);
        }
    }

    free(buffer);

    killConsole();

    return 0;
}

// Own the command line, raw terminal mode
// clear the command line page
// show cursor
// make a reserved row for commands down below
// make the button i open insert, and esc to allow command mode
// add :q to quit, and command parsing