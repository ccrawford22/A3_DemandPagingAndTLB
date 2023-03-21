
#include <string>
#include <cstdio>
#include <iostream>
#include "vaddr_tracereader.h"
#include <getopt.h>
#include <vector>
#include <iomanip>
#include "page_table.h"

#define DEFAULT_NUMOF_MEM_REFERENCES 1000  //FIXME: Should read all addresses from file
#define DEFAULT_NUMOF_TLB_ENTRIES 0
#define DEFAULT_PRINTMODE "summary"
#define ADDRESS_SIZE 32




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
		 //get level sizes, calculate sum        
        std::vector<int> levelSizes;
        idx++;
		int VPNSize = 0;
		while (idx < argc){
			levelSizes.push_back(atoi(argv[idx]));
			VPNSize = VPNSize + atoi(argv[idx]);
			idx++;
		}
		if (VPNSize > ADDRESS_SIZE){
			std::cout << "Sum of levels is greater than address size" << std::endl;
			exit(1);
		}
		
		int sum = 0;
		unsigned int shiftAry[levelSizes.size()];
		int numLevels = sizeof(shiftAry)/sizeof(shiftAry[0]);
		for(int i = 0; i <numLevels; i++){
			sum = sum + levelSizes[i];
			shiftAry[i] = ADDRESS_SIZE-sum;
		}
		//create new pageTable
		PageTable *pageTable = new PageTable(shiftAry, levelSizes, numLevels, ADDRESS_SIZE);
	
		//open trace file
		FILE *tracef_h;
		tracef_h = fopen(traceFile, "rb");
		
		unsigned int addy;

		if (!tracef_h) {
			//error opening file
			std::cout << "Unable to open <<" << traceFile << ">>" << std::endl;
			exit(1);
		}else{
			//read trace file
			p2AddrTr mtrace;
			unsigned int vAddr;
			unsigned int frame = 0;
			for(int i = 0; i < 10; i++){
				NextAddress(tracef_h, &mtrace);  //tracef_h - file handle from fopen 
				vAddr = mtrace.addr;
				pageTable -> insert_vpn2pfn(pageTable, vAddr, frame);
				printf("Addr: 0x%08x\n", vAddr);
				frame++;
			}
			
		}
		
		
		fclose(tracef_h);
		return (0);
	}else{
		std::cout << "No arguments to process" << std::endl;
	}
		
	
		
		
		
}
