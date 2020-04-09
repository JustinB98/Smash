#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_node {
	int data;
	struct queue_node *next;
} QUEUE_NODE;

typedef struct queue {
	QUEUE_NODE *root;
} QUEUE;

QUEUE *queue_init();

void queue_insert(QUEUE *queue, int data);

int queue_peek(QUEUE *queue);

int queue_pop(QUEUE *queue);

void queue_remove(QUEUE *queue, int data);

void queue_fini(QUEUE *queue);

#endif /* QUEUE_H */
