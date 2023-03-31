/*
CS 480 - 1001: Spring 2023
A3: Virtual Memory with TLB Cache
Cody Crawford: 824167663
Caleb Greenfield:
*/

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <vector>

#define DEFAULT_LEVEL_COUNT 3
#define DEFAULT_BITS_PER_LEVEL 8

using namespace std;

class PageTable
{
public:
   class Level;
   class Map;

   class Level
   {

   public:
      /**
       * @brief Pointer to the page table that contains this level
       */
      PageTable *pageTable;
      /**
       * @brief current depth
       */
      int depth;
      /**
       * @brief Maximum number of entries in this level
       */
      unsigned int entries;

      std::vector<Level *> nextLevel;
      std::vector<Map *> map;

      Level(PageTable *pageTable, int depth);
      ~Level();
   };

   class Map
   {

   public:
      PageTable *pageTable;

      unsigned int mapping;

      unsigned int frame;

      unsigned int *pages;

      unsigned int offset;

      Map(PageTable *pageTable, unsigned int mapping, unsigned int frame, unsigned int *pages);
      ~Map();
   };

   PageTable();
   PageTable(unsigned int shifts[], std::vector<int> sizes, int levels, int addressSize);
   ~PageTable();

   unsigned int createMask(int numOfmaskBits, int shift);
   unsigned int virtualAddressToVPN(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
   PageTable::Map *lookup_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress);
   PageTable::Map *insert_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress, unsigned int frame);
   unsigned int calcPFN(PageTable *pagetable, unsigned int vAddr, unsigned int frame);

   /**
    * @brief Pointer to the root of the page tree
    */
   Level *root;
   /**
    * @brief Number of levels
    */
   int levelCount;
   /**
    * @brief bitmask[i] bit mask for level i
    */
   unsigned int *bitMask;
   /**
    * @brief bitShift[i]: # of bits to shift level i page bits
    */
   unsigned int *bitShift;
   /**
    * @brief entryCount[i]: number of possible pages for level i
    */
   unsigned int *entryCount;
   /**
    * @brief levelSizes[i]: size of level i in bits
    */
   int *levelSizes;

   int offsetSize;
   unsigned int offsetMask;

   unsigned int bytesUsed;

   Level *nullLevel;
   Map *nullMap;

   /**
    * @brief Recursively removes the node and all of its children
    *
    * @param root Root nodeof subtree to be removed
    */
   void remove(Level *root);

private:
};

#endif
