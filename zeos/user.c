#include <libc.h>

char buff[24];

char screen_buff[25][80][2];

int pid;

void screen_callback(char *screen_buffer){
  unsigned int size = 25*80*2;
  unsigned int i;
  char* ptr = screen_buff;

  for(i = 0; i<size; i++)
    (*screen_buffer++) = (*ptr++);
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  unsigned int size = 25*80*2;
  unsigned int i;
  char* ptr = screen_buff;
  for(i = 0; i<size; i++)
    *ptr++ = 'c';
  /*
  DEBUUUUG
  write(1, screen, size);
  void (*ptr_void)();
  itoa(errno,buff);
  write(1,buff,strlen(buff));
  */

 int a =set_screen_callback(&screen_callback);
  
  itoa(a,&buff);
  write(1,buff, strlen(buff));

// char* big_mem = get_big();
// for(i = 0; i<size; i++)
//     *ptr++ = 'd';
// //   write(1,"\n",1);
// screen_buff=big_mem;

//   char* small_mem = get_small();

//   itoa((int)small_mem,buff);
//  write(1,buff,strlen(buff));
//   write(1,"\n",1);

//   char* small_mem1 = get_small();

//     itoa((int)small_mem1,buff);
//  write(1,buff,strlen(buff));
//   write(1,"\n",1);

//     char* small_mem2 = free_small(small_mem1);

//     itoa((int)small_mem2,buff);
//  write(1,buff,strlen(buff));
//   write(1,"\n",1);

//   small_mem1 = get_small();

//     itoa((int)small_mem1,buff);
//  write(1,buff,strlen(buff));
//   write(1,"\n",1);



// write(1,"\n",1);

int fk = fork();

//   itoa((int)get_big(),buff);
//  write(1,buff,strlen(buff));

// write(1,"\n",1);

//   if(fk<0){
//       itoa(errno,buff);
//       write(1,buff,strlen(buff));
//       write(1,"\n",1);
//   }else{
//     itoa(fk,buff);
//       write(1,buff,strlen(buff));
//       write(1,"\n",1);
//   }
// if (fk==0)exit();

 

//   write(1,"llegaGet",8);

//   int pruebaGet = (int)get_big();
//   itoa(pruebaGet,buff);
//   write(1,buff,strlen(buff));

// write(1,"llegaGet",8);
// int prueba = free_big(buff);
//   itoa(prueba,buff);
//   write(1,buff,strlen(buff));

  while(1) {
   }
}
