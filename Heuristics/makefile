HEADERS = ../Execute/src

#If the program that executes heuristics was compiled with -DCOMPILER_SUPPORT
DEFINE='-DCOMPILER_SUPPORT'

CXX     = g++
INCLUDE = -I$(HEADERS)
CFLAGS  = -Wall -O2 -fPIC -std=c++11 $(INCLUDE) $(DEFINE)

LINK    = g++
LIBS    = 
LFLAGS  = -shared $(LIBS)

SOURCES =  $(wildcard src/*.cpp)
OUTPUTS =  $(patsubst src/%.cpp,bin/%.so,$(SOURCES))


.PRECIOUS: obj/%.o


.PHONY: all
all: folders $(OUTPUTS)

.PHONY: folders
folders: bin obj

bin:
	mkdir -p bin

obj:
	mkdir -p obj

bin/%.so: obj/%.o
	$(LINK) $^ -o $@ $(LFLAGS)

obj/%.o: src/%.cpp $(HEADERS)/Heuristic.hpp
	$(CXX) $< -o $@ -c $(CFLAGS)

$(HEADERS)/Heuristic.hpp: $(HEADERS)/InstructionFlags.hpp $(HEADERS)/Types.hpp
	touch $@


.PHONY: clean
clean:
	rm -rf obj
	rm -rf bin
