CC=g++

SOURCE_FOLDER=src

MAIN_FILE=main.cpp


server: $(SOURCE_FOLDER)/$(MAIN_FILE)
	$(CC) $^ -Wall -o server

server_debug: $(SOURCE_FOLDER)/$(MAIN_FILE)
	$(CC) $^ -Wall -g -o server_debug

test: 
	rm -f server_debug
	make server_debug
	valgrind ./server_debug