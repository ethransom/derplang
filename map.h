/*
 * Implements a basic hashtable
*/

#ifndef map_h
#define map_h

struct MapNode {
  char* key;
  void *data;
  struct MapNode *next;
  struct MapNode *prev;
};

typedef struct {
  struct MapNode *first;
  struct MapNode *last;
  int length;
  void* not_found_val;
} Map;

Map* Map_create();

void Map_clear(Map *map);

void Map_destroy(Map *map);

void Map_set(Map *map, char* key, void *data);

void* Map_get(Map *map, char* key);

void* Map_del(Map *map, char* key);

// void Map_clone(Map *map);

void Map_print(Map *map);

/*
#define MAP_FOREACH(L, S, M, V) struct MapNode *_MapNode = NULL;\
  struct MapNode *V = NULL;\
  for(V = _MapNode = L->S; _MapNode != NULL; V = _MapNode = _MapNode->M)
*/

#endif