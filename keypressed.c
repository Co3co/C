#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    if (access("/tmp/presslog", F_OK) != 0)
        return 0;

    char buf, keyRepeatSpeed[24];
    float logLines;

    FILE *file = fopen("/tmp/presslog", "r");
    for (buf = getc(file); buf != EOF; buf = getc(file))
        logLines = (buf == '\n') ? logLines + 1 : logLines;
    fclose(file);
    remove("/tmp/presslog");

    FILE *fkeyRepeatSpeed = popen("xset -q | awk NR==9 | awk '{print $7}'", "r");
    fgets(keyRepeatSpeed, 24, fkeyRepeatSpeed);
    fclose(fkeyRepeatSpeed);

    printf("%.0f", logLines / atof(keyRepeatSpeed) * 1000);
    return 0;
}

