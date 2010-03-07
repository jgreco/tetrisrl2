#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "llist_node.h"

#define HASHSIZE 50


llist_node ht_lookup(llist_node *ht, char *s);
llist_node ht_add(llist_node *ht, char *s, void *contents);

#endif
