#include "index.h"

hashNode *inverted_index_hashtable = NULL;

int charListFindLength(charListNode *head) {
  int length = 0;
  charListNode *ptr;
  for (ptr = head; ptr != NULL; ptr = ptr->next) {
    length += 1;
  }
  return length;
}

charListNode *charListAdd(charListNode *tail, char c) { 
  charListNode *new_node = (charListNode *)malloc(sizeof(charListNode));
  new_node->character = c;
  new_node->next = NULL;
  tail->next = new_node;
  return new_node;
}

char *charListGetWord(charListNode *head) {
  int length = charListFindLength(head);
  int i;
  char *word = (char *)malloc(sizeof(char) * length);
  charListNode *ptr = head->next;
  for (i = 0; i < length-1; i++) { // Looping through charList
    word[i] = ptr->character;
    ptr = ptr->next;
  }
  word[length-1] = '\0';
  return word;
}

void charListClear(charListNode *head) { 
  charListNode *prev = head->next;
  charListNode *ptr;
  
  if (prev == NULL) {
    return;
  }
  for (ptr = prev->next; ptr != NULL; prev = ptr, ptr = ptr->next) {
    free(prev);
    prev = NULL;
  }
  head->next = NULL;
}

void swap(frequencyHeapNode **heap, int parent_index, int child_index) {
  frequencyHeapNode *tmp_ptr = heap[parent_index];
  heap[parent_index] = heap[child_index];
  heap[child_index] = tmp_ptr;
}

void siftUp(frequencyHeapNode **heap, int index) { 
  int parent_index;
  if (index == 0) return;
  if (index % 2 == 0) 
    parent_index =  (index - 1) / 2;
  else
    parent_index =  index / 2;

  while ( (index >= 0) && (parent_index >= 0) && (index != parent_index) ) { // Checks to make sure indexes are valid and not the same
    if (heap[parent_index]->frequency < heap[index]->frequency) { //Comparison to support a maxheap
      swap(heap, parent_index, index);
      index = parent_index;

      if (index % 2 == 0) 
	parent_index = (index - 1) / 2;
      else 
	parent_index =  index / 2;
    }
    else {
      return;
    }
  }
}

void siftDown(frequencyHeapNode **heap, int heap_size){ 
  int current = 0;
  int max = 0;
  int left = 2*current; 
  int right = 2*current + 1;

  while (current < heap_size) {
    if (heap[left]->frequency > heap[max]->frequency) {
      max = left;
    }
    if (heap[right]->frequency > heap[max]->frequency) {
      max = right;
    }
    if (max == current) {
 
     return;
    }
    swap(heap, max, current);
    current = max;
  }
}

frequencyHeapNode *heapContains(hashNode *hash_node, char *file_path) { 
  int i;

  if (hash_node == NULL) return NULL;
  if (hash_node->heap == NULL) return NULL;

  for (i = 0; i < hash_node->heap_size; i += 1) {
    if ( strcmp( hash_node->heap[i]->file_path, file_path ) == 0 ) {
      return hash_node->heap[i];
    }
  }
  return NULL;
}

frequencyHeapNode *frequencyHeapNodeCreate(char *file_path){
    frequencyHeapNode *new_node = (frequencyHeapNode *)malloc(sizeof(frequencyHeapNode));
    new_node->file_path = (char *)malloc( (strlen(file_path)+1) * sizeof(char *) );
    strcpy(new_node->file_path, file_path);
    new_node->file_path[strlen(file_path)] = '\0';
    new_node->frequency = 1;
    return new_node;
}

void heapPrint(hashNode *hash_node) {
  int i;
  for (i = 0; i < hash_node->heap_size; i++) {
    printf("%d: (%s, %d) at %ld\n", i, hash_node->heap[i]->file_path, hash_node->heap[i]->frequency, (long int)hash_node->heap[i]);
  }
}

void heapFPrint(FILE *fp, hashNode *hash_node) {
  int i;
  for (i = 0; i < hash_node->heap_size; i++) {
    fprintf(fp, "%s", hash_node->heap[i]->file_path);
    fprintf(fp, " ");
    fprintf(fp, "%d ", hash_node->heap[i]->frequency); 
  }
}

void heapInsert(hashNode *hash_node, char *file_path) {
  frequencyHeapNode *new_node;
  frequencyHeapNode **tmp_heap;

  if ( (hash_node->heap_size + 1) % 50 == 0) {
    tmp_heap = (frequencyHeapNode **)realloc(hash_node->heap, sizeof(frequencyHeapNode *) * (50 + hash_node->heap_size));
    hash_node->heap = tmp_heap;
  }

  new_node = frequencyHeapNodeCreate(file_path);
  hash_node->heap[hash_node->heap_size] = new_node;
  siftUp(hash_node->heap, hash_node->heap_size);

  hash_node->heap_size++;

}

void heapIncrement(hashNode *hash_node, char *file_path) { 
  frequencyHeapNode *ptr = NULL;
  if ( !(ptr = heapContains(hash_node, file_path)) ) {
    /* Heap Doesn't have record of this file */
    heapInsert(hash_node, file_path);
  }
  else {
    ptr->frequency++;
  }
}


void heapDestroy(frequencyHeapNode **heap, int heap_size) {
  int i;
  
  for ( i = 0; i < heap_size; i++ ) {
    free(heap[i]->file_path);
    printf("Destroying block at %ld\n", (long int)heap[i]);
    free(heap[i]);
  }
}

hashNode *findKey(char *keyword) {
  hashNode *ptr = NULL;
  hashNode *s = NULL;
  for(s = inverted_index_hashtable; s != NULL; s = s->hh.next) {
    if(strcmp(keyword, s->word) == 0) {
      return s;
    }
  }

  //HASH_FIND_STR(inverted_index_hashtable, keyword, ptr);

  return ptr;
}

void addKey(char *keyword, char *file_path) { 
  hashNode *new_node;

  new_node = (hashNode *)malloc(sizeof(hashNode));
  new_node->word = (char *)malloc(sizeof(*keyword));
  strcpy(new_node->word, keyword);
  frequencyHeapNode **new_heap = (frequencyHeapNode **)malloc(sizeof(sizeof(frequencyHeapNode *) * 50)); /* Heap starts with 50 buckets */
  new_node->heap = new_heap;
  new_node->heap_size = 0;
  heapIncrement(new_node, file_path);
  HASH_ADD_STR(inverted_index_hashtable, word, new_node); //Adds new key and value to hashtable 
  
}

int hashNodeCmp(hashNode *a, hashNode *b) {
  return (strcmp(a->word, b->word));
}

/*
 * INPUT: File pointer
 * 
 * Parses through the file and extracts words
 * to be dumped into the frequency hash table.
 */
void parseFile(FILE *file, char *file_name) { 
  char c;
  charListNode *charlist_head = (charListNode *)malloc(sizeof(charListNode));
  charlist_head->character = '$'; /* $ will indicate the head of the list */
  charlist_head->next = NULL;
  charListNode *charlist_tail = charlist_head;
  char *current_word;
  hashNode *hashkey_ptr;
  char buf[PATH_MAX + 1];
  char *file_path = realpath(file_name, buf);


  while ( (c = fgetc(file)) != EOF ) {
    if (!isalnum(c)) {
      /* End of current word */
      if ( charlist_head->next == NULL ) {
	continue;
      }
      
      current_word = charListGetWord(charlist_head);
      if ( !(hashkey_ptr = findKey(current_word)) ) {
	/* Current word not found in hash table */
	addKey(current_word, file_path);
      }
      else { /* Word was found in hash table */
	heapIncrement(hashkey_ptr, file_path);
	hashTablePrint();
      }
      charListClear(charlist_head);
      current_word = NULL;
      hashkey_ptr = NULL;
      charlist_tail = charlist_head;
    }
    else {
      if (isalpha(c)) c = tolower(c);
      charlist_tail = charListAdd(charlist_tail, c);
    }
  }

  free(charlist_head);
  //hashTablePrint();
}

void parseDir(DIR *dir, char *dir_path) { //INCOMPLETE
  struct dirent *dptr;
  DIR *recursing_dirp;
  FILE *file;
  char path_buf[PATH_MAX + 1]; 

  while ( (dptr = readdir(dir)) ) {
    path_buf[0] = '\0';
    strcat(path_buf, dir_path); 
    strcat(path_buf, "/");
    strcat(path_buf, dptr->d_name);
    /* path_buf now holds the relative path to file/dir */
    if( (strcmp(dptr->d_name, ".") == 0) || (strcmp(dptr->d_name, "..") == 0) ) {
      continue;
    }
    if (dptr->d_type == DT_REG) {
      file = fopen(path_buf, "r");
      if (file == NULL) {
      }
      parseFile(file, path_buf);
      fclose(file);
    }
    else if (dptr->d_type == DT_DIR) {
      recursing_dirp = opendir(path_buf); 
      parseDir(recursing_dirp, path_buf);//recursive call for nested directories
      closedir(recursing_dirp);
    }
    else {
      printf("Bad file from: %s\n", path_buf);
      exit(1);
    }
  }
}


void outputHashTable(char *output_filename){ //INCOMPLETE
  
  FILE *fp = fopen(output_filename, "w");
  if ( fp == NULL ) {
      printf("error opening file.\n");
      exit(1);
  }
  hashNode *s;
  for (s = inverted_index_hashtable; s != NULL; s = s->hh.next) {
    fprintf(fp, "<list> ");
    fprintf(fp, "%s\n", s->word);
    heapFPrint(fp, s);
    fprintf(fp, "\n</list>\n\n");
  }
  fclose(fp);
}

void hashTablePrint() {
  hashNode *s;
  for(s = inverted_index_hashtable; s != NULL; s = s->hh.next) {
    printf("Word: %s, Heap Address: %ld, Heap Size: %d\n", s->word, (long int)s->heap, s->heap_size);
    heapPrint(s);
    printf("\n");
  }
}

void hashTableDestroy() {
  hashNode *s;
  for( s = inverted_index_hashtable; s != NULL; s = s->hh.next ) {
    free(s->word);
    heapDestroy(s->heap, s->heap_size);
  }

}
void printUsage() {
  fprintf(stdout, "USAGE: index <output file> <input file/directory>\n");
}

int main(int argc, char **argv) { //INCOMPLETE
  DIR *dir = NULL;
  FILE *file = NULL;
  if (argc != 3) {
    fprintf(stderr, "Incorrect Number of Arguments\n");
    printUsage();
    exit(1);
  }

  if ( !(dir = opendir(argv[2])) ) {
    /* Second argument isn't a directory */
    if ( !(file = fopen(argv[2], "r")) ) {
      fprintf(stderr, "File Not Found: %s\n", argv[2]);
      exit(1);
    }
    /* Indexing a single file */
    parseFile(file, argv[2]);
    fclose(file);
  }
  else {
    parseDir(dir, argv[2]);
    closedir(dir);
  }
 
  HASH_SORT(inverted_index_hashtable, hashNodeCmp);

  outputHashTable(argv[1]);

  return 1;
}

