#include<kernel_mm.h>


void init_big(struct Big_Memory_Managment* bmm){
    INIT_LIST_HEAD(&bmm->used_mem);
}

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

//SMALL FUNCTIONS

void add_big_for_small(struct Big_Memory_Managment* bmm, struct Small_Memory_Managment* smm){
    list_add(&smm->used,&bmm->used_mem);
}

void init_small(struct Small_Memory_Managment* smm){
    //primera header i segona marquem com utilitzada
    smm->small_entry[0]=0x03;
    smm->size=1;
}

char* get_entry_small(struct Small_Memory_Managment* smm, int i, int j){
    smm ->size++;
    //poner a 1 entrada correspondiente
    smm->small_entry[i]=smm->small_entry[i] | (0x01<<j);
    char* ret = (char*) smm;
    return ret + (((i<<3)+j)<<5);
}

void free_small_space(struct Big_Memory_Managment* bmm, struct Small_Memory_Managment* smm, int entry){
    smm ->size--;
    if (smm->size==0){
        list_del(&smm->used);
        int free_ptr = ((unsigned int)(smm)>>12) - ((unsigned int)(bmm)>>12);
        free_big_space(bmm, free_ptr);
    }else{
        int i = entry/8;
        //entry % 8
        int j = entry & 0x3;
        //poner a 0 entrada correspondiente
        smm->small_entry[i]=smm->small_entry[i]&(~(0x01<<j));
    }
}