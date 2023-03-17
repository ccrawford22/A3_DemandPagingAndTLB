
#include <string>
#include <cstdio>
#include <iostream>
#include "vaddr_tracereader.h"
#include <getopt.h>
#include <vector>
#include <iomanip>

#define DEFAULT_NUMOF_MEM_REFERENCES 1000  //FIXME: Should read all addresses from file
#define DEFAULT_NUMOF_TLB_ENTRIES 0
#define DEFAULT_PRINTMODE "summary"
#define ADDRESS_SIZE 32




unsigned int virtualAddressToVPN(unsigned int virtualAddress, unsigned int mask, unsigned int shift){
	unsigned int page = mask & virtualAddress;
	page = page >> shift;
	return page;
}




int main(int argc, char **argv){
	int option; 
	int idx; 
	
	//define default values
	int n = DEFAULT_NUMOF_MEM_REFERENCES;    
	int c = DEFAULT_NUMOF_TLB_ENTRIES;  
	std::string p = DEFAULT_PRINTMODE;  
	
	//get optional arguments using getopt
	while ( (option = getopt(argc, argv, "n:c:p:")) != -1) {
		switch (option) {
		case 'n': 
			n = atoi(optarg);
			break;

		case 'c':
			c = atoi(optarg);
			break;

		case 'p':
			p = optarg;
			break;

		default:
			break;
		}

	}

	idx = optind;

	/* If idx < argc, there are mandatory arguments to process */
	if (idx < argc) {
		//get file name
		const char * traceFile = argv[idx];
		
		//opt arg error handling
		if (n < 0){
			std::cout << "Number of memory accesses must be a number, greater than or equal to 0" << std::endl;
			exit(1);
		}
		
		if (c<0){
			std::cout << "Cache capacity must be a number, greater than or equal to 0" << std::endl;
			exit(1);
		}
		
		if (p == "levelbitmasks"){
			//report_levelbitmasks
		}else if (p == "va2pa"){
			//report_virtualAddr2physicalAddr
		}else if (p == "va2pa_tlb_ptwalk"){
			//report_va2pa_TLB_PTwalk
		}else if (p == "vpn2pfn"){
			//report_pagetable_map
		}else if (p == "offset"){
			//hexnum
		}else{
			//report_summary
		}
		
		//get level sizes, calculate sum	
		std::vector<int> levelSizes;	
		idx++;
		int VPNBits = 0;
		while (idx < argc){
			levelSizes.push_back(atoi(argv[idx]));
			VPNBits = VPNBits + atoi(argv[idx]);
			idx++;
		}
		
		//create mask
		unsigned int mask = 1;
		unsigned int numOfMaskBits = VPNBits;
		int offsetBits = ADDRESS_SIZE-VPNBits;
		
		for (int b = 1; b < numOfMaskBits; b++)
		{
			mask = mask << 1;
			mask = mask | 1;
		}
		//shift left by number of offset bits
		mask = mask << offsetBits;
	
		
		std::cout << "Mask: " << std::hex << mask << std::endl;
	
		//open trace file
		FILE *tracef_h;
		tracef_h = fopen(traceFile, "rb");

		if (!tracef_h) {
			//error opening file
			std::cout << "Unable to open <<" << traceFile << ">>" << std::endl;
			exit(1)
		}else{
			//read trace file
			p2AddrTr mtrace;
			unsigned int vAddr;
			unsigned int VPN;
			for(int i = 0; i < 10; i++){
				NextAddress(tracef_h, &mtrace);  //tracef_h - file handle from fopen 
				vAddr = mtrace.addr;
				printf("Addr: 0x%08x ", vAddr);
				VPN = virtualAddressToVPN(vAddr, mask, offsetBits);
				printf("VPN: 0x%08x\n", VPN);
			}
		}
		
		fclose(tracef_h);
		return (0);
	}else{
		std::cout << "No arguments to process" << std::endl;
	}
		
	
		
		
		
}
