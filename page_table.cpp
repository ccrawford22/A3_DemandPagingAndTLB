#include <climits>

#include "page_table.h"

PageTable::PageTable()
{
}

PageTable::~PageTable()
{
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
unsigned int virtualAddressToVPN(unsigned int virtualAddress, unsigned int mask, unsigned int shift)
{
    return UINT_MAX;
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
Map *lookup_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress)
{
    return nullptr;
};

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
void insert_vpn2pfn(PageTable *pagetable, unsigned int virtualAddress, unsigned int frame);
