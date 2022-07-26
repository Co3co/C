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
    char opt;
    int acc;
    strcpy(fileName, argv[1]);

    opt = getopt(argc, argv, "-a");
    switch(opt) {
        case 'a':
            acc = 1;
            strcpy(fileName, argv[optind++]);
            break;
        default:
            break;
    }

    if (access(fileName, F_OK) != 0) {
        puts("File does not exist.");
        return 0;
    }

    FILE *file = fopen(fileName, "r");
    while ((buf = getc(file)) != EOF) {
        printf("%c", buf);
        if (acc == 1)
            fflush(stdout);
    }
    fclose(file);
    return 0;
}

