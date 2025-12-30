#include <stdio.h>
#include <Windows.h>
#include "console.h"
#include "xim.h"

int main(int argc, char **argv) {
    initializeConsole();
    initVirtualBuffer();

    initializeXim();

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