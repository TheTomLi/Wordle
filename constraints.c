#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "constraints.h"

/* Create and initialize a constraints struct. 
 * Sets the fields to 0 or empty string.
 * Return a pointer to the newly created constraints struct.
 */
struct constraints *init_constraints() {
    struct constraints *result = malloc(sizeof(struct constraints));
    int i;
    for(i=0; i<WORDLEN; i++){
	strcpy(result->must_be[i], "");
    }
    for(i=0; i<ALPHABET_SIZE; i++){
	result->cannot_be[i] = '0';
    }

    return result;
}

/* Update the "must_be" field at "index" to be a string 
 * containing "letter"
 * The tile at this index is green, therefore the letter at "index"
 * must be "letter"
 */
void set_green(char letter, int index, struct constraints *con) {
    assert(islower(letter));
    assert(index >= 0 && index < WORDLEN);
    
    con->must_be[index][0] = letter;
    con->must_be[index][1] = '\0';
}

/* Update "con" by adding the possible letters to the string at the must_be 
 * field for "index".
 * - index is the index of the yellow tile in the current row to be updated
 * - cur_tiles is the tiles of this row
 * - next_tiles is the tiles of the row that is one closer to the solution row
 * - word is the word in the next row (assume word is all lower case letters)
 * Assume cur_tiles and next_tiles contain valid characters ('-', 'y', 'g')
 * 
 * Add to the must_be list for this index the letters that are green in the
 * next_tiles, but not green in the cur_tiles or green or yellow in the 
 * next_tiles at index.
 * Also add letters in yellow tiles in next_tiles.
 */
void set_yellow(int index, char *cur_tiles, char *next_tiles, 
                char *word, struct constraints *con) {

    assert(index >=0 && index < SIZE);
    assert(strlen(cur_tiles) == WORDLEN);
    assert(strlen(next_tiles) == WORDLEN);
    assert(strlen(word) == WORDLEN);
    // make next_tile ggggg, when next_tile is w->grid[0]    
    char next_tile_value[SIZE];
    int j = 0;
    while(j<WORDLEN && (next_tiles[j] == '-' || next_tiles[j] == 'g' || next_tiles[j] == 'y')){
	j += 1;
    }
    if (j == WORDLEN){
	strcpy(next_tile_value, next_tiles);
    }else{
	char all_green[SIZE] = "ggggg";
	strcpy(next_tile_value, all_green);
    }
    // find possible characters and append them to possible_char
    char possible_char[SIZE] = "";
    int i;
    for (i = 0; i < WORDLEN; i++){
    	if (i != index){
	    if (next_tile_value[i] == 'y'){
		strncat(possible_char, &word[i], 1);
	    }
	    if (next_tile_value[i] == 'g' && cur_tiles[i] != 'g'){
		strncat(possible_char, &word[i], 1);
	    }
	}
    }
    strcpy(con->must_be[index], possible_char);
    
}

/* Add the letters from cur_word to the cannot_be field.
 * See the comments in constraints.h for how cannot_be is structured.
 */
void add_to_cannot_be(char *cur_word, struct constraints *con) {
    assert(strlen(cur_word) <= WORDLEN);

    int i;
    for (i = 0; i < strlen(cur_word); i++){
	char letter = cur_word[i];
	int j = letter - 'a';
	con->cannot_be[j] = '1';
    }
}

void print_constraints(struct constraints *c) {
    printf("cannot_be: ");

    int i;
    for(i = 0; i < ALPHABET_SIZE; i++){
    	if (c->cannot_be[i] == '1'){
	    printf("%c ", 'a' + i);
	}
    }
    
    printf("\nmust_be\n");

    for (i = 0; i < WORDLEN; i++){
	printf("[%d] ", i);
	int j = 0;
	while (c->must_be[i][j] != '\0'){
	    printf("%c ", c->must_be[i][j]);
	    j++;
	}
	printf("\n");
    }

}

/* Free all dynamically allocated memory pointed to by c
 */
void free_constraints(struct constraints *c) {
    free(c);
}
