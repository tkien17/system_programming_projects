#include "lru.h"
#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

void lru_init_queue(Set *set) {
  LRUNode *s = NULL;
  LRUNode **pp = &s;  // place to chain in the next node
  for (int i = 0; i < set->line_count; i++) {
    Line *line = &set->lines[i];
    LRUNode *node = (LRUNode *)(malloc(sizeof(LRUNode)));
    node->line = line;
    node->next = NULL;
    (*pp) = node;
    pp = &((*pp)->next);
  }
  set->lru_queue = s;
}

void lru_init(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    lru_init_queue(&sets[i]);
  }
}

void lru_destroy(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    LRUNode *p = sets[i].lru_queue;
    LRUNode *n = p;
    while (p != NULL) {
      p = p->next;
      free(n);
      n = p;
    }
    sets[i].lru_queue = NULL;
  }
}

void lru_fetch(Set *set, unsigned int tag, LRUResult *result) {
  // TODO:
  // Implement the LRU algorithm to determine which line in
  // the cache should be accessed.
  //
   LRUNode *p = set->lru_queue;
   LRUNode *prev = NULL;

   //search hit 
   for (p = set->lru_queue; p != NULL; prev=p, p = p->next) {
     if (p->line->valid && p->line->tag == tag) {
       result->line = p->line;
       result->access = HIT;
       if (prev != NULL) {
         prev->next = p->next;
         p->next = set->lru_queue;
         set->lru_queue = p;
       }
       return;
     }
   }
   
   // search cold miss 
   prev = NULL; 
   for (p = set->lru_queue; p!=NULL; prev=p, p=p->next){
    if(p->line->valid == 0){
      result->access = COLD_MISS;
      result->line = p->line;
      p->line->valid = 1;
      p->line->tag = tag;
      if (prev != NULL) {
        prev->next = p->next;
        p->next = set->lru_queue;
        set->lru_queue = p;
      }
    return; 
    }
  } 
    result->access = CONFLICT_MISS;
    p=set->lru_queue;
    prev = NULL; 
    while (p->next != NULL) {
      prev = p;
      p = p->next;
    } 
    p->line->tag = tag;
    result->line = p->line;
    if (prev != NULL) {
      prev->next = p->next;
      p->next = set->lru_queue;
      set->lru_queue = p;
    }  
}
