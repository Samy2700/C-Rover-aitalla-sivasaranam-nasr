#ifndef QUEUE_H
#define QUEUE_H
#include "loc.h"

typedef struct s_queue {
    t_position *values;
    int size;
    int last;
    int first;
} t_queue;

t_queue createQueue(int);
void enqueue(t_queue *, t_position);
t_position dequeue(t_queue *);

#endif // QUEUE_H
