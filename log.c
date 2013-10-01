/** @file log.c */
#include <stdlib.h>
#include <string.h>
#include "log.h"


/**
 * Initializes the log.
 *
 * You may assume that:
 * - This function will only be called once per instance of log_t.
 * - This function will be the first function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @returns
 *   The initialized log structure.
 */
void log_init(log_t *l) {
	l->next = NULL;
	l->prev = NULL;
}

/**
 * Frees all internal memory associated with the log.
 *
 * You may assume that:
 * - This function will be called once per instance of log_t.
 * - This funciton will be the last function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure to be destoryed.
 */
void log_destroy(log_t* l) {
	while(l!=NULL){
		free(l->command);
		free(l);
		l=l->next;
	}
}

/**
 * Push an item to the log stack.
 *
 *
 * You may assume that:
* - All pointers will be valid, non-NULL pointer.
*
 * @param l
 *    Pointer to the log data structure.
 * @param item
 *    Pointer to a string to be added to the log.
 */
log_t *log_push(log_t* l, const char *item) {
	log_t * new = malloc(sizeof(log_t));
	char* newCom = malloc(sizeof(char) * sizeof(item));
	strcpy(newCom,item);
	new->command = newCom;
	l->next = new;
	new->prev = l;
	new->next = NULL;
	return new;
}


/**
 * Preforms a newest-to-oldest search of log entries for an entry matching a
 * given prefix.
 *
 * This search starts with the most recent entry in the log and
 * compares each entry to determine if the query is a prefix of the log entry.
 * Upon reaching a match, a pointer to that element is returned.  If no match
 * is found, a NULL pointer is returned.
 *
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param prefix
 *    The prefix to test each entry in the log for a match.
 *
 * @returns
 *    The newest entry in the log whose string matches the specified prefix.
 *    If no strings has the specified prefix, NULL is returned.
 */
char *log_search(log_t* l, const char *prefix) {
	char *cmd;
	int i;
	int matches;
	while(l!=NULL){
		matches = 1;
		cmd = l->command;
		for(i=0;prefix[i+1]!='\0';i++){
			if(prefix[i+1] != cmd[i]){
				matches = 0;
				break;
			}
		}

		if(matches) return cmd;
		l = l->prev;
	}
    return "No Matches";
}
