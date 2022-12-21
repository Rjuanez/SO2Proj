#include <mm_address.h>
#include <list.h>

struct Big_Memory_Managment{
    unsigned char big_mem[NUM_PAG_BIG_MEM]; //array para marcar si espacio libre/ocupado
    int big_mem_it;              //puntero a espacio posiblemente libre, para ahorrar iterar todas las posiciones(o intentarlo)
};


void init_big(struct Big_Memory_Managment* bmm);

void copy_big(struct Big_Memory_Managment* original,struct Big_Memory_Managment* copy);

int get_big_ptr(struct Big_Memory_Managment* bm);

void free_big_space(struct Big_Memory_Managment* bm, int free_ptr);
