/*
 * Generic hashmap manipulation functions
 *
 * Originally by Elliot C Back - http://elliottback.com/wp/hashmap-implementation-in-c/
 *
 * Modified by Pete Warden to fix a serious performance problem, support strings as keys
 * and removed thread synchronization - http://petewarden.typepad.com
 *
 * Modified by Christian Rogers to remove unnecessary typedefs (for my coding style)
 *
 */

#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#define MAP_MISSING -3  /* No such element */
#define MAP_FULL -2    /* Hashmap is full */
#define MAP_OMEM -1    /* Out of Memory */
#define MAP_OK 0    /* OK */

#include <sys/types.h>

struct hashmap_element {
    char *key;
    int in_use;
    void *data;
    size_t size;
};

/* A hashmap has some maximum size and current size,
 * as well as the data to hold. */
struct hashmap_map {
    int table_size;
    int size;
    struct hashmap_element *data;
};




/*
 * void* is a pointer.  This allows you to put arbitrary structures in
 * the hashmap.
 */

/*
 * PFany is a pointer to a function that can take two void* arguments
 * and return an integer. Returns status code..
 */
typedef int (*PFany)(void *, void *);

/*
 * void* is a pointer to an internally maintained data structure.           // No, replaced all uses of this typedef with void*
 * Clients of this package do not need to know how hashmaps are
 * represented.  They see and manipulate only void*'s.
 */


/*
 * Return an empty hashmap. Returns NULL if empty.
*/
extern struct hashmap_map * hashmap_new();

/*
 * Iteratively call f with argument (item, data) for
 * each element data in the hashmap. The function must
 * return a map status code. If it returns anything other
 * than MAP_OK the traversal is terminated. f must
 * not reenter any hashmap functions, or deadlock may arise.
 */
extern int hashmap_iterate(struct hashmap_map *in, PFany f, void *item);

/*
 * Add an element to the hashmap. Return MAP_OK or MAP_OMEM.
 */
extern int hashmap_put(struct hashmap_map *in, char *key, void *value);

/*
 * Get an element from the hashmap. Return MAP_OK or MAP_MISSING.
 */
extern int hashmap_get(struct hashmap_map *in, char *key, void *arg);

/*
 * Remove an element from the hashmap. Return MAP_OK or MAP_MISSING.
 */
extern int hashmap_remove(struct hashmap_map *in, char *key);

/*
 * Get any element. Return MAP_OK or MAP_MISSING.
 * remove - should the element be removed from the hashmap
 */
extern int hashmap_get_one(struct hashmap_map *in, void **arg, int remove);

/*
 * Free the hashmap
 */
extern void hashmap_free(struct hashmap_map *in);

/*
 * Get the current size of a hashmap
 */
extern int hashmap_length(struct hashmap_map *in);

#endif __HASHMAP_H__