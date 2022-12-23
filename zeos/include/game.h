//crosshair of 3x3
struct crosshair{
  unsigned int x;
  unsigned int y;
  char crosshair;
};

void init_crosshair(struct crosshair* ch);

int crosshair_move_up(struct crosshair* ch);

int crosshair_move_down(struct crosshair* ch);

int crosshair_move_left(struct crosshair* ch);

int crosshair_move_right(struct crosshair* ch);

struct square{
  int x;
  int y;
  unsigned int size;
  unsigned int dir_vec[2];
  char color;
  unsigned int time;
};

void init_square(struct square* sq, int x, int y, int size,char color, int x_dir, int y_dir);

void move_square(struct square* sq);

int shot_square(struct crosshair* ch, struct square* sq);