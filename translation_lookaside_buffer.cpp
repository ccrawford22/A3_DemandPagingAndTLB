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

void TLBuffer::updateRecentlyAccessedPages(unsigned int vpn)
{
    auto entry = std::find(recentlyAccessedPages.begin(), recentlyAccessedPages.end(), vpn);
    if (entry != recentlyAccessedPages.end())
    {
        // Update access time of existing page
        recentlyAccessedPages.erase(entry);
    }
    else if (recentlyAccessedPages.size() >= 8)
    {
        // Remove oldest page if list is full
        auto oldestEntry = std::min_element(recentlyAccessedPages.begin(), recentlyAccessedPages.end(),
                                            [this](const unsigned int &a, const unsigned int &b)
                                            {
                                                return entries[a].lastAccessTime < entries[b].lastAccessTime;
                                            });
        recentlyAccessedPages.erase(oldestEntry);
    }
    // Add new page to list
    recentlyAccessedPages.push_back(vpn);
    // Update access time of all pages
    unsigned int accessTime = entries[vpn].lastAccessTime;
    for (auto &page : recentlyAccessedPages)
    {
        entries[page].lastAccessTime = accessTime;
    }
}

PageTable::Map *TLBuffer::lookup(unsigned int vpn)
{
    PageTable::Map *mapping = new PageTable::Map();
    auto entryIt = entries.find(vpn);
    if (entryIt != entries.end())
    { // TLBuffer hit
        mapping = entryIt->second.mapping;
        updateRecentlyAccessedPages(vpn);
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
