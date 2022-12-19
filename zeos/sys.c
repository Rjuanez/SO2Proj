/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <p_stats.h>

#include <errno.h>

#include <kernel_mm.h>

#define LECTURA 0
#define ESCRIPTURA 1

void * get_ebp();

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; 
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
	return -ENOSYS; 
}

int sys_getpid()
{
	return current()->PID;
}

int global_PID=1000;

int ret_from_fork()
{
  return 0;
}

int sys_fork(void)
{
  struct list_head *lhcurrent = NULL;
  
  union task_union *uchild;
  struct Big_Memory_Managment* bmm = (struct Big_Memory_Managment*) (PAG_LOG_BIG_MEM_MANAGMENT<<12);
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);

  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  /* new pages dir */
  allocate_DIR((struct task_struct*)uchild);

  /* Allocate pages for Big memory managment*/
  int new_ph_pag, pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);

  new_ph_pag=alloc_frame();
  if(new_ph_pag==-1)return -EAGAIN;
  set_ss_pag(process_PT, PAG_LOG_BIG_MEM_MANAGMENT, new_ph_pag);

  /* Allocate pages for DATA+STACK */
  for (pag=0; pag<NUM_PAG_DATA; pag++)
  {
    new_ph_pag=alloc_frame();
    if (new_ph_pag!=-1) /* One page allocated */
    {
      set_ss_pag(process_PT, PAG_LOG_INIT_DATA+pag, new_ph_pag);
    }
    else /* No more free pages left. Deallocate everything */
    {
      /* Deallocate allocated pages. Up to pag.Even allocated for bmm */
      free_frame(get_frame(process_PT, PAG_LOG_BIG_MEM_MANAGMENT));
      del_ss_pag(process_PT, PAG_LOG_BIG_MEM_MANAGMENT);
      for (i=0; i<pag; i++)
      {
        free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
        del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
      }
      /* Deallocate task_struct */
      list_add_tail(lhcurrent, &freequeue);
      
      /* Return error */
      return -EAGAIN; 
    }
  }

    /* Allocate pages for HEAP */
  for (pag=0; pag<NUM_PAG_BIG_MEM; pag++)
  {
    if(bmm->big_mem[pag]){
      new_ph_pag=alloc_frame();
      if (new_ph_pag!=-1) /* One page allocated */
      {
        set_ss_pag(process_PT, PAG_LOG_INIT_BIG_MEM+pag, new_ph_pag);
      }
      else /* No more free pages left. Deallocate everything */
      {
        /* Deallocate allocated pages. Up to pag.Even allocated for bmm and DATA */
        free_frame(get_frame(process_PT, PAG_LOG_BIG_MEM_MANAGMENT));
        del_ss_pag(process_PT, PAG_LOG_BIG_MEM_MANAGMENT);
        for (i=0; i<NUM_PAG_DATA; i++)
        {
          free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
          del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
        }
        for (i=0; i<pag; i++)
        {
          free_frame(get_frame(process_PT, PAG_LOG_INIT_BIG_MEM+i));
          del_ss_pag(process_PT, PAG_LOG_INIT_BIG_MEM+i);
        }
        /* Deallocate task_struct */
        list_add_tail(lhcurrent, &freequeue);
        
        /* Return error */
        return -EAGAIN; 
      }
    } 
  }

  /* Copy parent's SYSTEM and CODE to child. */
  page_table_entry *parent_PT = get_PT(current());
  for (pag=0; pag<NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag=0; pag<NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE+pag, get_frame(parent_PT, PAG_LOG_INIT_CODE+pag));
  }
  /* Copy parent's DATA to child. */
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
  {
    /* Map one child page to parent's address space. */
    set_ss_pag(parent_PT, pag+NUM_PAG_DATA, get_frame(process_PT, pag));
    copy_data((void*)(pag<<12), (void*)((pag+NUM_PAG_DATA)<<12), PAGE_SIZE);
    del_ss_pag(parent_PT, pag+NUM_PAG_DATA);
  }

  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  int cpy_counter=0;

  /* COPY MEMORY MANAGMENT PARENT TO CHILD */

  set_ss_pag(parent_PT, PAG_LOG_INIT_COPY_SPACE, get_frame(process_PT, PAG_LOG_BIG_MEM_MANAGMENT));
  copy_data((void*)(PAG_LOG_BIG_MEM_MANAGMENT<<12), (void*)(PAG_LOG_INIT_COPY_SPACE<<12), PAGE_SIZE);
  del_ss_pag(parent_PT, PAG_LOG_INIT_COPY_SPACE);
  cpy_counter++;

  /* Copy parent's HEAP to child. */
  for (pag=PAG_LOG_INIT_BIG_MEM; pag<TOTAL_PAGES; pag++)
  {
    //only if used
    if(bmm ->big_mem[pag-PAG_LOG_INIT_BIG_MEM]){
      /* Map one child page to parent's address space. */
      if (cpy_counter==20){
          /* Deny access to the child's memory space */
          set_cr3(get_DIR(current()));
          cpy_counter=0;
      }
      set_ss_pag(parent_PT, PAG_LOG_INIT_COPY_SPACE+cpy_counter, get_frame(process_PT, pag));
      copy_data((void*)(pag<<12), (void*)((PAG_LOG_INIT_COPY_SPACE+cpy_counter)<<12), PAGE_SIZE);
      del_ss_pag(parent_PT, PAG_LOG_INIT_COPY_SPACE+cpy_counter);
      cpy_counter++;
    }
  }
  
  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  uchild->task.PID=++global_PID;
  uchild->task.state=ST_READY;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes) {
char localbuffer [TAM_BUFFER];
int bytes_left;
int ret;

	if ((ret = check_fd(fd, ESCRIPTURA)))
		return ret;
	if (nbytes < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_READ, buffer, nbytes))
		return -EFAULT;
	
	bytes_left = nbytes;
	while (bytes_left > TAM_BUFFER) {
		copy_from_user(buffer, localbuffer, TAM_BUFFER);
		ret = sys_write_console(localbuffer, TAM_BUFFER);
		bytes_left-=ret;
		buffer+=ret;
	}
	if (bytes_left > 0) {
		copy_from_user(buffer, localbuffer,bytes_left);
		ret = sys_write_console(localbuffer, bytes_left);
		bytes_left-=ret;
	}
	return (nbytes-bytes_left);
}


extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}

void sys_exit()
{  
  int i;

  page_table_entry *process_PT = get_PT(current());
  struct Big_Memory_Managment* bmm = (struct Big_Memory_Managment*)(PAG_LOG_BIG_MEM_MANAGMENT<<12);

  // Deallocate all the propietary physical pages
  for (i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
  }

  //Deallocate heap memory
  for (i=0; i<NUM_PAG_BIG_MEM; i++)
  {
    if(bmm->big_mem[i]){
      free_frame(get_frame(process_PT, PAG_LOG_INIT_BIG_MEM+i));
      del_ss_pag(process_PT, PAG_LOG_INIT_BIG_MEM+i);
    }
  }

  //Deallocate BMM
  free_frame(get_frame(process_PT, PAG_LOG_BIG_MEM_MANAGMENT));
  del_ss_pag(process_PT, PAG_LOG_BIG_MEM_MANAGMENT);

  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->PID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st)
{
  int i;
  
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  
  if (pid<0) return -EINVAL;
  for (i=0; i<NR_TASKS; i++)
  {
    if (task[i].task.PID==pid)
    {
      task[i].task.p_stats.remaining_ticks=remaining_quantum;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}

int sys_get_key(char* c){
  unsigned char* ret;
  if ((ret = CIRCULAR_BUFFER_GET(&circularBuffer)) == (void*) 0){
    //Blocked, falta verificar!!!
    force_task_block();
    ret = CIRCULAR_BUFFER_GET(&circularBuffer);
  }
  *c = *ret;
  return 1;
}

unsigned char big_mem[NUM_PAG_BIG_MEM];
unsigned char big_mem_it=0;

int sys_set_screen_callback(void *(*callback_function)(char*)){
  if (callback_function == NULL) return -EINVAL;

  //ASSIGN SCREEN DEVICE TO USER ACCESIBLE PAGE
  current()->screen_callback_ptr = callback_function;
  return 0;
}

int sys_restore_ctx(){
  unsigned long* ctx_ptr = current()->context;
  unsigned long* stack_ptr = current();

    for (int i =0; i<17; i++){
    stack_ptr[KERNEL_STACK_SIZE-17+i] = ctx_ptr;
  }
}

//JUST LOOK FOR FREE SPACE
char* sys_get_big(){
  struct Big_Memory_Managment* bmm = (struct Big_Memory_Managment*) (PAG_LOG_BIG_MEM_MANAGMENT<<12);
  int free_ptr = get_big_ptr(bmm);
  //NO SPACE
  if(free_ptr == -1) return (void*)0;
  //SAVE MEMORY PAGE
  int new_ph_pag = alloc_frame();
  if (new_ph_pag == -1) return (void*)0;

  set_ss_pag(get_PT(current()), PAG_LOG_INIT_BIG_MEM+free_ptr, new_ph_pag);
  char* big_page = (char*)((PAG_LOG_INIT_BIG_MEM+free_ptr)<<12);
  
  return big_page;
}

int sys_free_big(char *s){
  struct Big_Memory_Managment* bmm = (struct Big_Memory_Managment*) (PAG_LOG_BIG_MEM_MANAGMENT<<12);
  int free_ptr = (((unsigned int) s)>>12);

  free_frame(get_frame(get_PT(current()),free_ptr));
  del_ss_pag(get_PT(current()), free_ptr);
  free_big_space(bmm, free_ptr-PAG_LOG_INIT_BIG_MEM);

  return 0;
}

//JUST LOOK FOR FREE SPACE
char* sys_get_small(){
  struct Big_Memory_Managment* bmm = (struct Big_Memory_Managment*) (PAG_LOG_BIG_MEM_MANAGMENT<<12);
  struct list_head* lh;
  struct Small_Memory_Managment* smm;

//iterem
  list_for_each(lh, &bmm->used_mem){
    smm = (struct Small_Memory_Managment*) lh;
    unsigned char small_dir;
    for(int i =0; i<16;i++){
      if (smm->small_entry[i]!=-1){
        small_dir = (unsigned char) smm->small_entry[i];
        for(int j=0; j<8;j++){
          if(((small_dir>>j) & 0x01) == 0){
            return get_entry_small(smm,i,j);
          }
        }
      }
    }
  }
  //o totes ocupades o llista buida, reservem un nou big
  smm=(struct Small_Memory_Managment*) sys_get_big();
  add_big_for_small(bmm,smm);
  init_small(smm);
  //posicio 2 ja que la 1 la dediquem per emmagatzemar info
  return (((char*)smm) + 32);
}

int sys_free_small(char *s){
  unsigned int aux = ((unsigned int)s);
  struct Big_Memory_Managment* bmm = (struct Big_Memory_Managment*) (PAG_LOG_BIG_MEM_MANAGMENT<<12);
  struct Small_Memory_Managment* smm=(struct Small_Memory_Managment*) (aux&0xFFFFF000);
  int entry = (aux&0x00000FFF)>>5;
  free_small_space(bmm,smm,entry);
  return 0;
}