PASS      =
SUBFOLDER =

CXX     = g++ #../Scripts/compilation/g++
INCLUDE = -I./common
CFLAGS  = -Wall $(INCLUDE)

LINK    = g++#../Scripts/compilation/g++
#LINK    = g++
LIBS    = -lpthread
LFLAGS  = $(LIBS)

SOURCES =  $(wildcard src/test*.cpp)
OUTPUTS =  $(patsubst src/%.cpp,exe/$(SUBFOLDER)/%,$(SOURCES))

PASS_NAME:=$(PASS)
export PASS_NAME

.PRECIOUS: obj/$(SUBFOLDER)/%.o obj/$(SUBFOLDER)/main.o


.PHONY: all
all:
	make comp PASS=FunctionLimit SUBFOLDER=simple
#	make comp PASS=PostDom SUBFOLDER=postdom
#	make comp PASS=LongestPath SUBFOLDER=longpath
	#make comp PASS=Structured SUBFOLDER=structured


.PHONY: comp
comp: folders $(OUTPUTS)

.PHONY: folders
folders: exe/$(SUBFOLDER) obj/$(SUBFOLDER)

exe/$(SUBFOLDER):
	mkdir -p $@

obj/$(SUBFOLDER):
	mkdir -p $@

obj/$(SUBFOLDER)/main.o: common/main.cpp common/common.hpp
	$(CXX) $< -c -o $@ $(CFLAGS)

exe/$(SUBFOLDER)/%: obj/$(SUBFOLDER)/%.o obj/$(SUBFOLDER)/main.o
	$(LINK) $^ -o $@ $(LFLAGS)

obj/$(SUBFOLDER)/%.o: src/%.cpp common/common.hpp common/bigfun.cpp
	$(CXX) $< -c -o $@ $(CFLAGS)


.PHONY: clean
clean:
	rm -rf obj
	rm -rf exe
