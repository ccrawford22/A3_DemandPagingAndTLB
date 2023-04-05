#include "translation_lookaside_buffer.h"
#include "page_table.h"

TLBCache::TLBCache(size_t capacity) : capacity(capacity) {}

PageTable::Map *TLBCache::get(unsigned int key)
{
    auto iter = cache.find(key);
    if (iter == cache.end())
    {
        return nullptr;
    }
    else
    {
        // Cache hit, move accessed item to front of the list
        moveToFront(iter);
        return iter->second;
    }
}

void TLBCache::add(unsigned int key, PageTable::Map *value)
{
    if (cache.size() >= capacity)
    {
        // If cache is full, evict least recently used item
        auto last = lru_list.back();
        cache.erase(last);
        lru_list.pop_back();
    }
    // Add new item to cache
    cache[key] = value;
    lru_list.push_front(key);
}

void TLBCache::moveToFront(std::unordered_map<unsigned int, PageTable::Map *>::iterator iter)
{
    // Move accessed item to front of the list to represent it as most recently used
    lru_list.remove(iter->first);
    lru_list.push_front(iter->first);
}