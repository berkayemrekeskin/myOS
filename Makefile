all: clean compile run
compile: main.cpp files.cpp shell.cpp
	@g++ -o main main.cpp files.cpp shell.cpp regular_file.cpp linked_file.cpp directory_file.cpp -std=c++11
run:
	@valgrind --leak-check=full ./main
clean: 
	@rm -f *.o