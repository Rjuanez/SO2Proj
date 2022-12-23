#include <game.h>

void init_crosshair(struct crosshair* ch){
    ch-> x = 40;
    ch-> y = 12;
    ch-> crosshair = '+';
}

int crosshair_move_up(struct crosshair* ch){
    if (ch->y == 0) return -1;
    else ch->y--;
    return 0;
}

int crosshair_move_down(struct crosshair* ch){
    if (ch->y == 24) return -1;
    else ch->y++;
    return 0;
}

int crosshair_move_left(struct crosshair* ch){
    if (ch->x == 0) return -1;
    else ch->x--;
    return 0;
}

int crosshair_move_right(struct crosshair* ch){
    if (ch->x == 79) return -1;
    else ch->x++;
    return 0;
}

void init_square(struct square* sq, int x, int y, int size, char color, int x_dir, int y_dir){
    sq->x = x;
    sq->y = y;
    sq->size = size;
    sq->color = color;
    sq ->dir_vec[0] = x_dir;
    sq ->dir_vec[1] = y_dir;
}

void move_square(struct square* sq){
    sq->x += sq->dir_vec[0];
    sq->y += sq->dir_vec[1];
    if ((sq->x<0 || sq->x+sq->size*2)>=79){
        sq->dir_vec[0]=-sq->dir_vec[0];
        move_square(sq);

    }else if (sq->y<0 || (sq->y+sq->size)>= 24){
        sq->dir_vec[1]=-sq->dir_vec[1];
        move_square(sq);
    }
}