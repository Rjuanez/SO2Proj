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
  char* cpy = &screen_buff[(ch->x*80*2)+ch->y*2];
  for(int i =0; i<18;i+=2){
    cpy[i]= ch->crosshair[i];
    cpy[i+1] = '^';
    if (i%3 ==2) cpy+=160-6;
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

// write(1,"\n",1);
// char* free;
// for (int i =0; i<129; i++){
//   int b = (int) get_small();
//   write(1, "\n",1);
//   itoa(i,buff);
//   write(1, buff, strlen(buff));
//   write (1, " ", 1);
//   itoa(b, buff);
//   write(1, buff, strlen(buff));
//    free = (char*) b;
// }

int fk = fork();

if (fk ==0){
  struct crosshair* ch = (struct crosshair*) get_small();
  init_crosshair(ch);
  while(1){
    get_key(s,1);
    write(1,"telca",strlen("tecla"));
    itoa(ch->x,buff);
    write(1,buff,strlen(buff));
    print_crosshair(ch);
  }
}

exit();

  while(1) {
   }
}
