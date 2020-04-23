#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_node {
	int data;					/* data stored */
	struct queue_node *next;	/* Pointer to next queue_node (NULL if end) */
} QUEUE_NODE;

typedef struct queue {
	QUEUE_NODE *root; /* First element, NULL if empty */
} QUEUE;

/**
 * Creates a new queue using malloc(3)
 *
 * @return	Queue to use for other functions in this module
 *			NULL if malloc(3) failed
 */
QUEUE *queue_init();

/**
 * Inserts int in queue such that all ints are in order
 *
 * @param queue	Queue to use
 * @param data	Data to insert
 */
void queue_insert(QUEUE *queue, int data);

/**
 * Returns the last element of the queue (largest int)
 *
 * @param queue	Queue to get the peek of
 *
 * @return	Peek of queue
 */
int queue_peek(QUEUE *queue);

/**
 * Removes the last element of the queue (largest int/peek)
 *
 * @param queue	Queue to remove the last element of
 *
 * @return	Element that got removed
 */
int queue_pop(QUEUE *queue);

/**
 * Removes the specified element of the queue
 *
 * @param queue	Queue to use
 * @param data	Data to remove
 */
void queue_remove(QUEUE *queue, int data);

/**
 * Frees all memory associated with the queue
 *
 * @param queue	Queue to free
 */
void queue_fini(QUEUE *queue);

#endif /* QUEUE_H */
