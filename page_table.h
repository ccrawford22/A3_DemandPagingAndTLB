#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

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
      unsigned int depth;
      /**
       * @brief Maximum number of entries in this level
       */
      unsigned int entryCount;

      Level **nextLevel;
      Map **map;

      Level(PageTable *pageTable, unsigned int depth);
      ~Level();
   };

   class Map
   {

   public:
      Map();
      ~Map();

   private:
   };

   PageTable(unsigned int levelCount, unsigned int bitsPerLevel);
   ~PageTable();

   unsigned int virtualAddressToVPN(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
   Map *lookup_vpn2pfn(unsigned int virtualAddress);
   void insert_vpn2pfn(unsigned int virtualAddress, unsigned int frame);

   unsigned int getLevelCount();

private:
   /**
    * @brief Pointer to the root of the page tree
    */
   Level *root;
   /**
    * @brief Number of levels
    */
   unsigned int levelCount;
   /**
    * @brief bitmask[i] bit mask for level i
    */
   unsigned int *bitmask;
   /**
    * @brief bitShift[i]: # of bits to shift level i page bits
    */
   unsigned int *bitShift;
   /**
    * @brief entryCount[i]: number of possible pages for level i
    */
   unsigned int *entryCount;
};

#endif