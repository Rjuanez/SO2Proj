#include<kernel_mm.h>


void copy_big(struct Big_Memory_Managment* original,struct Big_Memory_Managment* copy){
    copy->big_mem_it=original->big_mem_it;
    char*  original_ptr= (char*) original->big_mem;
    char* copy_ptr = (char*) copy->big_mem;
    for(int i =0; i<NUM_PAG_BIG_MEM;i++)
        *copy_ptr++ = *original_ptr++;
}

int get_big_ptr(struct Big_Memory_Managment* bm){
    for(int i =0; i<NUM_PAG_BIG_MEM && bm->big_mem[bm->big_mem_it] != 0; i++)
        if(++(bm->big_mem_it)==NUM_PAG_BIG_MEM) bm->big_mem_it=0; 
    if (bm->big_mem[bm->big_mem_it] == 1) return -1;
    bm->big_mem[bm->big_mem_it] = 1;
    return bm->big_mem_it;
}

void free_big_space(struct Big_Memory_Managment* bm, int free_ptr){
    bm->big_mem[free_ptr] = 0;
}