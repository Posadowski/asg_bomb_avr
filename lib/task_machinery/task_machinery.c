#include "task_machinery.h"

uint16_t global_task_id = 0;

enum taskMachinery_error taskMachinery_engque(task_queue **head, uint16_t time,
                                              void (*callback)(void *),
                                              void *data) {
  task_queue *new_element = (task_queue *)malloc(sizeof(task_queue));
  if (!new_element) {
    return TASK_ALLOC_ERROR;
  }

  new_element->time_to_execute = time;
  new_element->callback = callback;
  new_element->data = data;

  new_element->taskID = global_task_id++;

  if (*head == NULL || time < (*head)->time_to_execute) {
    // If the queue is empty or the new item has less execution time than the
    // head, then the new element becomes the new head
    new_element->next = *head;
    new_element->prev = NULL;
    if (*head != NULL) {
      (*head)->prev = new_element;
    }
    *head = new_element;
    return TASK_OK;
  }

  task_queue *current = *head;
  while (current->next != NULL && current->next->time_to_execute <= time) {
    current = current->next;
  }

  new_element->next = current->next;
  if (current->next != NULL) {
    current->next->prev = new_element;
  }
  current->next = new_element;
  new_element->prev = current;
  return TASK_OK;
}

uint16_t taskMachinery_task_count() { return global_task_id; }