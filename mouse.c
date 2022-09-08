#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdo.h>

xdo_t *xdo;
charcodemap_t *mods;
int nmods;

void click(int button, int clearmods);
void click_coords(int x, int y, int button, int screen, int clearmods);
void click_coords_return(int x, int y, int button, int screen, int clearmods);
void move(int x, int y, int screen, int clearmods);
void move_relevant(int x, int y, int clearmods);
void move_smooth(float tx, float ty, int speedMS, int screen, int clearmods);
void hold(int holdTime, int button, int clearmods);
void drag(float ix, float iy, float tx, float ty, int speedMS, int button, int screen, int clearmods);
void get_coords(int *x, int *y, int *screen);
void get_coords_click(int *x, int *y, int *screen, int clearmods);
void get_pixel_color(char color[]);
int get_hold_time(float *holdTime);
int help_check(int argc, char **argv);
void print_help(void);

void click(int button, int clearmods) {
    if (clearmods == 1) 
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    if (clearmods == 1) 
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void click_coords(int x, int y, int button, int screen, int clearmods) {
    if (clearmods == 1)
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_move_mouse(xdo, x, y, screen);
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    if (clearmods == 1)
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void click_coords_return(int x, int y, int button, int screen, int clearmods) {
    if (clearmods == 1)
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    int startX, startY, startScreen;
    xdo_get_mouse_location(xdo, &startX, &startY, &startScreen);

    xdo_move_mouse(xdo, x, y, screen);
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    xdo_move_mouse(xdo, startX, startY, screen);

    if (clearmods == 1)
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void move(int x, int y, int screen, int clearmods) {
    if (clearmods == 1) 
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_move_mouse(xdo, x, y, screen);

    if (clearmods == 1) 
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void move_relevant(int x, int y, int clearmods) {
    if (clearmods == 1) 
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_move_mouse_relative(xdo, x, y);

    if (clearmods == 1) 
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void move_smooth(float tx, float ty, int speedMS, int screen, int clearmods) {
    if (clearmods == 1) 
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

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

    if (clearmods == 1) 
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void hold(int holdTime, int button, int clearmods) {
    if (clearmods == 1) 
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

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

    if (clearmods == 1) 
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void drag(float ix, float iy, float tx, float ty, int speedMS, int button, int screen, int clearmods) {
    if (clearmods == 1) 
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

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

    if (clearmods == 1) 
        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}
void get_coords(int *x, int *y, int *screen) {
    xdo_get_mouse_location(xdo, x, y, screen);
}
void get_coords_click(int *x, int *y, int *screen, int clearmods) {
    xdo_get_mouse_location(xdo, x, y, screen);
}
void get_pixel_color(char color[]) {
    FILE *cmd = popen("getpixelcolor", "r");
    fgets(color, 256, cmd);
    pclose(cmd);
}
int get_hold_time(float *holdTime) {
    if (access("/tmp/presslog", F_OK) != 0) {
        return 1;
    } else {
        char buf, repeatRate[24], repeatDelay[24];
        float logLines;
        logLines = 0;

        FILE *file = fopen("/tmp/presslog", "r");
        while ((buf = getc(file)) != EOF)
            logLines = (buf == '\n') ? logLines + 1 : logLines;
        fclose(file);
        remove("/tmp/presslog");

        FILE *frepeatDelay = popen("xset -q | awk NR==9 | awk '{print $4}'", "r");
        fgets(repeatDelay, 24, frepeatDelay);
        pclose(frepeatDelay);

        FILE *frepeatRate = popen("xset -q | awk NR==9 | awk '{print $7}'", "r");
        fgets(repeatRate, 24, frepeatRate);
        pclose(frepeatRate);

        *holdTime = logLines / atof(repeatRate) * 1000 + atof(repeatDelay);
        return 0;
    }
}
void print_help(void) {
    puts("mouse [-c] [-r repeats] <action>");
    puts("      -c clear-modifiers");
    puts("          clear all active key modifiers before acting");
    puts("      -r repeat");
    puts("          repeat the action a specified amount of times");
    puts("actions:");
    puts("      [click {button}]");
    puts("      [clickc {x,y,button,screen}]");
    puts("      [clickcr {x,y,button,screen}]");
    puts("      [move {x,y,screen}]");
    puts("      [mover {x,y}]");
    puts("      [moves {x,y,speed,screen}]");
    puts("      [hold {time,button}]");
    puts("      [drag {ix,iy,tx,ty,interval,button,screen}]");
    puts("      [get {x,y,xy,holdtime,pixelcolor}]\n");
    exit(1);
}

int main(int argc, char **argv) {
    if (argc == 1)
        print_help();

    xdo = xdo_new(NULL);
    xdo_get_active_modifiers(xdo, &mods, &nmods);

    char *options = "cr:h";
    char opt;
    int clearmods = 0;
    int repeat = 1;

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch(opt) {
            case 'c':
                clearmods = 1;
                break;
            case 'r':
                repeat = atoi(optarg);
                break;
            case 'h':
                print_help();
                break;
        }
    }

    char actions[argc-optind][256];
    if (optind < argc)
        for (int a = 0; optind < argc; a++, optind++)
            strcpy(actions[a], argv[optind]);
    else
        print_help();

    int actionsc = sizeof(actions) / sizeof(actions[0]);
    
    for (int r = 0; r < repeat; r++) {
        if (strcmp(actions[0], "click") == 0)
        {
            int button = (actionsc > 1) ? atoi(actions[1]) : 1;

            click(button, clearmods);
        }
        else if (strcmp(actions[0], "clickc") == 0 || strcmp(actions[0], "clickcr") == 0)
        {
            if (actionsc < 3)
                print_help();

            int x = atoi(actions[1]);
            int y = atoi(actions[2]);
            int button = (actionsc > 3) ? atoi(actions[3]) : 1;
            int screen = (actionsc > 4) ? atoi(actions[4]) : 0;

            if (strcmp(actions[0], "clickcr") == 0)
                click_coords_return(x, y, button, screen, clearmods);
            else
                click_coords(x, y, button, screen, clearmods);
        }
        else if (strcmp(actions[0], "hold") == 0)
        {
            int holdTime = (actionsc > 1) ? atoi(actions[1]) : 0;
            int button = (actionsc > 2) ? atoi(actions[2]) : 1;

            hold(holdTime, button, clearmods);
        }
        else if (strcmp(actions[0], "drag") == 0)
        {
            int initalX = atoi(actions[1]);
            int initalY = atoi(actions[2]);
            int targetX = atoi(actions[3]);
            int targetY = atoi(actions[4]);
            int speedMS = (actionsc > 5) ? atoi(actions[5]) : 1;
            int button = (actionsc > 6) ? atoi(actions[6]) : 1;
            int screen = (actionsc > 7) ? atoi(actions[7]) : 0;

            drag(initalX, initalY, targetX, targetY, speedMS, button, screen, clearmods);
        }
        else if (strcmp(actions[0], "move") == 0 || strcmp(actions[0], "mover") == 0 || strcmp(actions[0], "moves") == 0)
        {
            if (actionsc < 3)
                print_help();

            int x, y, screen, speedMS;
            x = atoi(actions[1]);
            y = atoi(actions[2]);

            screen = (actionsc > 3 && (strcmp(actions[0], "move") == 0 || strcmp(actions[0], "mover") == 0)) ? atoi(actions[3]) : 0;
            speedMS = (actionsc > 3 && strcmp(actions[0], "moves") == 0) ? atoi(actions[3]) : 1;
            screen = (actionsc > 4 && strcmp(actions[0], "moves") == 0) ? atoi(actions[4]) : 0;

            printf("%d %d\n", speedMS, screen);
            if (strcmp(actions[0], "move") == 0)
                move(x, y, screen, clearmods);
            else if (strcmp(actions[0], "mover") == 0)
                move_relevant(x, y, clearmods);
            else
                move_smooth(x, y, speedMS, screen, clearmods);
        }
        else if (strcmp(actions[0], "get") == 0)
        {
            int x, y, screen;
            get_coords(&x, &y, &screen);

            if (strcmp(actions[1], "x") == 0)
                printf("%d", x);
            else if (strcmp(actions[1], "y") == 0)
                printf("%d", y);
            else if (strcmp(actions[1], "xy") == 0)
                printf("%d %d", x, y);
            else if (strcmp(actions[1], "ht") == 0 || strcmp(actions[1], "holdtime") == 0) {
                float holdTime;
                int success;
                success = get_hold_time(&holdTime);
                if (success != 0) {
                    puts("'/tmp/presslog' does not exist.");
                    return 1;
                } else
                    printf("%.0f", holdTime);
            } else if (strcmp(actions[1], "pc") == 0 || strcmp(actions[1], "pixelcolor") == 0) {
                char color[50];
                get_pixel_color(color);
                printf("%s", color);
            } else
                print_help();
        }
        else
            print_help();
    }

    return 0;
}
