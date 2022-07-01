#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdo.h>

xdo_t *xdo;
charcodemap_t *mods;
int nmods;

void click(int button);
void click_cords(int x, int y, int screen, int button);
void click_cords_return(int x, int y, int screen, int button);
void move(int x, int y, int screen);
void move_rel(int x, int y);
void get_coords(int *x, int *y, int *screen);
void hold(int pressTime, int button);
void drag(int x, int y, int targetX, int targetY, int speedMS, int screen, int button);
void get_pixel_color(char color[]);
int bigger(int val1, int val2);
int print_help(void);

void click(int button) {
    xdo_get_active_modifiers(xdo, &mods, &nmods);
    xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}

void click_cords(int x, int y, int screen, int button) {
    xdo_get_active_modifiers(xdo, &mods, &nmods);
    xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

    xdo_move_mouse(xdo, x, y, screen);
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);

    xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
}

void click_cords_return(int x, int y, int screen, int button) {
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

void get_coords(int *x, int *y, int *screen) {
    xdo_get_mouse_location(xdo, x, y, screen);
}

void hold(int pressTime, int button) {
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    usleep(pressTime*1000);
    xdo_mouse_up(xdo, CURRENTWINDOW, button);
}

void drag(int x, int y, int targetX, int targetY, int speedMS, int screen, int button) {
    xdo_mouse_down(xdo, CURRENTWINDOW, button);
    for (x, y; x < targetX || x > targetX && y < targetY || y > targetY;) {
        x = (x < targetX) ? x + 1 : x - 1;
        y = (y < targetY) ? y + 1 : y - 1;
        printf("%d, %d\n", x, y);
        xdo_move_mouse(xdo, x, y, screen);
        usleep(speedMS*1000);
    }
    for (y; y < targetY || y > targetY;) {
        y = (y < targetY) ? y + 1 : y - 1;
        printf("%d\n", y);
    }
    xdo_mouse_up(xdo, CURRENTWINDOW, button);
}

void get_pixel_color(char color[]) {
    FILE *cmd = popen("eval $(xdotool getmouselocation --shell) ; xwd -root -silent | convert xwd:- -depth 8 -crop \"1x1+$X+$Y\" txt:- | grep -om1 '#\\w\\+'", "r");
    fgets(color, 256, cmd);
    fclose(cmd);
}

int print_help(void) {
    puts("usage: mouse [click {button}] [move {x,y,screen}] [move-rel {x,y}] [get {x,y,xy}] [getpixelcolor]");
    return 0;
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
        if (argc != 2) {
            print_help();
            return 1;
        }
        int button = (argc > 2) ? atoi(argv[2]) : 1;
        click(button);
    }

    else if (strcmp(action, "clickc") == 0 || strcmp(action, "clickcr") == 0)
    {
        if (argc != 4) {
            print_help();
            return 1;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        int button = (argc > 4) ? atoi(argv[4]) : 1;
        int screen = (argc > 5) ? atoi(argv[5]) : 0;
        if (strcmp(action, "clickcr") == 0) {
            click_cords_return(x, y, screen, button);
        } else {
            click_cords(x, y, screen, button);
        }
    }

    else if (strcmp(action, "hold") == 0) {
        if (argc != 3) {
            print_help();
            return 1;
        }
        int pressTime = atoi(argv[2]);
        int button = (argc > 4) ? atoi(argv[4]) : 1;
        hold(pressTime, button);
    }

    else if (strcmp(action, "drag") == 0) {
        if (argc < 5) {
            print_help();
            return 1;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        int targetX = atoi(argv[4]);
        int targetY = atoi(argv[5]);
        int speedMS = (argc > 6) ? atoi(argv[6]) : 1;
        int button = (argc > 7) ? atoi(argv[7]) : 1;
        int screen = (argc > 8) ? atoi(argv[8]) : 0;
        drag(x, y, targetX, targetY, speedMS, screen, button);
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
        } else {
            printf("%d %d", x, y);
        }
    }

    else if (strcmp(action, "getpixelcolor") == 0)
    {
        char color[50];
        get_pixel_color(color);
        printf("%s", color);
    }

    else {
        print_help();
        return 1;
    }

    xdo_free(xdo);
}

