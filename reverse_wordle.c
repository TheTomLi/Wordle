#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordle.h"
#include "constraints.h"

/* Read the wordle grid and solution from fp. 
 * Return a pointer to a wordle struct.
 * See sample files for the format. Assume the input file has the correct
 * format.  In other words, the word on each is the correct length, the 
 * words are lower-case letters, and the line ending is either '\n' (Linux,
 * Mac, WSL) or '\r\n' (Windows)
 */
struct wordle *create_wordle(FILE *fp) {
    struct wordle *w = malloc(sizeof(struct wordle));
    char line[MAXLINE]; 
    w->num_rows = 0;

    while(fgets(line, MAXLINE, fp ) != NULL) {
        
        // remove the newline character(s) 
        char *ptr;
        if(((ptr = strchr(line, '\r')) != NULL) ||
           ((ptr = strchr(line, '\n')) != NULL)) {
            *ptr = '\0';
        }
        
        strncpy(w->grid[w->num_rows], line, SIZE);
        w->grid[w->num_rows][SIZE - 1] = '\0';
        w->num_rows++;
    }
    return w;
}


/* Create a solver_node and return it.
 * If con is not NULL, copy con into dynamically allocated space in the struct
 * If con is NULL set the new solver_node con field to NULL.
 * Tip: struct assignment makes copying con a one-line statements
 */
struct solver_node *create_solver_node(struct constraints *con, char *word) {

    struct solver_node *result = malloc(sizeof(struct solver_node));
    result->con = malloc(sizeof(struct constraints));
    if (con == NULL){
    	result->con = NULL;
    }else{
	int i;
	for (i = 0; i < WORDLEN; i++){
	    strcpy(result->con->must_be[i], con->must_be[i]);
	}
	for (i = 0; i < ALPHABET_SIZE; i++){
	    result->con->cannot_be[i] = con->cannot_be[i];
	}
    }
    strncpy(result->word, word, 6);

    return result;
}

/* Return 1 if "word" matches the constraints in "con" for the wordle "w".
 * Return 0 if it does not match
 */
int match_constraints(char *word, struct constraints *con,
                      struct wordle *w, int row) {
    // return 0 if any of the three cases below happens, otherwise return 1
    for (int i = 0; i < WORDLEN; i++) {
        if (strlen(con->must_be[i]) == 0) {
            if (con->cannot_be[word[i] - 'a'] == '1') {
                return 0;
            }
        }else{
            if (strchr(con->must_be[i], word[i]) == NULL) {
                return 0;
            }
        }
        if (strchr(w->grid[0], word[i]) != NULL) {
            if (strchr(word, word[i]) != strrchr(word, word[i])) {
                return 0;
            }
        }
    }
    return 1;
}



/* remove "letter" from "word"
 * "word" remains the same if "letter" is not in "word"
 */
void remove_char(char *word, char letter) {
    char *ptr = strchr(word, letter);
    if(ptr != NULL) {
        *ptr = word[strlen(word) - 1];
        word[strlen(word) - 1] = '\0';
    }
}

/* Build a tree starting at "row" in the wordle "w". 
 * Use the "parent" constraints to set up the constraints for this node
 * of the tree
 * For each word in "dict", 
 *    - if a word matches the constraints, then 
 *        - create a copy of the constraints for the child node and update
 *          the constraints with the new information.
 *        - add the word to the child_list of the current solver node
 *        - call solve_subtree on newly created subtree
 */

void solve_subtree(int row, struct wordle *w,  struct node *dict,
                   struct solver_node *parent) {
    if (verbose) {
        printf("Running solve_subtree: %d, %s\n", row, parent->word);
    }

    // Base case: when row reached w->num_row, we will set parent to be a leaf.
    if (row == w->num_rows) {
        parent->child_list = NULL;
        parent->con = NULL;
    } else {
	
	// copy parent's constraints->must_be and modify it by calling set_green and set_yellow
        struct constraints *new_con = init_constraints();
	// copy parents contraints->cannot_be to new_con constraint->cannot
	int k;
        for (k = 0; k < ALPHABET_SIZE; k++){
            new_con->cannot_be[k] = parent->con->cannot_be[k];
        }
        add_to_cannot_be(parent->word, new_con);

        int i;
        for (i = 0; i < WORDLEN; i++) {
            if (w->grid[row][i] == 'g') {
                set_green(parent->word[i], i, new_con);
            } else if (w->grid[row][i] == 'y') {
                set_yellow(i, w->grid[row], w->grid[row - 1], parent->word, new_con);
            }
        }

	// find words from dict that satisfies new_con constraints and store in a linked list
        struct node *word_pt = dict;
        struct node *good_words = NULL;
        while (word_pt != NULL) {
            if (match_constraints(word_pt->word, new_con, w, row) == 1 && strcmp(word_pt->word, parent->word) != 0) {
                struct node *new_word = malloc(sizeof(struct node));
                strcpy(new_word->word, word_pt->word);
                new_word->next = good_words;
                good_words = new_word;
            }
            word_pt = word_pt->next;
        }

	// iterate through the good_word linked list and make a linked solver nodes.
        struct solver_node *children = NULL;
        while (good_words != NULL) {
            struct solver_node *new_child = create_solver_node(new_con, good_words->word);
            add_to_cannot_be(good_words->word, new_child->con);
            new_child->next_sibling = children;
            children = new_child;
            good_words = good_words->next;
        }
        parent->child_list = children;
	
	// iterate through linked solver nodes, and call recursivly on each one.
        struct solver_node *child = parent->child_list;
        while (child != NULL) {
            solve_subtree(row + 1, w, dict, child);
            child = child->next_sibling;
        }
    }
}
/* Print to standard output all paths that are num_rows in length.
 * - node is the current node for processing
 * - path is used to hold the words on the path while traversing the tree.
 * - level is the current length of the path so far.
 * - num_rows is the full length of the paths to print
 */

void print_paths(struct solver_node *node, char **path, 
                 int level, int num_rows) {

    // Base case: when we reached the num_rows, we will print path
    if (num_rows == 1){
	path[level - 1] = node->word;
	int i;
	for (i = 0; i < level; i++){
	    printf("%s ", path[i]);
	}
	printf("\n");
    }else{
	// append node->word to path at index level - 1, and pass path
	// to the next recursive calls
	path[level - 1] = node->word;
	struct solver_node *child = node->child_list;
	if (child != NULL){
	    while (child != NULL){
		print_paths(child, path, level + 1, num_rows - 1);
		child = child->next_sibling;
	    }
	}
    }
}

/* Free all dynamically allocated memory pointed to from w.
 */ 
void free_wordle(struct wordle *w){
    free(w);
}

/* Free all dynamically allocated pointed to from node
 */
void free_tree(struct solver_node *node){
    // create an identical pointer to free itself
    struct solver_node *curr = node;
    while(node != NULL){
	// free constrains and its children first, then move node
	// to be its sibling and free the node itself, until no
	// sibling left.
	free_constraints(curr->con);
	free_tree(curr->child_list);
	node = node->next_sibling;
	free(curr);
	curr = node;
    }
    
}
