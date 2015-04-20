#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <ctype.h>
#include "uthash.h"


typedef struct frequencyHeapNode {
  char *file_path;
  int frequency;
} frequencyHeapNode;

typedef struct hashNode {
  char *word;
  frequencyHeapNode **heap;
  int heap_size;
  UT_hash_handle hh;
} hashNode;

typedef struct charListNode {
  char character;
  struct charListNode *next;
} charListNode;


/*
 * Given a list, it will calculate the length 
 */
int charListFindLength(charListNode *head); 

/*
 * Adds a given character to the end of the given list
 */
charListNode *charListAdd(charListNode *tail, char c);


/*
 * Crawls through the list putting all the characters
 * together into a string holding the word.
 */
char *charListGetWord(charListNode *head);

/*
 * Destroys all elements of the list except 
 * the head holding a bogus character
 */
void charListClear(charListNode *head);

/*
 * Performs the siftup operation for heaps starting
 * at the provided index
 */
void siftUp(frequencyHeapNode **heap, int index);

/*
 * Performs the siftdown operation for heaps
 * given the number of elements in the heap
 */
void siftDown(frequencyHeapNode **heap, int heap_size);

/*
 * Checks if the heap contains a certain file
 */
frequencyHeapNode *heapContains(hashNode *hash_node, char *file_path);

/*
 * Creates a new frequencyHeapyNode from a given file path
 */
frequencyHeapNode *frequencyHeapNodeCreate(char *file_path);

/*
 * prints the heap in the given hashNode to the file pointer
 */
void heapFPrint(FILE *fp, hashNode *hash_node);

/*
 * Prints the heap in the given hashNode to stdout
 */
void heapPrint(hashNode *hash_node);

/*
 * Inserts a node into the heap with the apporpriate 
 * file name
 */
void heapInsert(hashNode *hash_node, char *file_path);

/*
 * Increments the frequency of the word occurence of the
 * approprate file in the heap contained in the hashNode
 */
void heapIncrement(hashNode *hash_node, char *file_path);

/*
 * Finds the hashNode in the hashTable matching the
 * given keyword
 */
hashNode *findKey(char *keyword);

/*
 * Adds the given keyword with the file to the hashTable
 * with a new heap auto created
 */
void addKey(char *keyword, char *file_path);

/*
 * parses a file for it's contents to index
 */
void parseFile(FILE *file, char *file_name);

/*
 * recursively parses a directory for it's files
 */
void parseDir(DIR *dir, char *dir_path);

/*
 * outputs the hashtable to a given filename
 */
void outputHashTable(char *output_filename);

/*
 * prints the hashtable to stdout 
 */
void hashTablePrint();

/*
 * Prints Usage
 */
void printUsage();

