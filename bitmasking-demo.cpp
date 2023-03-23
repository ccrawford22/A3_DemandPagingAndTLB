#define BADFLAG 1
#define NORMALEXIT 0

#include <iostream>
#include <unistd.h>
#include <fstream>

#include "page_table.h"
#include "translation_lookaside_buffer.h"
#include "vaddr_tracereader.h"

// Constants and Macros
string USAGE = " trace_file n_bits_for_level_0 ["
               " n_bits_for_level_1... n_bits_for_level_k"
               " -n number_of_memory_accesses_to_process "
               " -c TLB_cache_capacity "
               " -p print_mode (levelbitmasks, va2pa, va2pa_tlb_ptwalk, vpn2prn, offset, summary) "
               " ]";
/**
 * @brief levelbitnmasks: Write out the bitmasks for each level starting with
      the lowest tree level (root node is at level 0), one per line.
      In this mode, you do not need to actually process any addresses.
      Program prints bitmasks and exits. (Use report_levelbitmasks.)
 */
#define LEVEL_BIT_MASKS "levelbitmasks"
/**
 * @brief va2pa – Show virtual address translation to physical address for every
      address, one address translation per line.
      (Use report_virtualAddr2physicalAddr.)
 */
#define VA2PA "va2pa"
/**
 * @brief va2pa_tlb_ptwalk - Show virtual to physical translation for every address,
      lookup TLB then pagetable walk if TLB misses, one address translation per line.
      (Use report_va2pa_TLB_PTwalk.)
 */
#define VA2PA_TLB_PTWALK "va2pa_tlb_ptwalk"
/**
 * @brief vpn2pfn – For every virtual address, show its virtual page numbers for each
      level followed by the frame number, one address per line.
      (Use report_pagetable_map.)
 */
#define VPN2PFN "vpn2pfn"
/**
 * @brief offset – Show offsets of virtual addresses (excluding the virtual page number),
      one address offset per line. (Use hexnum.)
 */
#define OFFSET "offset"
/**
 * @brief summary – Show summary statistics. This is the default argument if -p is not
      specified. (Use report_summary.) Statistics reported include the page size, number
      of addresses processed, hit and miss rates for tlb and pagetable walk,
      number of frames allocated, total bytes required for page table (hint: use sizeof).
      You should get a roughly accurate estimate of the total bytes used for the page table
      including data used in all page tree levels. Note your calculated number may not
      match the number of total bytes in sample_output.txt (should be close though), as you
      may not have strictly the same data members in your structures as in the solution code,
      which is fine. But you should be aware that in general, with more paging levels, less
      total bytes would normally be used.
 */
#define SUMMARY "summary"

void errorUsage(string appName)
{
  cerr << "Usage: " << appName << " trace_file n_bits_for_level_0 ["
                                  " n_bits_for_level_1... n_bits_for_level_k"
                                  " -n number_of_memory_accesses_to_process "
                                  " -c TLB_cache_capacity "
                                  " -p print_mode "
                                  " ]"
       << endl;
}

/* demo of bit masking and shifting */
int main(int argc, char **argv)
{
  // CHECK USAGE
  /* Example Call:
      trace.tr 12 8
          Constructs a 2 level page table with 12 bits for level 0, and 8 bits
          for level 1. The remaining 12 bits would be for the offset in each page.
          This invocation does not simulate TLB (as -c is not specified).
          Process addresses from the entire file (as -n is not specified) and
          output the summary (as -p is not specified).
  */
  if (argc < 3)
  {
    errorUsage(argv[0]);
    exit(BADFLAG);
  }

  int option;           /* command line switch */
  int idx;              // general purpose index variable
  int n = 0;            // number of memory traces (set to 0 to print all mem traces by default)
  int c = 0;            // Cache capacity of the TLB (set to 0 for NO TLB caching by default)
  string p = "summary"; // print mode (set to "summary" by default)
  bool verbose = false;
  string filename;
  /* Handle Optional Arguments */
  while ((option = getopt(argc, argv, "vn:c:p:")) != -1)
  {
    /*
       If the option has an argument, optarg is set to point to the
       argument associated with the option.
     */
    switch (option)
    {
    case 'n': /* Assume this takes a number */
              /* -n N
                  Process only the first N memory accesses / references. Processes
                  all addresses if not present.
        
                  Error handling:
                  • If an out-of-range number (< 0) is specified,
                    print to the standard output (or standard error stderr):
                    Number of memory accesses must be a number, greater than or equal to 0
                    then exit.
              */
      if (atoi(optarg) < 0)
      {
        cout << endl
             << "Number of memory accesses must be a number, greater than "
                "or equal to 0"
             << endl;
        exit(BADFLAG);
      }
      n = atoi(optarg);
      break;
    case 'c': /* Assume this takes a number */
              /* -c N Cache capacity of the TLB
              i.e., max number of page mapping entries (N) in TLB.
              Default is 0 if not specified, meaning NO TLB caching.
        
              Error handling:
              • If an out-of-range number (< 0) is specified,
                print to the standard output (or standard error stderr):
                Cache capacity must be a number, greater than or equal to 0
                then exit.
              */
      if (atoi(optarg) < 0)
      {
        cout << endl
             << "Cache capacity must be a number, greater than or equal to 0"
             << endl;
        exit(BADFLAG);
      }
      c = atoi(optarg);
      break;
    case 'p':
      /* -p N print mode.
              Mode is a string that specifies what to be printed to the standard output:
              levelbitmasks – Write out the bitmasks for each level starting with
              the lowest tree level (root node is at level 0), one per line. In this mode,
              you do not need to actually process any addresses. Program prints bitmasks and
              exits. (Use report_levelbitmasks.)

              va2pa – Show virtual address translation to physical address for every address,
              one address translation per line. (Use report_virtualAddr2physicalAddr.)

              va2pa_tlb_ptwalk - Show virtual to physical translation for every address,
              lookup TLB then pagetable walk if TLB misses, one address translation per line.
              (Use report_va2pa_TLB_PTwalk.)

              vpn2pfn – For every virtual address, show its virtual page numbers for each
              level followed by the frame number, one address per line. (Use report_pagetable_map.)

              offset – Show offsets of virtual addresses (excluding the virtual page number),
              one address offset per line. (Use hexnum.)

              summary – Show summary statistics. This is the default argument if -p is not specified. (Use report_summary.) Statistics reported include the page size, number of addresses processed, hit and miss rates for tlb and pagetable walk, number of framesallocated, total bytes required for page table (hint: use sizeof). You should get
              a roughly accurate estimate of the total bytes used for the page table including
              data used in all page tree levels. Note your calculated number may not match the
              number of total bytes in sample_output.txt (should be close though), as you may
              not have strictly the same data members in your structures as in the solution code,
              which is fine. But you should be aware that in general, with more paging levels,
              less total bytes would normally be used.
      */
      p = optarg;
      if (p != LEVEL_BIT_MASKS &&
          p != VA2PA &&
          p != VA2PA_TLB_PTWALK &&
          p != VPN2PFN &&
          p != OFFSET &&
          p != SUMMARY)
      {
        // print usage and exit
        errorUsage(argv[0]);
        exit(BADFLAG); // BADFLAG is an error # defined in a header
      }
      p = optarg;
      break;
    case 'v': /* optarg is undefined */
      cout << "-v argument is present, turning on verbose mode" << endl;
      verbose = true;
      break;

    default:
      // print usage and exit
      errorUsage(argv[0]);
      exit(BADFLAG); // BADFLAG is an error # defined in a header
    }
  }

  // HANDLE MANDATORY ARGUMENTS
  idx = optind;

  // If idx < argc, there are mandatory arguments to process
  if (idx < argc)
  {

    // Open the trace file
    ifstream trace_file(argv[idx]);
    if (!trace_file.is_open())
    {
      cerr << "Unable to open " << argv[idx] << endl;
      return 1;
    }

    // Parse the number of bits for each level
    int bits[3];
    for (int i = 0; i < 3; i++)
    {
      bits[i] = atoi(argv[2 + i]);
      if (bits[i] < 1)
      {
        cerr << "Level " << i << " page table must be at least 1 bit" << endl;
        return 1;
      }
    }

    // Check if the total number of bits is within the limit
    if (bits[0] + bits[1] + bits[2] > 28)
    {
      cerr << "Too many bits used in page tables" << endl;
      return 1;
    }

    // TODO: I believe the vaddr_tracereader.cpp file has some examples of how to process the trace file
    // See example code posted below, in C
    //     FILE *ifp;	        /* trace file */
    // unsigned long i = 0;  /* instructions processed */
    // p2AddrTr trace;	/* traced address */
    // /* check usage */
    // if(argc != 2) {
    //   fprintf(stderr,"usage: %s input_byutr_file\n", argv[0]);
    //   exit(1);
    // }
    // /* attempt to open trace file */
    // if ((ifp = fopen(argv[1],"rb")) == NULL) {
    //   fprintf(stderr,"cannot open %s for reading\n",argv[1]);
    //   exit(1);
    // }
    // while (!feof(ifp)) {
    //   /* get next address and process */
    //   if (NextAddress(ifp, &trace)) {
    //     AddressDecoder(&trace, stdout);
    //     i++;
    //     if ((i % 100000) == 0)
    // fprintf(stderr,"%dK samples processed\r", i/100000);
    //   }
    // }
    // /* clean up and return success */
    // fclose(ifp);
    // return (0);

    // TODO: Process the trace file
  }

  // continue to process more mandatory arguments
  // if there are any left
  if (verbose && idx < argc)
  {
    cout << "Recieved " << argc - idx << " additional arguments that will be ignored:" << endl;
    for (int i = idx; i < argc; i++)
    {
      cout << "\tArg " << i << " = " << argv[i] << endl;
    }
  }

  PageTable *pTable = new PageTable(3, 8);

  cout << "Created Page Table with " << pTable->getLevelCount() << " levels." << endl;

  delete pTable;
  pTable = nullptr;

  return 0;
}

void bitMaskDemo()
{

  int page;

  /* Suppose that we have a three level page table:
   * Level 0:  4 bits
   * Level 1:  8 bits
   * Level 2:  8 bits
   *
   * offset is 32 - (4 + 8 + 8) = 12 bits
   * page size is 2^12 = 4096 bytes
   */

  int address = 0xFe98f982;

  /* Suppose we are interested in the level 1 page table index:
   * We would have to construct a bit mask where the following bits
   * are set:
   *
   * 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
   * 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   * ===============================================================
   * 0 0 0 0|1 1 1 1|1 1 1 1|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0
   * or in hexadecimal:
   *    0   |   F   |   F   |   0   |   0   |   0   |   0   |   0
   *
   * You will have to construct this in an algorithmic fashion, for
   * demonstration purposes we'll just use a constant.
   */
  unsigned int mask = 0x0FF00000;

  /* Note that since we stored Address as an int instead of an
   * unsigned int, when we print in decimal we think that this is an
   * negative number if bit 31 is set.  When printing in hexadecimal,
   * all numbers are assumed unsigned and it won't make any
   * difference.  (If we had declared this as an unsigned int, it
   * would have printed as positive integer.)
   */
  printf("Address:\t0x%08x (%d decimal)\n", address, address);

  /* Pull out the relevant bits by bit-wise and */
  page = mask & address;
  printf("After masking:\t0x%08x\n", page);

  /* Shift right by the relevant number of bits */
  page = page >> 20;

  printf("Page number = 0x%x\n", page);

  /* example to build a mask 0xF0*/
  unsigned int aMask = 1;
  unsigned int numOfMaskBits = 4;
  /* after the following loop, aMask will become 0xF */
  for (size_t b = 1; b < numOfMaskBits; b++)
  {
    aMask = aMask << 1;
    aMask = aMask | 1;
  }

  // Shift into appropriate position, for this example, shift to left by 4 bits
  int leftShift = 4;
  aMask = aMask << leftShift; // aMask will become 0xF0
}
