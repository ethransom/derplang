#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "map.h"

#include "debug.h"

Map* Map_create() {
	Map *map = malloc(sizeof(Map));
	assert(map != NULL);

	map->length = 0;

	map->not_found_val = NULL;

	return map;
}

void Map_clear(Map* map) {
	if (map->length > 0) {
		struct MapNode *node = map->first;

		for (int i = 0; i < map->length; i++) {
			struct MapNode *next = node->next;
			free(node);
			node = next;
		}
	}

	map->first = NULL;
	map->last = NULL;
	map->length = 0;
}

void Map_destroy(Map *map) {
	assert(map != NULL);

	Map_clear(map);

	free(map);
}

void* _Map_get_node(Map* map, char* key) {
	for (struct MapNode* node = map->first; node != NULL; node = node->next) {
		if (strcmp(node->key, key) == 0) {
			return node;
		}
	}

	return NULL;
}

void Map_set(Map *map, char* key, void *data) {
	struct MapNode* node = _Map_get_node(map, key);
	if (node != NULL) {
		node->data = data;
		return;
	}

	node = malloc(sizeof(struct MapNode));
	assert(node != NULL);

	node->data = data;
	node->key = key;

	// map is empty
	if (map->length == 0) {
		map->first = node;
		map->last = node;
	} else { // map is not empty
	// link nodes together
		map->last->next = node;
		node->prev = map->last;

		map->last = node;
	}

	map->length++;
}

void* Map_get(Map* map, char* key) {
	struct MapNode* node = _Map_get_node(map, key);

	if (node == NULL) return map->not_found_val;

	return node->data;
}

void* Map_del(Map *map, char* key) {
	sentinel("not yet implemented");
	// if (map->length == 0) {
	//   return NULL;
	// }

	void *data = map->last->data;

	// if (map->length == 1) {
	//   free(map->last);
	//   map->last = NULL;
	//   map->first = NULL;
	// }

	// if (map->length > 1) {
	//   struct MapNode *removee = map->last;
	//   map->last = removee->prev;
	//   free(removee);
	//   map->last->next = NULL;
	// }

	// map->length--;

	return data;
error:
	return NULL;
}

void Map_print(Map *map) {
	printf("Map is at memory location: %p\n", map);
	printf("Length: %d\n", map->length);

	struct MapNode *node = map->first;

	for (int i = 0; i < map->length; i++) {
	// printf("\t%d\n", node->data);

		node = node->next;
	}
}
