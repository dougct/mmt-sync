llvmpath = $$HOME/llvm
folder_name = MyPasses


.PHONY: all
all: 
	make auxiliary
	make pass NAME=FunctionLimit
	make pass NAME=PostDom
	make pass NAME=Structured
	make pass NAME=LongestPath


.PHONY: auxiliary
auxiliary: pragmas.o setpragmas

pragmas.o: pragmas.c
	gcc -c pragmas.c -o pragmas.o -O2

setpragmas: setpragmas.cpp ../Tools/PragmasCodes.hpp
	g++ setpragmas.cpp -o setpragmas -O2



.PHONY: clean
clean:
	rm -f pragmas.o setpragmas
	make cleaning NAME=FunctionLimit
	make cleaning NAME=PostDom
	make cleaning NAME=Structured
	make cleaning NAME=LongestPath


this_lib_folder = $(llvmpath)/lib/$(folder_name)/$(NAME)
target = $(llvmpath)/Debug+Asserts/lib/$(NAME).so


.PHONY: pass
pass: $(target)

$(target): $(NAME)/$(NAME).cpp
	rm -rf $(this_lib_folder)
	mkdir -p $(this_lib_folder)
	cp ./$(NAME)/* $(this_lib_folder)
	cp ./shared/* $(this_lib_folder)
	cp ./makefile.specific $(this_lib_folder)/Makefile
	cd $(this_lib_folder) && make SPECIFIC=$(NAME)


.PHONY: cleaning
cleaning:
	rm -rf $(this_lib_folder)
	rm -f $(llvmpath)/Debug+Asserts/lib/$(NAME).so
