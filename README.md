Demand Paging with Multilevel Page Table and TLB

This README file provides an overview of the programming assignment focusing on demand paging with a multilevel page table and Translation Lookaside
Buffer (TLB). The assignment involves simulating the virtual memory management scheme of demand paging, where pages are loaded and mapped from 
persistent storage to memory only when required.

Functionality

The program aims to simulate demand paging using a multilevel page tree/table with address translation caching using a TLB. Here are the key functionalities:

1. Initialization: Upon starting, the program creates an empty page table, allocating only the level 0 (root) node.

2. Address Translation: The program reads logical/virtual addresses one at a time from an input trace file. For each address, the program simulates 
   the Memory Management Unit (MMU) and the demand paging process of the Operating System. The following steps are performed:

   a. Extract the full virtual page number (VPN) and search the TLB for the Virtual Page Number (VPN) -> Physical Frame Number (PFN) mapping information.

   b. If the mapping is found in the TLB, use the corresponding PFN for translation.

   c. If the mapping is not found in the TLB (TLB miss), walk the page table tree:
      - Search the page table for the mapping.
      - If the mapping is found in the page table, insert the mapping into the TLB cache. If the TLB is full, apply cache replacement using an approximation of 
        the LRU (Least Recently Used) policy.
      - If the mapping is not in the page table, insert the page into the page tree/table with an assigned frame index. The frame index simulates the demand 
        paging allocation of a physical frame to the virtual page brought into memory. (Note: The assignment assumes an infinite number of frames available, so no page replacement algorithm is needed.)

3. **Cache Hit Counters**: The program keeps track of cache hit counters based on the following scenarios:
   - If the page mapping is found in the TLB cache, increment the cache hit counter.
   - If the page mapping is not found in the TLB (TLB miss) and then found in the page table, increment the page table hit counter.

4. **Output and Statistics**: The program prints appropriate outputs to the standard output as specified in the "User Interface" section of the assignment. 
The total number of address accesses is calculated based on the formula: TLB cache hits + page table hits + page table misses.

5. **Frame Number Allocation**: Frame numbers (PFN) are assigned sequentially, starting from 0. Initially, the page table is empty. When the first virtual address 
is read from the trace file, the corresponding VPN is inserted into the multilevel page table using the specified number of bits for each level. At the leaf 
level/node, frame 0 is assigned to the first VPN. The frame number is incremented for each subsequent VPN inserted into the page table.

Note: It is recommended to implement the multilevel paging without TLB first and then add the TLB simulation. Several autograding tests focus on the multilevel 
paging without TLB functionality.

Conclusion

This programming assignment focuses on simulating demand paging with a multilevel page table and TLB. The program reads logical/virtual addresses from a trace 
file, performs address translation, and simulates the demand paging process. The TLB is used to cache page mappings, and appropriate counters are maintained to 
track cache hits and page table hits. The program outputs relevant information and statistics to the standard output. It is advised to implement multilevel paging 
without TLB initially and then incorporate the TLB simulation.
