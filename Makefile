# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS=-std=c++11 -Wall -g -g3 -c

# object files
OBJS = main.o vaddr_tracereader.o page_table.o map_tree.o

# Program name
PROGRAM = a3

all: $(PROGRAM)

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)

main.o : main.cpp
	$(CC) -g $(CCFLAGS) main.cpp
	
vaddr_tracereader.o : vaddr_tracereader.c vaddr_tracereader.h
	$(CC) $(CCFLAGS) vaddr_tracereader.c
	
page_table.o : page_table.cpp page_table.h
	$(CC) $(CCFLAGS) page_table.cpp
	
map_tree.o : map_tree.cpp map_tree.h
	$(CC) $(CCFLAGS) map_tree.cpp
	
debug: $(PROGRAM)
	gdb $(PROGRAM)

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)
	
	
