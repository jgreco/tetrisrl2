#include "monster.h"
#include <iniparser.h>
#include "hashtable.h"
#include <string.h>

static struct llist_str *monsters[HASHSIZE];

static char *construct_key(char *section, char *keyword);
char buf[STRN_SIZE];


void init_monsters()
{
	dictionary *monst_file;
	int i, num_entries;
	monster *new_monst;

	monst_file = iniparser_load("monsters.ini");
	num_entries = iniparser_getnsec(monst_file);

	for(i=0; i<num_entries; i++) {
		/* what type of monster are we reading now? */
		char *sec = iniparser_getsecname(monst_file, i);

		/* add entry to dictionary */
		llist_node tmp = ht_add(monsters, sec, NULL);

		/* allocate monster object */
		tmp->contents = malloc(sizeof(monster));
		new_monst = (monster *)(tmp->contents);

		new_monst->type = strdup(sec);

		new_monst->symbol = strdup(iniparser_getstring(monst_file, construct_key(sec, "symbol"), "x"));

		/* TODO
		//set monster type
		//
		//TODO FINISH ALL OF THIS */
	}

	return;
}

static char *construct_key(char *section, char *keyword)
{
	snprintf(buf, sizeof(char)*STRN_SIZE, "%s:%s", section, keyword);

	return buf;
}
