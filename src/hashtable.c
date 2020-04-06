#include <stdlib.h>
#include <stdio.h>

#include "hashtable.h"

static LINKED_LIST *get_slot(HASHTABLE *hashtable, int key) {
	int size = hashtable->size;
	LINKED_LIST **slots = hashtable->slots;
	int slot_index = key % size;
	return slots[slot_index];
}

HASHTABLE *hashtable_init(int size) {
	HASHTABLE *hashtable; LINKED_LIST **slots; int i;
	hashtable = malloc(sizeof(HASHTABLE));
	if (hashtable == NULL) goto hashtable_init_failed;
	slots = malloc(sizeof(LINKED_LIST *) * size);
	if (slots == NULL) goto hashtable_init_free_table;
	for (i = 0; i < size; ++i) {
		LINKED_LIST *new_list = linked_list_init();
		if (new_list == NULL) goto hashtable_init_free_slots;
		slots[i] = new_list;
	}
	hashtable->slots = slots;
	hashtable->size = size;
	goto hashtable_init_finish;
hashtable_init_free_slots:
	for (int j = 0; j < i; ++j) {
		free(slots[i]);
	}
	free(slots);
hashtable_init_free_table:
	free(hashtable);
	hashtable = NULL;
hashtable_init_failed:
	fprintf(stderr, "Could not create hashtable\n");
hashtable_init_finish:
	return hashtable;
}

void hashtable_insert(HASHTABLE *hashtable, int key, void *data) {
	LINKED_LIST *slot = get_slot(hashtable, key);
	linked_list_insert(slot, key, data);
}

void hashtable_remove(HASHTABLE *hashtable, int key, void (*onRemove)(void *)) {
	LINKED_LIST *slot = get_slot(hashtable, key);
	linked_list_remove(slot, key, onRemove);
}

void *hashtable_find(HASHTABLE *hashtable, int key) {
	LINKED_LIST *slot = get_slot(hashtable, key);
	return linked_list_find(slot, key);
}

void hashtable_fini(HASHTABLE *hashtable, void (*onRemove)(void *)) {
	int size = hashtable->size;
	LINKED_LIST **slots = hashtable->slots;
	for (int i = 0; i < size; ++i) {
		linked_list_fini(slots[i], onRemove);
	}
	free(hashtable->slots);
	free(hashtable);
}

