#include <libc.h>
#include <user_mm.h>
#include <game.h>

char buff[24];

char* screen_buff;

char s[1];

int pid;

void screen_callback(char *screen_buffer){
  unsigned int size = 25*80*2;
  unsigned int i;
  char* ptr = screen_buff;

  for(i = 0; i<size; i++)
    (*screen_buffer++) = (*ptr++);
  
  restore_ctx();
}

//inicialitzem la pantalla
void start_screen(char *screen_buffer){
  unsigned int i;
  unsigned int size = 25*80*2;
  for (i=0; i<size; i+=2){
    screen_buffer[i] = ' ';
    screen_buffer[i+1] = 0;
    }
}

void print_crosshair(struct crosshair* ch){
  char* cpy = &screen_buff[(ch->y*80*2)+ch->x*2];
  (*cpy++) = ch->crosshair;
  *cpy = (*cpy&0xF0)|0x0F;
}

void remove_crosshair(struct crosshair* ch){
  char* cpy = &screen_buff[(ch->y*80*2)+ch->x*2];
  (*cpy++) = ' ';
}

void print_square(struct square* sq){
  char* cpy = &screen_buff[(sq->y*80*2)+sq->x*2];
  int i, j;
  for (i=0; i<sq->size; i++)
    for (j=0; j<sq->size*2; j++){
      cpy[(i*80*2)+j*2] = ' ';
      cpy[(i*80*2)+j*2+1] = sq->color;
    }
}

void remove_square(struct square* sq){
  char* cpy = &screen_buff[(sq->y*80*2)+sq->x*2];
  int i, j;
  for (i=0; i<sq->size; i++)
    for (j=0; j<sq->size*2; j++){
      cpy[(i*80*2)+j*2] = ' ';
      cpy[(i*80*2)+j*2+1] = 0x00;
    }
}

void print_text(int x, int y, char* text, int size){
  char* cpy = &screen_buff[(x*80*2)+y*2];
  for (int i = 0; i<size*2; i+=2){
    cpy[i] = text[i/2];
    cpy[i+1] = 0x0F;
  }
}


int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  unsigned int size = 25*80*2;
  unsigned int i;
  screen_buff = get_big();

  start_screen(screen_buff);
  set_screen_callback(&screen_callback);

  //DEIXEM COMENTADA UNA VERSIO AMB MULTITHREAD PERO AMB BLINKING

// int fk = fork();

// if (fk ==0){
//   struct crosshair* ch = (struct crosshair*) get_small();
//   init_crosshair(ch);
//   while(1){
//     get_key(s,0);
//     remove_crosshair(ch);
//     switch (*s)
//     {
//       case 'w':
//       crosshair_move_up(ch);
//       *s = ' ';
//       break;

//       case 's':
//       crosshair_move_down(ch);
//       *s = ' ';
//       break;

//       case 'a':
//       crosshair_move_left(ch);
//       *s = ' ';
//       break;

//       case 'd':
//       crosshair_move_right(ch);
//       *s = ' ';
//       break;
    
//     default:
//       break;
//     }
//     print_crosshair(ch);
//   }
// }

// fk = fork();

// if (fk == 0){
//     struct square* sq = (struct square*) get_small();
//     init_square(sq,0,0,2);
//     while(1) print_square(sq);
// }

  struct square* sq_array[4];

  struct square* sq = (struct square*) get_small();
  init_square(sq,0,0,4,0x60,1,1);
  sq_array[0] = sq;

  sq = (struct square*) get_small();
  init_square(sq,23,18,2,0x60,-1,1);
  sq_array[1] = sq;

  sq = (struct square*) get_small();
  init_square(sq,60,12,3,0x30,1,-1);
  sq_array[2] = sq;

  sq = (struct square*) get_small();
  init_square(sq,54,21,2,0x20,-1,1);
  sq_array[3] = sq;

  int it = 0;

  struct crosshair* ch = (struct crosshair*) get_small();
  init_crosshair(ch);

  int endgame = 0;
  int score = 0;

  while(1) {
    get_key(s,0);
   print_text(1,34,"score:",strlen("score:"));
   itoa(score,buff);
   print_text(1,41,buff,strlen(buff));
    switch (*s)
    {
      case 'w':
      remove_crosshair(ch);
      crosshair_move_up(ch);
      *s = ' ';
      break;

      case 's':
      remove_crosshair(ch);
      crosshair_move_down(ch);
      *s = ' ';
      break;

      case 'a':
      remove_crosshair(ch);
      crosshair_move_left(ch);
      *s = ' ';
      break;

      case 'd':
      remove_crosshair(ch);
      crosshair_move_right(ch);
      *s = ' ';
      break;

      case 'f':
      if (shot_square(ch,sq)){
        remove_square(sq);
        free_small(sq);
        if (it<3){
          score++;
          sq = sq_array[it++];
          }
        else endgame = 1;
      }
      break;
    
    default:
      break;
    }
    if (endgame) break;
    move_square(sq);
    print_square(sq);
    print_crosshair(ch);

   }
   char* aux = screen_buff;
   free_big(aux);
   screen_buff= get_big();
   start_screen(screen_buff);
   print_text(12,34,"game over",strlen("game over"));
   while(1){
   }
}
