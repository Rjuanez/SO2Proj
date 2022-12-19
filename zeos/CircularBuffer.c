#include <CircularBuffer.h>

void INIT_CIRCULAR_BUFFER(struct CircularBuffer* circularBuffer)
{
    circularBuffer ->head = cirBuff;
    circularBuffer ->tail = cirBuff;
}

unsigned char* CIRCULAR_BUFFER_CONTROL_ITERATION(struct CircularBuffer* circularBuffer, unsigned char* ptr)
{
    if ((++ptr) == &cirBuff[CIRCULAR_BUFF_SIZE]) return cirBuff;
    else return ptr;
}

void CIRCULAR_BUFFER_ADD(struct CircularBuffer* circularBuffer, unsigned char key)
{
    if (circularBuffer -> full) return;
    //if not full
    *(circularBuffer -> head) = key;
    //we take profit of equality operator return value to save code
    if((circularBuffer -> head = CIRCULAR_BUFFER_CONTROL_ITERATION(circularBuffer, circularBuffer -> head)) == circularBuffer -> tail)
     circularBuffer -> full = 1;
}

unsigned char* CIRCULAR_BUFFER_GET(struct CircularBuffer* circularBuffer)
{
    //check if empty, to save struct slot we return nullptr
    if ((circularBuffer -> tail == circularBuffer -> head) && !circularBuffer->full) return nullptr;
    unsigned char* ret = circularBuffer->tail;
    circularBuffer -> tail = CIRCULAR_BUFFER_CONTROL_ITERATION(circularBuffer, circularBuffer->tail);
    circularBuffer -> full = 0;
    return ret;
}