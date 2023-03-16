# CXX Make variable for compiler
CXX = g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CXXFLAGS = -std=c++11 -Wall -g3 -c

# object files
OBJS = map_tree.o page_table.o translation_lookaside_buffer.o bitmasking-demo.o

# Program name
PROGRAM = mmuwithtlb

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $^

map_tree.o: map_tree.h map_tree.cpp
	$(CXX) $(CXXFLAGS) map_tree.cpp

page_table.o: map_tree.o page_table.h page_table.cpp 
	$(CXX) $(CXXFLAGS) page_table.cpp

translation_lookaside_buffer.o: translation_lookaside_buffer.h translation_lookaside_buffer.cpp
	$(CXX) $(CXXFLAGS) translation_lookaside_buffer.cpp

bitmasking-demo.o : map_tree.o page_table.o translation_lookaside_buffer.o bitmasking-demo.cpp
	$(CXX) $(CXXFLAGS) bitmasking-demo.cpp
    
clean :
	rm -f *.o *.exe mmuwithtlb