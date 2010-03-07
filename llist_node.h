#ifndef LLIST_NODE_H_
#define LLIST_NODE_H_

#include "config.h"

typedef struct llist_str {
	struct llist_str *next;
	void *contents;

	char label[STRN_SIZE];
} *llist_node;

#endif
