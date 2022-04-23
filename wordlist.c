#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"

/* Read the words from a filename and return a linked list of the words.
 *   - The newline character at the end of the line must be removed from
 *     the word stored in the node.
 *   - You an assume you are working with Linux line endings ("\n").  You are
 *     welcome to also handle Window line endings ("\r\n"), but you are not
 *     required to, and we will test your code on files with Linux line endings.
 *   - The time complexity of adding one word to this list must be O(1)
 *     which means the linked list will have the words in reverse order
 *     compared to the order of the words in the file.
 *   - Do proper error checking of fopen, fclose, fgets
 */
struct node *read_list(char *filename) {
    FILE *word_file = fopen(filename, "r");
    struct node *result = NULL;
    char line[SIZE + 1];
    
    if (word_file == NULL){
	perror("fopen");
	exit(1);
    }
    
    while(fgets(line, SIZE + 1, word_file) != NULL){
	struct node *new = malloc(sizeof(struct node));
	line[5] = '\0';
	strncpy(new->word, line, SIZE);
	new->next = result;
	result = new;
    }


    if (fclose(word_file) != 0) {
	perror("fclose");
	exit(1);
    }
    return result;
}

/* Print the words in the linked-list list one per line
 */
void print_dictionary(struct node *list) {
    struct node *curr = list;
    while (curr != NULL){
	printf("%s\n", curr->word);
	curr = curr->next;
    }
}
/* Free all of the dynamically allocated memory in the dictionary list 
 */
void free_dictionary(struct node *list) {
    struct node *curr = list;
    while (curr != NULL){
	list = list->next;
    	free(curr);
	curr = list;
    }
}
