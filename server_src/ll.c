#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ll.h"

typedef struct node {
    entry_t entry;
    struct node *next;
} node_t;

typedef struct ll_map {
    size_t length;
    node_t *head;
} ll_map_t;

/**
 * Initialize a new node with the given key and value.
 * 
 * The returned node should be heap-allocated with malloc and it takes ownership
 * of the arguments so the caller should not modify or free them afterward.
 */
static node_t *node_init(char *key, char *value);

/**
 * Frees a linked list node and all nodes it points to.
 * 
 * Also frees the keys and values since they are owned by the node.
 * 
 * Passing NULL to this function is valid (and will do nothing).
 */
static void node_free(node_t *curr);

static node_t *node_init(char *key, char *value) {

    //Needs to manually count sizes because of a bug in the test code
    size_t thekeysize = 0;

    while(key[thekeysize]!='\0'){
        thekeysize++;
    }

    size_t thevalsize = 0;

    while(value[thevalsize]!='\0'){
        thevalsize++;
    }

    //Push this node instance onto the heap
    node_t *nod = malloc(sizeof(node_t));  

    //Initalize the next node pointer as NULL
    nod->next = NULL;

    //Create temporary entry pointer (must free later)
    entry_t *entree = malloc(sizeof(entry_t));   

    //Store node parameters in output node pointer nod
    entree->key = key;
    entree->value = value;
     
    //Dereference entry before storing it in nod
    nod->entry = *entree;

    //Don't forget to free the entry pointer
    free(entree);   

    return nod;
}

static void node_free(node_t *curr) {
    //Begin recursion
    //If curr is not the tail, the next node is freed
    if(curr->next!=NULL){
        node_free(curr->next);
    }

    //If curr is the tail, then curr->next is NULL
    //and the above line did nothing (feature not bug)

    //We can now safely free this node
    free(curr->entry.key);
    free(curr->entry.value);
    free(curr);
    //After this function returns, the previous node will
    //be freed until the head is reached
}


ll_map_t *ll_init(void) {
    //Push this llmap onto the heap
    ll_map_t *lisa = malloc(sizeof(ll_map_t));

    //Initalize the head node with an empty key-value pair
    lisa->head = NULL;

    //Initalize the size of the map to 0
    lisa->length = (size_t)0;

    return lisa;
}

void ll_free(ll_map_t *dict) {
    //Make sure that you only free the head if the list is
    //actually initalized
    if(dict->head!=NULL){
    
    //Since node free is recursive, freeing
    //the head frees the entire linked list
        node_free(dict->head);
    }

    free(dict);

}

char *ll_put(ll_map_t *dict, char *key, char *value) {
    //Adds a new key-value pair to the dict. If the key already exists, the value
    //is replaced and the old value is returned. Otherwise, NULL is returned.

    //Make local variables
    char *oldvalue = NULL;
    char *testkey = NULL;
    node_t *testnode = dict->head;

    //First lets check the head node
    if (testnode == NULL){
        dict->head = node_init(key, value);
        dict->length++;
    }
    else {  //Head node exists
        //Traverse list until the end is reached
        for(size_t i = 0; i<dict->length; i++){ //goes through as many times as there are data nodes
            //Check that the key does not exist in the dictionary
            //Fetch the current node's key value
            testkey = testnode->entry.key;

            //If it matches the input key, store the old value 
            //and replace it                
            if(strcmp(testkey, key) == 0){
                oldvalue = testnode->entry.value;

                //Create temporary entry pointer (must free later)
                entry_t *entree2 = malloc(sizeof(entry_t));
                entree2->value = value;
                entree2->key = key;

                //Free the old key
                free(testnode->entry.key);
 
                //Dereference entry before storing it in nod
                testnode->entry = *entree2;

                //Don't forget to free the entry pointer
                free(entree2);
            }
            //Go check next node
            testnode = testnode->next;
        }

        //If the old value did not change, put
        //the key-value pair in the dictionary
        if(oldvalue == NULL){
            //Init the node
            testnode = dict->head;
            while(testnode->next!=NULL){
                testnode = testnode->next;
            }
            testnode->next = node_init(key, value);

            //Increment the size of the dictionary
            dict->length++;
        }

    }

    //The appropriate return value should be in old value
    return oldvalue;
}

char *ll_get(ll_map_t *dict, char *key) {
//    printf("\nSTARTING GET AT DICT %p LEN %zu LOOKING FOR %s's pair\n", dict, dict->length, key);
    //Init local vars
    char *dictvalue = NULL;
    char *testkey = NULL;
    //Prep test node pointer to start testing the head
    node_t *testnode = dict->head;
/*            printf("%p\n", testnode);
            printf("%s\n", testnode->entry.key);
            printf("%s\n", testnode->entry.value);
            printf("current test testkey:%s key:%s testnodeval:%s dictval: %s\n", testkey, key, testnode->entry.value, dictvalue);*/
    //Will loop until testnode is NULL (reached invalid node)
    for(size_t i = 0; i<dict->length; i++){
        //Check current test node's key
        testkey = testnode->entry.key;
//        printf("current test testkey:%s key:%s testnodeval:%s cmp: %d; dictval: %s\n", testkey, key, testnode->entry.value, strcmp(testkey, key), dictvalue);
        //If the key is the one we are looking for, set
//            printf("current test testkey:%s key:%s testnodeval:%s cmp: %d; dictval: %s\n", testkey, key, testnode->entry.value, strcmp(testkey, key), dictvalue);
        //dictvalue to the corresponding value to return
        if((strcmp(testkey, key) == 0) && dictvalue==NULL){
//        printf("current test testkey:%s key:%s testnodeval:%s cmp: %d; dictval: %s\n", testkey, key, testnode->entry.value, strcmp(testkey, key), dictvalue);
            dictvalue = testnode->entry.value;

        }

        //Update testnode to test the next node

        testnode = testnode->next;

    }

//    printf("\nRETURNING FROM GET WITH %s\n", dictvalue);
    return dictvalue;
}

strarray_t *ll_get_keys(ll_map_t *dict) {
    /**
     * Returns an array of all the keys in the dict. The array should
     * be freed with `strarray_free` when it is no longer needed. See `str_util.h` 
     * for more information on `strarray_t`.
     */

    strarray_t *keys = strarray_init(dict->length);
    node_t *tempnode = dict->head;
    for(size_t i = 0; i<dict->length; i++){
        keys->data[i] = malloc(sizeof(char) * (strlen(tempnode->entry.key)+1));
        strcpy(keys->data[i], tempnode->entry.key);
        tempnode = tempnode->next;
    }

    return keys;
}


























