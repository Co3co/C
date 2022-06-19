#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct File {
    char name[256];
    char contents[1048576];
    char lines[1048576];
};

int main(int argc, char **argv) {
    if (argv[1] == NULL) {
        puts("Please supply a file to read.");
        return 0;
    }

    struct File file;
    strcpy(file.name, argv[1]);
    char fileLine[1048576];
    char cmd[256];

    if (access(file.name, F_OK) != 0) {
        puts("File does not exist.");
        return 0;
    }

    snprintf(cmd, sizeof(cmd), "wc -l %s | awk '{print $1}'", file.name);
    FILE *fLines = popen(cmd, "r");
    fgets(file.lines, 1024, fLines);

    FILE *pFile = fopen(file.name, "r");
    for(int i = 0; i < atoi(file.lines); i++) {
        fgets(fileLine, 255, pFile);
        strcat(file.contents, fileLine);
    }
    fclose(pFile);

    printf("%s", file.contents);
    return 0;
}

