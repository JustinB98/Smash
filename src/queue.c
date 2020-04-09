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
	queue->root = NULL;
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

static QUEUE_NODE *queue_node_insert(QUEUE_NODE *node, QUEUE_NODE *prev, QUEUE_NODE *new_node) {
	if (node == NULL) {
		if (prev != NULL) prev->next = new_node;
		return new_node;
	} else if (new_node->data > node->data) {
		new_node->next = node;
		if (prev != NULL) prev->next = new_node;
		return new_node;
	} else {
		return queue_node_insert(node->next, node, new_node);
	}
}

void queue_insert(QUEUE *queue, int data) {
	QUEUE_NODE *queue_node = create_queue_node(data);
	if (queue_node == NULL) return;
	queue->root = queue_node_insert(queue->root, NULL, queue_node);
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

static void queue_node_remove(QUEUE_NODE *node, QUEUE_NODE *prev, int data) {
	if (node == NULL){
		return;
	} else if (node->data == data) {
		if (prev != NULL) prev->next = node->next;
		free(node);
	} else {
		queue_node_remove(node->next, node, data);
	}
}

void queue_remove(QUEUE *queue, int data) {
	queue_node_remove(queue->root, NULL, data);
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

