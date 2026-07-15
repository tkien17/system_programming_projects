#include "cache.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "lru.h"

char *make_block(int block_size) {
  // TODO:
  //   Make and initialize a block of memory given the block_size.
  //   Note that the memory should be initialized with zeros.
  //
  if (block_size <= 0) {
    return NULL;
  }
  char *block = (char *)malloc(block_size);
  if(block == NULL){
    return NULL;
  }
  for (int i = 0; i < block_size; i++) {
    block[i] = 0;
  }
  return block;
}

Line *make_lines(int line_count, int block_size) {
  // TODO:
  //   Make and initialize the lines given the line count.
  //   Then make and initialize the blocks using make_block function.
  //
  Line *lines = (Line *)malloc(line_count * sizeof(Line));
  if(lines == NULL) return NULL;
  for (int i = 0; i < line_count; i++) {
    lines[i].valid = 0;
    lines[i].tag = 0;
    lines[i].block_size = block_size;
    lines[i].block = make_block(block_size);
    if(lines[i].block == NULL){
      for (int j = 0; j < i; j++) {
        free(lines[j].block);
      }
      free(lines);
      return NULL;
    }
  }
  return lines;
}

Set *make_sets(int set_count, int line_count, int block_size) {
  // TODO:
  //   Make and initialize the sets given the set count. Then
  //   make and initialize the lines and blocks.
  //
  Set *sets = (Set *)malloc(set_count * sizeof(Set));
  if(sets == NULL) return NULL;
  for (int i = 0; i < set_count; i++) {
    sets[i].line_count = line_count;
    sets[i].lines = make_lines(line_count, block_size);
    if(sets[i].lines == NULL){
      for (int j = 0; j < i; j++) {
        for (int k = 0; k < sets[j].line_count; k++) {
          free(sets[j].lines[k].block);
        }
        free(sets[j].lines);
      }
      free(sets);
      return NULL;
    }
  }
  return sets;
}

Cache *make_cache(int set_bits, int line_count, int block_bits) {
  Cache *cache = (Cache *)malloc(sizeof(Cache));
  if(cache == NULL) return NULL;
  // TODO:
  //   Make and initialize the cache, sets, lines, and blocks.
  //   You should use the `exp2` function to determine the
  //   set_count and block_count from the set_bits and block_bits
  //   respectively (use `man exp2` from the command line).
  //
  // ADD YOUR CODE HERE:
  int set_count = (int)exp2(set_bits);
  int block_size = (int)exp2(block_bits);

  cache->set_count = set_count; 
  cache->line_count = line_count;
  cache->block_size = block_size;
  cache->set_bits = set_bits;
  cache->block_bits = block_bits;
  cache->sets = make_sets(set_count, line_count, block_size);
  if(cache->sets == NULL){
    free(cache);    
    return NULL;
  }

  // END TODO

  // Create LRU queues for sets:
  if (cache != NULL) {
    lru_init(cache);
  }

  return cache;
}

void delete_block(char *accessed) { free(accessed); }

void delete_lines(Line *lines, int line_count) {
  for (int i = 0; i < line_count; i++) {
    delete_block(lines[i].block);
  }
  free(lines);
}

void delete_sets(Set *sets, int set_count) {
  for (int i = 0; i < set_count; i++) {
    delete_lines(sets[i].lines, sets[i].line_count);
  }
  free(sets);
}

void delete_cache(Cache *cache) {
  lru_destroy(cache);
  delete_sets(cache->sets, cache->set_count);
  free(cache);
}

SearchInfo get_bits(Cache *cache, address_type address) {
  SearchInfo result;
  
  // TODO:
  //  Extract the set bits, tag bits, and block bits from a 32-bit address into
  //    result.
  //
  result.offset = address & ((1<<cache->block_bits) - 1); 
  unsigned int set_bits_extracted = address & ((1<<(cache->set_bits + cache->block_bits)) - 1 - result.offset);
  result.set_id = set_bits_extracted >> cache->block_bits;
  result.tag = address >> (cache->set_bits + cache->block_bits);
  return result;
}

AccessResult cache_access(Cache *cache, TraceLine *trace_line) {
  SearchInfo bits = get_bits(cache, trace_line->address);
  unsigned int s = bits.set_id;
  unsigned int t = bits.tag;
  unsigned int b = bits.offset;

  // Get the set:
  Set *set = &cache->sets[s];

  // Get the line:
  LRUResult result;
  lru_fetch(set, t, &result);
  Line *line = result.line;

  // If it was a miss we will clear the accessed bits:
  if (result.access != HIT) {
    for (int i = 0; i < cache->block_size; i++) {
      line->block[i] = 0;  
    }
  }

  // Then set the accessed byte to 1:
  line->block[b] = 1;

  return result.access;
}
