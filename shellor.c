#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    char cmd[256];

    for (int i = 1; i < argc; i++) {
        strcat(cmd, argv[i]);
        strcat(cmd, " ");
    }
    system(cmd);

    return 0;
}

