/*
CS 480 - 1001: Spring 2023
A3: Virtual Memory with TLBuffer Cache
Cody Crawford: 824167663
Caleb Greenfield:
*/

#ifndef TRANSLATION_LOOKASIDE_BUFFER_H
#define TRANSLATION_LOOKASIDE_BUFFER_H

#include <unordered_map>
#include <deque>
#include "page_table.h"

struct TLBEntry
{
    unsigned int vpn;            // Virtual Page Number
    PageTable::Map *mapping;     // Physical Frame Number
    unsigned int lastAccessTime; // Last access time of this entry
};

class TLBuffer
{
public:
    TLBuffer();
    TLBuffer(int size);
    PageTable::Map *lookup(unsigned int vpn);
    void insert(unsigned int vpn, PageTable::Map *mapping);

private:
    int size;
    std::unordered_map<unsigned int, TLBEntry> entries;
    std::deque<unsigned int> recentlyAccessedPages; // Stores the VPNs of the recently accessed pages
};

#endif