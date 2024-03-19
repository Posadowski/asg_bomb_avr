#ifndef TASK_MACHINERY_H_
#define TASK_MACHINERY_H_

#include <avr/io.h>
#include <stdlib.h>
enum taskMachinery_error {
  TASK_OK = 0,
  TASK_ALLOC_ERROR,
  TASK_TIME_OUT_OF_RANGE,
  TASK_QUEUE_EMPTY
};

typedef struct task_queue {
  uint16_t time_to_execute;  // milliseconds
  struct task_queue *next;
  struct task_queue *prev;
  void (*callback)(void *);  // pointer on callback
  void *data;  // pointer to data to be passed to the callback function
} task_queue;

// void taskMachinery_deque(uint16_t task_ID);
enum taskMachinery_error taskMachinery_engque(task_queue **head, uint16_t time,
                                              void (*callback)(void *),
                                              void *data);

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

#endif  // TASK_MACHINERY_H_