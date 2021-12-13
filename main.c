/* doubly_linked_list.c
 *
 * Implement a sorted linked list of strings with operations Insert
 * in alphabetical order, Print, Member, Delete, Free_list.
 * The list nodes are doubly linked.
 *
 * Input:    Single character lower case letters to indicate operations,
 *           possibly followed by value needed by operation -- e.g. 'i'
 *           followed by "hello" to insert the string "hello" -- no
 *           double or single quotes.
 *
 * Output:   Results of operations.
 *
 * Compile:  gcc -g -Wall -o doubly_linked_list doubly_linked_list.c
 *
 * Run:      ./doubly_linked_list
 *
 */

/* You may not add any new header file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Max 99 chars + '\0' */
#define STRING_MAX 100

struct list_node_s {
    char *data;
    struct list_node_s *prev_p;
    struct list_node_s *next_p;
};

/* Pointers to the head and tail of the list */
struct list_s {
    struct list_node_s *h_p;
    struct list_node_s *t_p;
    //int size; Struct would benefit from having a size variable tracking the amount of elements in the list
};

void Insert(struct list_s *list_p, char string[]);

int IndexOf(struct list_s *list_p, char string[]);

void Print(struct list_s *list_p);

int Member(struct list_s *list_p, char string[]);

void Delete(struct list_s *list_p, char string[]);

void Free_list(struct list_s *list_p);

char Get_command(void);

void Get_string(char string[]);

void Free_node(struct list_node_s *node_p);

struct list_node_s *Allocate_node(int size);

void Print_node(char title[], struct list_node_s *node_p);


/*-----------------------------------------------------------------*/
int main(void) {
    char command;
    char string[STRING_MAX];
    struct list_s list;

    list.h_p = list.t_p = NULL;
    /* start with empty list */

    command = Get_command();
    while (command != 'q' && command != 'Q') {
        switch (command) {
            case 'i':
            case 'I':
                Get_string(string);
                Insert(&list, string);
                break;
            case 'p':
            case 'P':
                Print(&list);
                break;
            case 'm':
            case 'M':
                Get_string(string);
                if (Member(&list, string))
                    printf("%s is in the list\n", string);
                else
                    printf("%s is not in the list\n", string);
                break;
            case 'd':
            case 'D':
                Get_string(string);
                Delete(&list, string);
                break;
            case 'f':
            case 'F':
                Free_list(&list);
                break;
            default:
                printf("There is no %c command\n", command);
                printf("Please try again\n");
        }
        command = Get_command();
    }
    Free_list(&list);

    return 0;
}  /* main */


/*-----------------------------------------------------------------*/
/* Function:   Allocate_node
 * Purpose:    Allocate storage for a list node
 * Input arg:  size = number of chars needed in data member (including
 *                storage for the terminating null)
 * Return val: Pointer to the new node
 */
struct list_node_s *Allocate_node(int size) {
    struct list_node_s *node = (struct list_node_s *) malloc(sizeof(struct list_node_s));
    if (node == NULL) {
        return NULL;
    }

    node->data = (char *) malloc(size * sizeof(char));
    if (node->data == NULL) {
        return NULL;
    }

    node->next_p = node->prev_p = NULL;
    return node;
}  /* Allocate_node */


/*-----------------------------------------------------------------*/
/* Function:   Insert
 * Purpose:    Insert new node in correct alphabetical location in list
 * Input arg:  string = new string to be added to list
 * In/out arg: list_p = pointer to struct storing head and tail ptrs
 * Note:       If the string is already in the list, print a message
 *                and return, leaving list unchanged
 */
void Insert(struct list_s *list_p, char string[]) {
    //Allocate memory for the new node
    struct list_node_s *node = Allocate_node(strlen(string) + 1);
    if (node == NULL) {
        printf("Error failed to allocate memory");
        return; //Error check
    }

    strcpy(node->data, string); //Copy string into new node

    //Empty list case- Set both pointers to the first element inserted
    if (list_p->h_p == NULL) {
        list_p->h_p = list_p->t_p = node;
    } else { //List is not empty
        int index = IndexOf(list_p, string); //Get the index of where to insert the new element
        if (index >= 0) { //If a duplicate is found print an error
            printf("Error: Entry %s already exists in list\n", string);
        } else { //No duplicate found
            ++index; //Increment index to compensate for the zero case
            index = -index; //Flip the negative index

            if (index == 0) { //Head Case
                node->next_p = list_p->h_p;
                node->next_p->prev_p = node;
                list_p->h_p = node;
            } else if (index == IndexOf(list_p, list_p->t_p->data) + 1) { //Tail case
                node->prev_p = list_p->t_p;
                node->prev_p->next_p = node;
                list_p->t_p = node;
            } else { //Middle Case
                struct list_node_s *indexNode = list_p->h_p;
                for (int i = 0; i < index - 1; i++) {
                    indexNode = indexNode->next_p;
                }

                node->next_p = indexNode->next_p;
                node->prev_p = indexNode;
                indexNode->next_p = node;
                node->next_p->prev_p = node;
            }
        }
    }
}  /* Insert */

/*-----------------------------------------------------------------*/
/* Function:   IndexOf
 * Purpose:    Get the index of a String in the list. If the String is not found return the negative index of
 *             where it should be
 * Input arg:  string = new string to be added to list
 * In/out arg: list_p = pointer to struct storing head and tail ptrs
 * Note:       Index begins at one to compensate for 0 not having a sign.
 */
int IndexOf(struct list_s *list_p, char string[]) {
    struct list_node_s *node = list_p->h_p;
    int i = 1; //Index begins at 1 to compensate for zero not having a sign
    for (; node != NULL; i++) {
        int cmpResult = strcmp(string, node->data); //Compare string vs the current strings in the list
        if (cmpResult == 0) { //If the string is found return the index
            return i - 1;
        } else if (cmpResult < 0) { //Return when the string position is found
            return -i;
        }
        node = node->next_p;
    }
    return -i; //If the string is not found return the next available position
}

/*-----------------------------------------------------------------*/
/* Function:   Print
 * Purpose:    Print the contents of the nodes in the list
 * Input arg:  list_p = pointers to first and last nodes in list
 *
 * Hint: The implementation of this function (below) shows how
 * to traverse a linked list.
 */
void Print(struct list_s *list_p) {
    struct list_node_s *curr_p = list_p->h_p;

    printf("list = ");

    while (curr_p != NULL) {
        printf("%s ", curr_p->data);
        curr_p = curr_p->next_p;
    }
    printf("\n");
}  /* Print */


/*-----------------------------------------------------------------*/
/* Function:   Member
 * Purpose:    Search list for string
 * Input args: string = string to search for
 *             list_p = pointers to first and last nodes in list
 * Return val: 1, if string is in the list, 0 otherwise
 */
int Member(struct list_s *list_p, char string[]) {
    //Member function is built into our index of function
    return IndexOf(list_p, string) >= 0;
}  /* Member */


/*-----------------------------------------------------------------*/
/* Function:   Free_node
 * Purpose:    Free storage used by a node of the list
 * In/out arg: node_p = pointer to node to be freed
 */
void Free_node(struct list_node_s *node_p) {
    free(node_p->data);
    free(node_p);
}  /* Free_node */


/*-----------------------------------------------------------------*/
/* Function:   Delete
 * Purpose:    Delete node containing string.
 * Input arg:  string = string to be deleted
 * In/out arg  list_p = pointers to head and tail of list
 * Note:       If the string is in the list, it will be unique.  So
 *             at most one node will be deleted.  If the string isn't
 *             in the list, the function just prints a message and
 *             returns, leaving the list unchanged.
 */
void Delete(struct list_s *list_p, char string[]) {
    //Use index function to see if the string is present in the list
    int index = IndexOf(list_p, string);

    if (index >= 0) { //If string is found delete
        //Iterate to the elem,ent we want to delete
        struct list_node_s *node = list_p->h_p;
        for (int i = 0; i < index; i++) {
            node = node->next_p;
        }
        if (index == 0) {//head case
            if (node->next_p != NULL) {
                list_p->h_p = node->next_p;
                list_p->h_p->prev_p = NULL;
            } else {
                list_p->h_p = NULL;
                list_p->t_p = NULL;
            }
        } else if (index == IndexOf(list_p, list_p->t_p->data)) { //tail case
            if (node->prev_p != NULL) {
                list_p->t_p = node->prev_p;
                list_p->t_p->next_p = NULL;
            } else {
                list_p->h_p = NULL;
                list_p->t_p = NULL;
            }
        } else { //middle case
            node->next_p->prev_p = node->prev_p;
            node->prev_p->next_p = node->next_p;
        }
        Free_node(node);
    } else { //If we don't find the string throw an error
        printf("Error: Entry %s is not in the list!\n", string);
    }

}  /* Delete */

/*-----------------------------------------------------------------*/
/* Function:   Free_list
 * Purpose:    Free storage used by list
 * In/out arg: list_p = pointers to head and tail of list
 */
void Free_list(struct list_s *list_p) {
    //Free is built into delete function. Delete the list
    while (list_p->h_p != NULL) {
        Delete(list_p, list_p->h_p->data);
    }
}  /* Free_list */


/*-----------------------------------------------------------------*/
/* Function:   Get_command
 * Purpose:    Find and return the next non-whitespace character in
 *             the input stream
 * Return val: The next non-whitespace character in stdin
 */
char Get_command(void) {
    char c;

    printf("Please enter a command (i, d, m, p, f, q):  ");
    /* Put the space before the %c so scanf will skip white space */
    scanf(" %c", &c);
    return c;
}  /* Get_command */

/*-----------------------------------------------------------------*/
/* Function:   Get_string
 * Purpose:    Read the next string in stdin (delimited by whitespace)
 * Out arg:    string = next string in stdin
 */
void Get_string(char string[]) {
    printf("Please enter a string:  ");
    scanf("%s", string);
}  /* Get_string */


/*-----------------------------------------------------------------*/
/* Function:  Print_node
 * Purpose:   Print the data member in a node or NULL if the
 *            pointer is NULL
 * In args:   title:  name of the node
 *            node_p:  pointer to node
 */
void Print_node(char title[], struct list_node_s *node_p) {
    printf("%s = ", title);
    if (node_p != NULL)
        printf("%s\n", node_p->data);
    else
        printf("NULL\n");
}  /* Print_node */