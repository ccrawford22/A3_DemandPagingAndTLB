#ifndef TRANSLATION_LOOKASIDE_BUFFER_H
#define TRANSLATION_LOOKASIDE_BUFFER_H

#include <unordered_map>
#include <list>
#include "page_table.h"

class TLBCache
{
public:
    TLBCache(size_t capacity);

    PageTable::Map *get(unsigned int key);

    void add(unsigned int key, PageTable::Map *value);

private:
    std::unordered_map<unsigned int, PageTable::Map *> cache; // Map for caching key-value pairs
    std::list<unsigned int> lru_list;                         // List for tracking LRU order
    size_t capacity;                                          // Maximum capacity of the cache

    void moveToFront(std::unordered_map<unsigned int, PageTable::Map *>::iterator iter);
};

#endif /* TLB_CACHE_H */