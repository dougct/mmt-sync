OUT_DIR = .
OBJ     = obj

#This version works with hardware-only heuristics, but it is slower.
DEFINE  = -DCOMPILER_SUPPORT

CXX     = g++
INCLUDE = 
CFLAGS  = -Wall -std=c++11 -O2 $(INCLUDE) $(DEFINE)

LINK    = g++
LIBS    = -ldl
LFLAGS  = -s $(LIBS)

OBJS    = $(OBJ)/Main.o $(OBJ)/Simulation.o $(OBJ)/DataAnalyser.o $(OBJ)/Thread.o $(OBJ)/StatisticCounter.o
OUTPUT  = $(OUT_DIR)/heuristic


.PHONY: all
all: $(OBJ) $(OUTPUT)

$(OBJ):
	mkdir -p $(OBJ)

$(OUTPUT): $(OBJS)
	$(LINK) $^ -o $@ $(LFLAGS)

$(OBJ)/Main.o: src/Main.cpp src/Simulation.hpp src/DataAnalyser.hpp src/Types.hpp
	$(CXX) $< -o $@ -c $(CFLAGS)

$(OBJ)/Simulation.o: src/Simulation.cpp src/Simulation.hpp src/DataAnalyser.hpp src/Thread.hpp src/Heuristic.hpp src/Types.hpp
	$(CXX) $< -o $@ -c $(CFLAGS)

$(OBJ)/DataAnalyser.o: src/DataAnalyser.cpp src/DataAnalyser.hpp src/Thread.hpp src/Types.hpp
	$(CXX) $< -o $@ -c $(CFLAGS)

$(OBJ)/Thread.o: src/Thread.cpp src/Thread.hpp src/Types.hpp
	$(CXX) $< -o $@ -c $(CFLAGS)

$(OBJ)/StatisticCounter.o: src/StatisticCounter.cpp src/StatisticCounter.hpp src/Types.hpp
	$(CXX) $< -o $@ -c $(CFLAGS)


.PHONY: clean
clean:
	rm -f $(OUTPUT)
	rm -rf $(OBJ)
