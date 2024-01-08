all: clean compile run
compile: src/main.cpp src/files.cpp src/shell.cpp src/regular_file.cpp src/linked_file.cpp src/directory_file.cpp
	@g++ -o os src/main.cpp src/files.cpp src/shell.cpp src/regular_file.cpp src/linked_file.cpp src/directory_file.cpp -std=c++11 -Wall -Wextra -Werror -g
run:
	@ -valgrind --leak-check=full --show-leak-kinds=all ./os
clean: 
	@rm -f *.o