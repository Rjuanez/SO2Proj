#include <game.h>

void init_crosshair(struct crosshair* ch){
    ch-> x = 12;
    ch-> y = 40;
    ch-> crosshair[0] = ' ';
    ch-> crosshair[1] = '|';
    ch-> crosshair[2] = ' ';
    ch-> crosshair[3] = '-';
    ch-> crosshair[4] = '*';
    ch-> crosshair[5] = '-';
    ch-> crosshair[6] = ' ';
    ch-> crosshair[7] = '|';
    ch-> crosshair[8] = ' ';
}

int crosshair_move_up(struct crosshair* ch){
    if (ch->y == 0) return -1;
    else ch->y--;
    return 0;
}