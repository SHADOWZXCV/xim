#include <stdio.h>

int main(int argc, char **argv) {

    printf("Hello world!\n");
    printf(argv[0]);

    return 0;
}

// Own the command line, raw terminal mode
// clear the command line page
// show cursor
// make a reserved row for commands down below
// make the button i open insert, and esc to allow command mode
// add :q to quit, and command parsing