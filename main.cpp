/*
CS 480 - 1001: Spring 2023
A3: Virtual Memory with TLB Cache
Cody Crawford: 824167663
Caleb Greenfield:
*/

#define BADFLAG 1
#define NORMALEXIT 0

#include <string>
#include <cstdio>
#include <iostream>
#include "vaddr_tracereader.h"
#include <getopt.h>
#include <vector>
#include <iomanip>
#include "page_table.h"
#include "print_helpers.h"
#include <cmath>

#define DEFAULT_NUMOF_MEM_REFERENCES 1000 // FIXME: Should read all addresses from file
#define DEFAULT_NUMOF_TLB_ENTRIES 0
#define DEFAULT_PRINTMODE "summary"
#define ADDRESS_SIZE 32

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

int main(int argc, char **argv)
{
    int option;
    int idx;

    // define default values
    int n = DEFAULT_NUMOF_MEM_REFERENCES;
    int c = DEFAULT_NUMOF_TLB_ENTRIES;
    std::string p = DEFAULT_PRINTMODE;

    // get optional arguments using getopt
    while ((option = getopt(argc, argv, "n:c:p:")) != -1)
    {
        switch (option)
        {
        case 'n':
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

        case 'c':
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

        default:
            // print usage and exit
            errorUsage(argv[0]);
            exit(BADFLAG); // BADFLAG is an error # defined in a header
            break;
        }
    }

    idx = optind;

    /* If idx < argc, there are mandatory arguments to process */
    if (idx < argc)
    {
        // get file name
        const char *traceFile = argv[idx];

        // opt arg error handling
        if (n < 0)
        {
            std::cout << "Number of memory accesses must be a number, greater than or equal to 0" << std::endl;
            exit(1);
        }

        if (c < 0)
        {
            std::cout << "Cache capacity must be a number, greater than or equal to 0" << std::endl;
            exit(1);
        }

        // get level sizes, calculate sum
        // get level sizes, calculate sum
        std::vector<int> levelSizes;
        idx++;
        int VPNSize = 0;
        while (idx < argc)
        {
            levelSizes.push_back(atoi(argv[idx]));
            VPNSize = VPNSize + atoi(argv[idx]);
            idx++;
        }
        if (VPNSize > ADDRESS_SIZE)
        {
            std::cout << "Sum of levels is greater than address size" << std::endl;
            exit(1);
        }

        int sum = 0;
        unsigned int shiftAry[levelSizes.size()];
        int numLevels = sizeof(shiftAry) / sizeof(shiftAry[0]);
        for (int i = 0; i < numLevels; i++)
        {
            sum = sum + levelSizes[i];
            shiftAry[i] = ADDRESS_SIZE - sum;
        }
        // create new pageTable
        PageTable *pageTable = new PageTable(shiftAry, levelSizes, numLevels, ADDRESS_SIZE);

        if (p == LEVEL_BIT_MASKS)
        {
            report_levelbitmasks(numLevels, pageTable->bitMask);
        }

        // open trace file
        FILE *tracef_h;
        tracef_h = fopen(traceFile, "rb");

        if (!tracef_h)
        {
            // error opening file
            std::cout << "Unable to open <<" << traceFile << ">>" << std::endl;
            exit(1);
        }
        else
        {
            // read trace file
            p2AddrTr mtrace;
            unsigned int vAddr;
            unsigned int frame = 0;
            int pageHit = 0;
            int pageMiss = 0;
            int cacheHit = 0;
            bool cHit;
            bool pHit;
            PageTable::Map *map;
            for (int i = 0; i < 10; i++)
            {
                NextAddress(tracef_h, &mtrace); // tracef_h - file handle from fopen
                vAddr = mtrace.addr;
                cHit = false;
                pHit = false;
                // look in cache

                // look in pageTable
                map = pageTable->lookup_vpn2pfn(pageTable, vAddr);
                if (map == nullptr)
                {
                    map = pageTable->insert_vpn2pfn(pageTable, vAddr, frame);
                    frame++;
                    pageMiss++;
                }
                else
                {
                    pageHit++;
                    pHit = true;
                }

                if (p == VA2PA)
                {
                    report_virtualAddr2physicalAddr(vAddr, map->mapping);
                }
                else if (p == VA2PA_TLB_PTWALK)
                {
                    report_va2pa_TLB_PTwalk(vAddr, map->mapping, cHit, pHit);
                }
                else if (p == VPN2PFN)
                {
                    report_pagetable_map(pageTable->levelCount, map->pages, map->frame);
                }
                else if (p == OFFSET)
                {
                    hexnum(map->mapping);
                }
            }

            if (p == SUMMARY)
            {
                report_summary(pow(2, pageTable->offsetSize), cacheHit, pageHit, cacheHit + pageHit + pageMiss, frame, 10);
            }
        }

        fclose(tracef_h);
        return (0);
    }
    else
    {
        std::cout << "No arguments to process" << std::endl;
    }
}