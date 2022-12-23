#include <user_mm.h>

struct Small_Memory_Managment* first_small;
#define nullptr (void*)0

char buff[32];

//SMALL FUNCTIONS

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

//JUST LOOK FOR FREE SPACE
char* get_small(){
    //si no tenim cap big asociat
  if (first_small == nullptr){
    first_small = (struct Small_Memory_Managment*) get_big();
    init_small(first_small);
    //posicio 2 ja que la 1 la dediquem per emmagatzemar info
    return (((char*)first_small) + 32);
  }
    //iterem
    struct Small_Memory_Managment* smm = first_small;
    unsigned char small_dir;
    
    unsigned char i;
    struct Small_Memory_Managment* smm_aux;
    while(smm != (void*)0){
        for(i =0; i<16;i++){
            //si -1 significa tos a 1 perque es ca2
            if (smm->small_entry[i]!=-1){
            //agafem el char
            small_dir = (unsigned char) smm->small_entry[i];
            unsigned char j;
            for(j=0; j<8;j++){
                if(((small_dir>>j) & 0x01) == 0){
                return get_entry_small(smm,i,j);
                }
            }
            }
        }
    smm_aux = smm;
    smm = smm->next_big;
    }
    //si esta ple reservem un nou big
    struct Small_Memory_Managment* new_smm = (struct Small_Memory_Managment*)get_big();
    init_small(new_smm);
    smm_aux ->next_big = new_smm;
    new_smm ->prev_big = smm_aux;
    return (((char*)new_smm) + 32);
}

void free_small_entry(struct Small_Memory_Managment* smm, int entry){
    smm ->size--;
    if (smm->size==0){
        //si init, prev a null
        if (smm ->prev_big == nullptr){
            if(smm ->next_big != nullptr)
                first_small = smm -> next_big;
            else first_small = nullptr;
        }else if(smm ->next_big != nullptr){
            smm->prev_big ->next_big = smm->next_big;
            smm ->next_big->prev_big = smm ->prev_big;
        }else smm ->prev_big ->next_big = nullptr;
        free_big((char*) smm);
    }else{
        int i = entry/8;
        //entry % 8
        int j = entry & 0x3;
        //poner a 0 entrada correspondiente
        smm->small_entry[i]=smm->small_entry[i]&(~(0x01<<j));
    }
}

extern errno;

int free_small(char *s){
  unsigned int aux = ((unsigned int)s);
  //no aligned con 32 bytes
  itoa(aux&0x0000001F,buff);
  
  if ((aux&0x0000001F) != 0){
    errno = 14;
    return -1;
  }
  struct Small_Memory_Managment* smm=(struct Small_Memory_Managment*) (aux&0xFFFFF000);
  //miramos si la pagina big esta reservada
  struct Small_Memory_Managment* aux_smm = first_small;
  unsigned char found = 0;
  while(aux_smm != (void*)0){
    if (aux_smm == smm) found = 14;
    aux_smm = aux_smm ->next_big;
  }
  if (!found){
    errno = 1;
    return -1;
  }
  int entry = (aux&0x00000FFF)>>5;
  int i = entry/8;
  //entry % 8
  int j = entry & 0x3;
  //la entrada no estava reservada
  if (smm->small_entry[i]&(0x01<<j)==0){
    errno = 14;
    return -1;
  }
  free_small_entry(smm,entry);
  return 0;
}