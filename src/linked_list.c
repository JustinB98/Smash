#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

LINKED_LIST *linked_list_init() {
	LINKED_LIST *list = malloc(sizeof(LINKED_LIST));
	list->root = NULL;
	return list;
}

static NODE *create_node(int key, void *data) {
	NODE *node = malloc(sizeof(NODE));
	if (node == NULL) {
		fprintf(stderr, "WARNING, NODE COULD NOT BE CREATED");
		return NULL;
	}
	node->key = key;
	node->data = data;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

static NODE *node_insert(NODE *node, int key, void *data) {
	if (node == NULL) {
		return create_node(key, data);
	} else if (node->next == NULL) {
		NODE *new_node = create_node(key, data);
		node->next = new_node;
		new_node->prev = node;
	} else {
		node->next = node_insert(node->next, key, data);
	}
	return node;
}

void remove_node_from_chain(NODE *node, int key, void (*onRemove)(void *)) {
	NODE *prev = node->prev;
	NODE *next = node->next;
	if (prev != NULL) {
		prev->next = next;
	}
	if (next != NULL) {
		next->prev = prev;
	}
	if (onRemove) onRemove(node->data);
	free(node);
}

static NODE *node_remove(NODE *node, int key, void (*onRemove)(void *)) {
	if (node == NULL) {
		return NULL;
	} else if (node->key == key) {
		NODE *next = node->next;
		remove_node_from_chain(node, key, onRemove);
		return next;
	} else {
		node->next = node_remove(node->next, key, onRemove);
		return node;
	}
}

static void *node_find(NODE *node, int key){
	NODE *current = node;
	while (current) {
		if (current->key == key) return current->data;
		else current = current->next;
	}
	return NULL;
}

static void node_fini(NODE *node, void (*onRemove)(void *)) {
	if (node == NULL) return;
	if (onRemove) onRemove(node->data);
	NODE *next = node->next;
	free(node);
	node_fini(next, onRemove);
}

void linked_list_insert(LINKED_LIST *list, int key, void *data) {
	NODE *root = list->root;
	list->root = node_insert(root, key, data);
}

void linked_list_remove(LINKED_LIST *list, int key, void (*onRemove)(void *)) {
	NODE *root = list->root;
	list->root = node_remove(root, key, onRemove);
}

void *linked_list_find(LINKED_LIST *list, int key) {
	NODE *root = list->root;
	return node_find(root, key);
}

void linked_list_fini(LINKED_LIST *list, void (*onRemove)(void *)) {
	NODE *root = list->root;
	node_fini(root, onRemove);
	free(list);
}
