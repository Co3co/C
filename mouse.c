#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdo.h>

int
main(int argc, char **argv)
{
    if (argc == 1) {
        printf("usage: mouse [click] [move {x,y,screen}] [move-rel {x,y}] [get{x,y,xy}] [getpixelcolor]");
        return 0;
    }

    xdo_t *xdo = xdo_new(NULL);
    charcodemap_t *mods;
    int nmods;
    char action[32];
    strcpy(action, argv[1]);

    if (strcmp(action, "click") == 0)
    {
        xdo_get_active_modifiers(xdo, &mods, &nmods);
        xdo_clear_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);

        int button = (argc > 2) ? atoi(argv[2]) : 1;
        xdo_mouse_down(xdo, CURRENTWINDOW, button);
        xdo_mouse_up(xdo, CURRENTWINDOW, button);

        xdo_set_active_modifiers(xdo, CURRENTWINDOW, mods, nmods);
    }
    else if (strcmp(action, "move") == 0)
    {
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        int screen = (argc > 4) ? atoi(argv[4]) : 0;
        xdo_move_mouse(xdo, x, y, screen);
    }
    else if (strcmp(action, "move-rel") == 0)
    {
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        xdo_move_mouse_relative(xdo, x, y);
    }
    else if (strcmp(action, "getxy") == 0)
    {
        int x, y, screen;
        xdo_get_mouse_location(xdo, &x, &y, &screen);
        printf("%d %d", x, y);
    }
    else if (strcmp(action, "getx") == 0)
    {
        int x, y, screen;
        xdo_get_mouse_location(xdo, &x, &y, &screen);
        printf("%d", x);
    }
    else if (strcmp(action, "gety") == 0)
    {
        int x, y, screen;
        xdo_get_mouse_location(xdo, &x, &y, &screen);
        printf("%d", y);
    }
    else if (strcmp(action, "getpixelcolor") == 0)
    {
    system("eval $(xdotool getmouselocation --shell) ; xwd -root -silent |\
            convert xwd:- -depth 8 -crop \"1x1+$X+$Y\" txt:- | grep -om1 '#\\w\\+'");
    }
    xdo_free(xdo);
    return 0;
}

