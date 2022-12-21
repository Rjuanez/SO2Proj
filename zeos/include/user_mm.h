//struct for small header
struct Small_Memory_Managment{
  struct Small_Memory_Managment* next_big; //punter a next small utilitzat
  struct Small_Memory_Managment* prev_big; //punter a prev small utilitzat
  int size; //number of smalls used
  char small_entry[16]; //in 4096B space it enters 128 32B spaces and 16*8 = 128. (BitMask de 0 libre 1 ocupada)
};


//FUNCTIONS FOR SMALL

void init_small(struct Small_Memory_Managment* smm);

char* get_entry_small(struct Small_Memory_Managment* smm, int i, int j);

void free_small_entry(struct Small_Memory_Managment* smm, int entry);

char* get_small();

int free_small(char *s);