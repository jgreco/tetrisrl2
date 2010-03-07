#include <stdlib.h>
#include <string.h>
#include "hashtable.h"


static unsigned int hash(char *s);

llist_node ht_lookup(llist_node *ht, char *s)
{
	llist_node np;

	for(np = ht[hash(s)]; np != NULL; np = np->next)
		if(strcmp(s, np->label) == 0)
			return np;  /* found */

	return NULL;  /* not found */
}

llist_node ht_add(llist_node *ht, char *s, void *contents)
{
	unsigned int hashval;
	llist_node np = ht_lookup(ht, s);

	if(np == NULL) {
		np = (llist_node) malloc(sizeof(struct llist_str));

/*
		if(np == NULL) {
			fprintf(stderr, "failed to allocate llist_node\n");
			exit(EXIT_FAILURE);
		} */

		strncpy(np->label, s, STRN_SIZE*sizeof(char));

		hashval = hash(s);
		np->next = ht[hashval];
		ht[hashval] = np;
	}

	return np;
}

unsigned int hash(char *s)
{
	unsigned int hashval;

	for(hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;

	return hashval % HASHSIZE;
}
