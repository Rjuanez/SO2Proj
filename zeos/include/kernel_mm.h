#include <mm_address.h>
#include <list.h>

struct Big_Memory_Managment{
    struct list_head used_mem;              //list of memory pages used for allocation of smalls
    unsigned char big_mem[NUM_PAG_BIG_MEM]; //array para marcar si espacio libre/ocupado
    int big_mem_it;              //puntero a espacio posiblemente libre, para ahorrar iterar todas las posiciones(o intentarlo)
};

//struct for small header
struct Small_Memory_Managment{
  struct list_head used; //anchor for used big spaces destinated for small allocations
  int size; //number of smalls used
  char small_entry[16]; //in 4096B space it enters 128 32B spaces and 16*8 = 128. (BitMask de 0 libre 1 ocupada)
};

void init_big(struct Big_Memory_Managment* bmm);

void copy_big(struct Big_Memory_Managment* original,struct Big_Memory_Managment* copy);

int get_big_ptr(struct Big_Memory_Managment* bm);

void free_big_space(struct Big_Memory_Managment* bm, int free_ptr);

//FUNCTIONS FOR SMALL

void add_big_for_small(struct Big_Memory_Managment* bmm, struct Small_Memory_Managment* smm);

void init_small(struct Small_Memory_Managment* smm);

char* get_entry_small(struct Small_Memory_Managment* smm, int i, int j);

void free_small_space(struct Big_Memory_Managment* bmm, struct Small_Memory_Managment* smm, int entry);