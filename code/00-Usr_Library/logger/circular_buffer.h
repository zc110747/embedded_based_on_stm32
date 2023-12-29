#ifndef _CIRCULARBUFFER_H_
#define _CIRCULARBUFFER_H_

typedef struct {
    unsigned int    size;
    unsigned int    currentSize;
    unsigned int    head;
    unsigned int    tail;
    unsigned char   *data;
} CircularBuffer, *CircularBufferPtr;

#define CircularBufferSize(c)           ((c)->currentSize)

#define CircularBufferIsEmpty(c)        ((c)->currentSize == 0)

#define CircularBufferHasData(c)        ((c)->currentSize != 0)

#define CircularBufferIsFull(c)         ((c)->currentSize >= (c)->size)

#define CircularBufferIsNotFull(c)      ((c)->currentSize < (c)->size)

#define CircularBufferInit(cb, s, d)    (cb)->size = s;         \
                                        (cb)->currentSize = 0;  \
                                        (cb)->head        = 0;  \
                                        (cb)->tail        = 0;  \
                                        (cb)->data        = d;

#define CircularBufferPut(cb, c)        if ((cb)->currentSize < (cb)->size)                     \
                                        {                                                       \
                                            (cb)->data[(cb)->head++] = (unsigned char)(c);      \
                                                                                                \
                                            if ((cb)->head == (cb)->size)                       \
                                                (cb)->head = 0;                                 \
                                                                                                \
                                            ++(cb)->currentSize;                                \
                                        }

#define CircularBufferGet(cb, c)        if ((cb)->currentSize)                                  \
                                        {                                                       \
                                            --(cb)->currentSize;                                \
                                                                                                \
                                            c = (cb)->data[((cb)->tail++)];                     \
                                                                                                \
                                            if ((cb)->tail == (cb)->size)                       \
                                                (cb)->tail = 0;                                 \
                                        }

#define CircularBufferFlush(cb)         {																												\
																					(cb)->currentSize = 0;  															\
																					(cb)->head        = 0;  															\
																					(cb)->tail        = 0;																\
																				}

#endif
