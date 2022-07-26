#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdo.h>

xdo_t *xdo;
charcodemap_t *mods;
int nmods;

void click(int button);
void click_cords(int x, int y, int button, int screen);
void click_cords_return(int x, int y, int button, int screen);
void move(int x, int y, int screen);
void move_rel(int x, int y);
void move_smooth(float tx, float ty, int speedMS, int screen);
void get_coords(int *x, int *y, int *screen);
void get_coords_click(int *x, int *y, int *screen);
void hold(int holdTime, int button);
void drag(float ix, float iy, float tx, float ty, int speedMS, int button, int screen);
void get_pixel_color(char color[]);
int get_hold_time(float *holdTime);
void print_help(void);

void click(int button) {
    xdo_get_active_modifiers(xdo, &mods, &nmods);
    xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}

void click_cords(int x, int y, int button, int screen) {
    xdo_get_active_modifiers(xdo, &mods, &nmods);
    xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_move_mouse(xdo, x, y, screen);
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}

void click_cords_return(int x, int y, int button, int screen) {
    xdo_get_active_modifiers(xdo, &mods, &nmods);
    xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    int startX, startY, startScreen;
    xdo_get_mouse_location(xdo, &startX, &startY, &startScreen);

    xdo_move_mouse(xdo, x, y, screen);
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    xdo_move_mouse(xdo, startX, startY, screen);
    xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}

void move(int x, int y, int screen) {
    xdo_move_mouse(xdo, x, y, screen);
}

void move_rel(int x, int y) {
    xdo_move_mouse_relative(xdo, x, y);
}

void move_smooth(float tx, float ty, int speedMS, int screen) {
    float frac, loop, loops, x, y, posdx, posdy;
    int ix, iy;
    xdo_get_mouse_location(xdo, &ix, &iy, &screen);
    float dx = tx - ix, dy = ty - iy;
    if (dx < 0 || dy < 0) {
        posdx = (dx < 0) ? dx - dx * 2 : dx;
        posdy = (dy < 0) ? dy - dy * 2 : dy;
        loops = (posdx < posdy) ? posdy : posdx;
    } else {
        loops = (dx < dy) ? dy : dx;
    }

    for (loop = 1; loop <= loops; loop++) {
        frac = loop / loops;
        x = ix + (frac * dx);
        y = iy + (frac * dy);
        xdo_move_mouse(xdo, x, y, screen);
        usleep(speedMS*1000);
    }
}

void get_coords(int *x, int *y, int *screen) {
    xdo_get_mouse_location(xdo, x, y, screen);
}

void hold(int holdTime, int button) {
    if (access("/tmp/presslog", F_OK) != 0) {
        xdo_mouse_down(xdo, CURRENTWINDOW, button);
        usleep(holdTime*1000);
        xdo_mouse_up(xdo, CURRENTWINDOW, button);
    } else {
        float dholdTime;
        get_hold_time(&dholdTime);

        xdo_mouse_down(xdo, CURRENTWINDOW, button);
        usleep(dholdTime*1000);
        xdo_mouse_up(xdo, CURRENTWINDOW, button);
    }
}

void drag(float ix, float iy, float tx, float ty, int speedMS, int button, int screen) {
    float frac, loop, loops, x, y, posdx, posdy;
    float dx = tx - ix, dy = ty - iy;
    if (dx < 0 || dy < 0) {
        posdx = (dx < 0) ? dx - dx * 2 : dx;
        posdy = (dy < 0) ? dy - dy * 2 : dy;
        loops = (posdx < posdy) ? posdy : posdx;
    } else {
        loops = (dx < dy) ? dy : dx;
    }

    xdo_move_mouse(xdo, ix, iy, screen);
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    for (loop = 1; loop <= loops; loop++) {
        frac = loop / loops;
        x = ix + (frac * dx);
        y = iy + (frac * dy);
        xdo_move_mouse(xdo, x, y, screen);
        usleep(speedMS*1000);
    }
    xdo_mouse_up(xdo, CURRENTWINDOW, button);
}

void get_pixel_color(char color[]) {
    FILE *cmd = popen("getpixelcolor", "r");
    fgets(color, 256, cmd);
    fclose(cmd);
}

int get_hold_time(float *holdTime) {
    if (access("/tmp/presslog", F_OK) != 0) {
        return 1;
    } else {
        char buf, repeatRate[24], repeatDelay[24];
        float logLines;

        FILE *file = fopen("/tmp/presslog", "r");
        while ((buf = getc(file)) != EOF)
            logLines = (buf == '\n') ? logLines + 1 : logLines;
        fclose(file);
        remove("/tmp/presslog");

        FILE *frepeatDelay = popen("xset -q | awk NR==9 | awk '{print $4}'", "r");
        fgets(repeatDelay, 24, frepeatDelay);
        fclose(frepeatDelay);

        FILE *frepeatRate = popen("xset -q | awk NR==9 | awk '{print $7}'", "r");
        fgets(repeatRate, 24, frepeatRate);
        fclose(frepeatRate);

        *holdTime = logLines / atof(repeatRate) * 1000 + atof(repeatDelay);
        return 0;
    }
}

void print_help(void) {
    puts("mouse [click {button}]");
    puts("      [clickc {x,y,button,screen}]");
    puts("      [clickcr {x,y,button,screen}]");
    puts("      [move {x,y,screen}]");
    puts("      [move-rel {x,y}]");
    puts("      [move-smooth {x,y,speed,screen}]");
    puts("      [hold {holdtime,button}]");
    puts("      [drag {ix,iy,tx,ty,speedMS,button,screen}]");
    puts("      [get {x,y,xy,holdtime,pixelcolor}]");
}


int main(int argc, char **argv) {
    if (argc == 1) {
        print_help();
        return 1;
    }

    xdo = xdo_new(NULL);
    char action[32];
    strcpy(action, argv[1]);

    if (strcmp(action, "click") == 0)
    {
        if (argc < 2) {
            print_help();
            return 1;
        }
        int button = (argc > 2) ? atoi(argv[2]) : 1;
        click(button);
    }

    else if (strcmp(action, "clickc") == 0 || strcmp(action, "clickcr") == 0)
    {
        if (argc < 4) {
            print_help();
            return 1;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        int button = (argc > 4) ? atoi(argv[4]) : 1;
        int screen = (argc > 5) ? atoi(argv[5]) : 0;
        if (strcmp(action, "clickcr") == 0) {
            click_cords_return(x, y, button, screen);
        } else {
            click_cords(x, y, button, screen);
        }
    }

    else if (strcmp(action, "hold") == 0) {
        int holdTime = (argc > 2) ? atoi(argv[2]) : 0;
        int button = (argc > 3) ? atoi(argv[3]) : 1;
        hold(holdTime, button);
    }

    else if (strcmp(action, "drag") == 0) {
        if (argc < 5) {
            print_help();
            return 1;
        }
        int initalX = atoi(argv[2]);
        int initalY = atoi(argv[3]);
        int targetX = atoi(argv[4]);
        int targetY = atoi(argv[5]);
        int speedMS = (argc > 6) ? atoi(argv[6]) : 1;
        int button = (argc > 7) ? atoi(argv[7]) : 1;
        int screen = (argc > 8) ? atoi(argv[8]) : 0;
        drag(initalX, initalY, targetX, targetY, speedMS, button, screen);
    }

    else if (strcmp(action, "move") == 0)
    {
        if (argc != 4) {
            print_help();
            return 1;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        int screen = (argc > 4) ? atoi(argv[4]) : 0;
        move(x, y, screen);
    }

    else if (strcmp(action, "move-rel") == 0)
    {
        if (argc != 4) {
            print_help();
            return 1;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        move_rel(x, y);
    }

    else if (strcmp(action, "move-smooth") == 0)
    {
        if (argc < 3) {
            print_help();
            return 1;
        }

        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        int speedMS = (argc > 4) ? atoi(argv[4]) : 1;
        int screen = (argc > 5) ? atoi(argv[5]) : 0;
        move_smooth(x, y, speedMS, screen);
    }

    else if (strcmp(action, "get") == 0)
    {
        if (argc != 3) {
            print_help();
            return 1;
        }

        int x, y, screen;
        get_coords(&x, &y, &screen);

        if (strcmp(argv[2], "x") == 0) {
            printf("%d", x);
        } else if (strcmp(argv[2], "y") == 0) {
            printf("%d", y);
        } else if (strcmp(argv[2], "xy") == 0) {
            printf("%d %d", x, y);
        } else if (strcmp(argv[2], "holdtime") == 0) {
            float holdTime;
            int success;
            success = get_hold_time(&holdTime);
            if (success != 0) {
                puts("'/tmp/presslog' does not exist.");
                return 1;
            } else {
                printf("%.0f", holdTime);
            }
        } else if (strcmp(argv[2], "pixelcolor") == 0) {
            char color[50];
            get_pixel_color(color);
            printf("%s", color);
        } else {
            print_help();
            return 1;
        }
    }

    else {
        print_help();
        return 1;
    }

    xdo_free(xdo);
}

