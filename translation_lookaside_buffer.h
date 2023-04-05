#ifndef TRANSLATION_LOOKASIDE_BUFFER_H
#define TRANSLATION_LOOKASIDE_BUFFER_H

#include <iostream>
#include <unordered_map>
#include <list>

template <typename Key, typename Value>
class TLBCache
{
public:
    TLBCache(size_t size);
    ~TLBCache();

    Value get(const Key &key);

    void insert(const Key &key, const Value &value);

private:
    size_t cacheSize;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cacheMap;
    std::list<std::pair<Key, Value>> lruList;
};

#endif // TRANSLATION_LOOKASIDE_BUFFER_H