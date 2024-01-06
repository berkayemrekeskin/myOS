all: clean compile run
compile: main.cpp files.cpp shell.cpp regular_file.cpp linked_file.cpp directory_file.cpp
	@g++ -o os main.cpp files.cpp shell.cpp regular_file.cpp linked_file.cpp directory_file.cpp -std=c++11
run:
	@valgrind --leak-check=full ./os
clean: 
	@rm -f *.o