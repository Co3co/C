#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argv[1] == NULL) {
        puts("Please provide a file to read.");
        return 0;
    }

    char fileName[256];
    char buf;
    strcpy(fileName, argv[1]);

    if (access(fileName, F_OK) != 0) {
        puts("File does not exist.");
        return 0;
    }

    FILE *file = fopen(fileName, "r");
    while (buf != EOF) {
        printf("%c", buf);
        buf = getc(file);
    }
    fclose(file);
    return 0;
}

