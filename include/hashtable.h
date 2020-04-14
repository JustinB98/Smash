#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct hashtable {
	int size;
	LINKED_LIST **slots;
} HASHTABLE;

HASHTABLE *hashtable_init(int size);

void hashtable_insert(HASHTABLE *hashtable, int key, void *data);

void hashtable_remove(HASHTABLE *hashtable, int key, void (*onRemove)(void *));

void *hashtable_find(HASHTABLE *hashtable, int key);

void hashtable_fini(HASHTABLE *hashtable, void (*onRemove)(void *));

#endif /* HASHTABLE_H */
