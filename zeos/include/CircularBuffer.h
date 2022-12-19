#define CIRCULAR_BUFF_SIZE 4
unsigned char cirBuff[CIRCULAR_BUFF_SIZE];

 #define nullptr ((void*)0)

struct CircularBuffer{
    unsigned char* head;
    unsigned char* tail;
    unsigned char full;
};

void INIT_CIRCULAR_BUFFER(struct CircularBuffer* circularBuffer);

unsigned char* CIRCULAR_BUFFER_CONTROL_ITERATION(struct CircularBuffer* circularBuffer, unsigned char* ptr);

void CIRCULAR_BUFFER_ADD(struct CircularBuffer* circularBuffer, unsigned char key);

unsigned char* CIRCULAR_BUFFER_GET(struct CircularBuffer* circularBuffer);