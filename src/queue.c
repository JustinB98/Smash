#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "queue.h"


QUEUE *queue_init() {
	QUEUE *queue = malloc(sizeof(QUEUE));
	if (queue == NULL) {
		perror("Could not create a queue");
	}
	return queue;
}

static QUEUE_NODE *create_queue_node(int data) {
	QUEUE_NODE *queue_node = malloc(sizeof(QUEUE_NODE));
	if (queue_node == NULL) {
		perror("Could not create a queue node");
	}
	queue_node->data = data;
	queue_node->next = NULL;
	return queue_node;
}

void queue_insert(QUEUE *queue, int data) {
	QUEUE_NODE *queue_node = create_queue_node(data);
	if (queue_node == NULL) return;
	queue_node->next = queue->root;
	queue->root = queue_node;
}

int queue_peek(QUEUE *queue) {
	if (queue->root == NULL) {
		return 0;
	} else {
		return queue->root->data;
	}
}

int queue_pop(QUEUE *queue) {
	if (queue->root == NULL) return 0;
	QUEUE_NODE *original_root = queue->root;
	queue->root = original_root->next;
	int data = original_root->data;
	free(original_root);
	return data;
}

void queue_remove(QUEUE *queue, int data) {
	QUEUE_NODE *current = queue->root;
	QUEUE_NODE *prev = NULL;
	while (current) {
		if (current->data == data) {
			if (prev != NULL) prev->next = current->next;
			free(current);
		}
		prev = current;
		current = current->next;
	}
}

void queue_fini(QUEUE *queue) {
	QUEUE_NODE *current = queue->root;
	QUEUE_NODE *temp = NULL;
	while (current) {
		temp = current;
		current = current->next;
		free(temp);
	}
	free(queue);
}

