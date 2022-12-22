//crosshair of 3x3
struct crosshair{
  char x;
  char y;
  char crosshair[9];
};

void init_crosshair(struct crosshair* ch);

int crosshair_move_up(struct crosshair* ch);