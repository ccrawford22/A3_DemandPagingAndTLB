#include "translation_lookaside_buffer.h"

template <typename Key, typename Value>
TLBCache<Key, Value>::TLBCache(size_t size)
{
    this->cacheSize = size;
}

template <typename Key, typename Value>
TLBCache<Key, Value>::~TLBCache() {}

template <typename Key, typename Value>
Value TLBCache<Key, Value>::get(const Key &key)
{
    auto it = cacheMap.find(key);
    if (it != cacheMap.end())
    {
        lruList.splice(lruList.begin(), lruList, it->second);
        return it->second->second;
    }
    else
    {
        return Value();
    }
}

template <typename Key, typename Value>
void TLBCache<Key, Value>::insert(const Key &key, const Value &value)
{
    auto it = cacheMap.find(key);
    if (it != cacheMap.end())
    {
        it->second->second = value;
        lruList.splice(lruList.begin(), lruList, it->second);
    }
    else
    {
        if (cacheMap.size() >= cacheSize)
        {
            auto last = lruList.back();
            cacheMap.erase(last.first);
            lruList.pop_back();
        }
        lruList.emplace_front(key, value);
        cacheMap[key] = lruList.begin();
    }
}
