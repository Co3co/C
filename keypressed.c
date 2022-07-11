#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    if (access("/tmp/presslog", F_OK) != 0) {
        return 0;
    }

    char logLines[1048576], keyRepeatSpeed[24];

    FILE *flogLines = popen("wc -l /tmp/presslog | awk '{print $1}'", "r");
    fgets(logLines, 1048576, flogLines);
    fclose(flogLines);
    remove("/tmp/presslog");

    FILE *fkeyRepeatSpeed = popen("xset -q | awk NR==9 | awk '{print $7}'", "r");
    fgets(keyRepeatSpeed, 24, fkeyRepeatSpeed);
    fclose(fkeyRepeatSpeed);

    printf("%.0f", atof(logLines) / atof(keyRepeatSpeed) * 1000);
    return 0;
}

