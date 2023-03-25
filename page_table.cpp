/*
CS 480 - 1001: Spring 2023
A3: Virtual Memory with TLB Cache
Cody Crawford: 824167663
Caleb Greenfield:
*/

#include <climits>
#include "page_table.h"
#include <vector>
#include <iostream>
#include <math.h>

PageTable::PageTable(unsigned int shifts[], std::vector<int> sizes, int levelCount, int addressSize)
{
    this->levelCount = levelCount;
    this->bitShift = &shifts[0];
    this->levelSizes = &sizes[0];
    this->bitMask = new unsigned int[levelCount];
    this->entryCount = new unsigned int[levelCount];
    this->offsetSize = this->bitShift[levelCount - 1];
    unsigned int fullVPNMask = PageTable::createMask(addressSize - this->bitShift[levelCount - 1], this->bitShift[levelCount - 1]);
    this->offsetMask = ~fullVPNMask;

    for (int i = 0; i < levelCount; i++)
    {
        bitMask[i] = PageTable::createMask(this->levelSizes[i], this->bitShift[i]);
    }

    for (int i = 0; i < levelCount; i++)
    {
        entryCount[i] = pow(2, levelSizes[i]);
    }

    // initialize root last as levels will try to access entryCount in the page table
    this->root = new Level(this, 0);

    for (int i = 0; i < levelCount; i++)
    {
        printf("Mask: 0x%08x\n", this->bitMask[i]);
    }
}

PageTable::~PageTable()
{
    delete (root);
    root = nullptr;
    delete bitMask;
    bitMask = nullptr;
    delete bitShift;
    bitShift = nullptr;
    delete entryCount;
    entryCount = nullptr;
}

unsigned int PageTable::createMask(int numOfMaskBits, int shift)
{
    unsigned int mask = 1;

    for (int b = 1; b < numOfMaskBits; b++)
    {
        mask = mask << 1;
        mask = mask | 1;
    }
    // shift left
    mask = mask << shift;
    return mask;
}

PageTable::Level::Level(PageTable *pageTable, unsigned int depth)
{
    this->pageTable = pageTable;
    this->depth = depth;
    this->entryCount = pageTable->entryCount[depth];
    this->nextLevel = new PageTable::Level *[entryCount];
    this->map = new PageTable::Map *[entryCount];

    for (size_t i = 0; i < entryCount; i++)
    {
        nextLevel[i] = nullptr;
        map[i] = nullptr;
    }
}

PageTable::Level::~Level()
{
    delete (pageTable);
    pageTable = nullptr;
    delete nextLevel;
    nextLevel = nullptr;
    delete map;
    map = nullptr;
}

PageTable::Map::Map(PageTable *pageTable, unsigned int mapping, unsigned int frame, unsigned int *pages)
{
    this->pageTable = pageTable;
    this->mapping = mapping;
    this->frame = frame;
    this->pages = pages;
}

/**
 * @brief Given a virtual address, apply the given bit mask and shift right by the given number
    of bits. Returns the virtual page number. This function can be used to access the virtual
    page number of any level by supplying the appropriate parameters.
    ▪ Example: Suppose the level two pages occupied bits 22 through 27, and we wish to extract the
    second level page number of address 0x3c654321. virtualAddressToVPN (0x3c654321, 0x0FC00000, 22)
    should return 0x31 (decimal 49). Remember, this is computed by taking the bitwise and operation
    of 0x3c654321 and 0x0FC00000, which is 0x0C400000. We then shift right by 22 bits. The last five
    hexadecimal zeros take up 20 bits, and the bits higher than this are 1100 0110 (C6). We shift
    by two more bits to have the 22 bits, leaving us with 11 0001, or 0x31.
        ▪ Check out the given bitmasking-demo.c for bit masking and shifting for extracting bits in the
        hexadecimal number.
    ▪ Note: to get the full Virtual Page Number (VPN) from all page levels, you would construct the
    bit mask for all bits preceding the offset bits, take the bitwise and of the virtual address and
    the mask, then shift right for the number of offset bits.
    VPN from all levels combined is needed for caching the Virtual Page Number (VPN) to Physical
    Frame Number (PFN) mapping in the TLB, see below.
    *
    * @param virtualAddress
    * @param mask
    * @param shift
    * @return unsigned int
    */
unsigned int PageTable::virtualAddressToVPN(unsigned int virtualAddress, unsigned int mask, unsigned int shift)
{
    unsigned int page = mask & virtualAddress;
    page = page >> shift;
    return page;
};

/**
 * @brief  Given a page table and a virtual address, return the mapping of virtual address to physical frame
    (in Map*) from the page table. You must have an appropriate return value for when the virtual page
    is not found (e.g. NULL if this is the first time the virtual page has been seen). Note that if
    you use a different data structure than the one proposed, this may return a different type, but
    the function name and idea should be the same. Similarly, If lookup_vpn2pfn was a method of the C++
    class PageTable, the function signature could change in an expected way: Map * PageTable::lookup_vpn2pfn
    (unsigned int virtualAddress). This advice should be applied to other page table functions as appropriate.
    *
    * @param pageTable
    * @param virtualAddress
    * @return Map*
    */
PageTable::Map *PageTable::lookup_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress)
{
    PageTable::Level *current = pageTable->root;
    if(virtualAddress == 323688384){
		std::cout << "Segmentation fault on line 156 at this virtual address: 323688384" << std::endl;
	}
    unsigned int levelNum;
    for (int i = 0; i < pageTable->levelCount - 1; i++)
    {
        levelNum = virtualAddressToVPN(virtualAddress, pageTable->bitMask[i], pageTable->bitShift[i]);
        if (current->nextLevel[levelNum] == nullptr)
        {
            return nullptr;
        }
        current = current->nextLevel[levelNum];
    }

    levelNum = virtualAddressToVPN(virtualAddress, pageTable->bitMask[pageTable->levelCount - 1], pageTable->bitShift[pageTable->levelCount - 1]);
    if (current->map[levelNum] != nullptr)
    {
        return current->map[levelNum];
    }
    else
    {
        return nullptr;
    }
}

/**
 * @brief Used to add new entries to the page table when we have discovered that a page has not yet been
    allocated (lookup_vpn2pfn returns NULL). Frame is the frame index which corresponds to the page number
    of the virtual address. Use a frame number of 0 the first time this is called, and increment the frame
    index by 1 each time a new page→frame map is needed. If you wish, you may replace void with int or bool
    and return an error code if unable to allocate memory for the page table. HINT: If you are inserting a
    page, you do not always add nodes at every level. The Map structure may already exist at some or all
    of the levels.
    *
    * @param pagetable
    * @param virtualAddress
    * @param frame
    */
PageTable::Map *PageTable::insert_vpn2pfn(PageTable *pagetable, unsigned int virtualAddress, unsigned int frame)
{
    PageTable::Level *current = pagetable->root;
    unsigned int levelNum;
    int depth = 0;
    unsigned int *pages;

    for (int i = 0; i < pagetable->levelCount - 1; i++)
    {
        levelNum = virtualAddressToVPN(virtualAddress, pagetable->bitMask[i], pagetable->bitShift[i]);
        pages[i] = levelNum;
        if (current->nextLevel[levelNum] == nullptr)
        {
            current->nextLevel[levelNum] = new PageTable::Level(pagetable, depth);
        }
        current = current->nextLevel[levelNum];
        depth++;
    }

    levelNum = virtualAddressToVPN(virtualAddress, pagetable->bitMask[pagetable->levelCount - 1], pagetable->bitShift[pagetable->levelCount - 1]);
    current->map[levelNum] = new PageTable::Map(pagetable, PageTable::calcPFN(pagetable, virtualAddress, frame), frame, pages);
    return current->map[levelNum];
}

unsigned int PageTable::calcPFN(PageTable *pagetable, unsigned int vAddr, unsigned int frame)
{
    unsigned int combined = (frame << pagetable->offsetSize) | vAddr & pagetable->offsetMask;
    return combined;
}
