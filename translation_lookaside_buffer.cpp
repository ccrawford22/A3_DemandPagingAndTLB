/*
CS 480 - 1001: Spring 2023
A3: Virtual Memory with TLB Cache
Cody Crawford: 824167663
Caleb Greenfield:
*/

#include <algorithm>
#include "translation_lookaside_buffer.h"
#include "page_table.h"

TLBuffer::TLBuffer() : size(0)
{
}

TLBuffer::TLBuffer(int size) : size(size)
{
}

PageTable::Map *TLBuffer::lookup(unsigned int vpn)
{
    PageTable::Map *mapping = new PageTable::Map();
    auto entryIt = entries.find(vpn);
    if (entryIt != entries.end())
    { // TLBuffer hit
        mapping = entryIt->second.mapping;
        entryIt->second.lastAccessTime = recentlyAccessedPages.size();
        return mapping;
    }
    return nullptr; // TLBuffer miss
}

void TLBuffer::insert(unsigned int vpn, PageTable::Map *mapping)
{
    if (entries.size() == size)
    { // TLBuffer is full
        // Find the entry with the oldest access time
        unsigned int oldestVpn = recentlyAccessedPages.front();
        for (auto vpn : recentlyAccessedPages)
        {
            if (entries[vpn].lastAccessTime < entries[oldestVpn].lastAccessTime)
            {
                oldestVpn = vpn;
            }
        }
        // Remove the entry with the oldest access time
        entries.erase(oldestVpn);
        recentlyAccessedPages.pop_front();
    }
    // Add the new entry
    TLBEntry entry = {vpn, mapping, recentlyAccessedPages.size()};
    entries[vpn] = entry;
    // Update the recently accessed pages
    auto pageIt = std::find(recentlyAccessedPages.begin(), recentlyAccessedPages.end(), vpn);
    if (pageIt != recentlyAccessedPages.end())
    {
        recentlyAccessedPages.erase(pageIt);
    }
    recentlyAccessedPages.push_back(vpn);
}
