#ifndef __FifoBuffer_H__
#define __FifoBuffer_H__

#define FifoBuffer_typedef(T, NAME) \
  typedef struct { \
    int size; \
    int start; \
    int end; \
    int write_count;\
    int read_count;\
    T* elems; \
  } NAME

#define FifoBuffer_init(BUF, S, T, BUFMEM) \
  BUF->size = S; \
  BUF->start = 0; \
  BUF->end = 0; \
  BUF->read_count=0;\
  BUF->write_count=0;\
  BUF->elems = (T*)BUFMEM

#define FifoBuffer_write(BUF, ELEM)\
    BUF->elems[BUF->end]=ELEM;\
    BUF->write_count++;\
    BUF->end=(BUF->end+1)%BUF->size;

#define FifoBuffer_read(BUF, ELEM)\
    ELEM=BUF->elems[BUF->start];\
    BUF->read_count++;\
    BUF->start=(BUF->start+1)%BUF->size;

#define FifoBuffer_peek(BUF,ELEM,INDEX)\
    ELEM=BUF->elems[BUF->start+INDEX];

#define FifoBuffer_flush(BUF)\
    BUF->start = 0; \
    BUF->end = 0; \
    BUF->read_count=0;\
    BUF->write_count=0;

#define FifoBuffer_count(BUF) (BUF->write_count-BUF->read_count)
#define FifoBuffer_is_full(BUF) (FifoBuffer_count(BUF)==BUF->size)
#define FifoBuffer_is_empty(BUF) (FifoBuffer_count(BUF)==0)
#define FifoBuffer_overflow(BUF) (FifoBuffer_count(BUF)>=BUF->size)

#endif