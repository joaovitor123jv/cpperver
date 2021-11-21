CC=g++

SOURCE_FOLDER=src
BIN_NAME=server

MAIN_FILE=main.cpp

CLEAN_DELETE_COMMAND=rm -f

OSFLAG:=
ifeq ($(OS), Windows_NT)
	BIN_NAME=server.exe
	OSFLAG += -D WIN32
	CLEAN_DELETE_COMMAND=rm
	LINKERS += -lws2_32
	ifeq ($(PROCESSOR_ARCHITECTURE), AMD64)
		OSFLAG += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE), x86)
		OSFLAG += -D IA32
	endif
else
# Operatyng system
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Linux)
		OSFLAG += -D LINUX
	endif
	ifeq ($(UNAME_S), Darwin)
		OSFLAG += -D OSX
	endif
# Processor type
		UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		OSFLAG += -D AMD64
	endif
		ifneq ($(filter %86, $(UNAME_P)),)
	OSFLAG += -D IA32
		endif
	ifneq ($(filter arm%, $(UNAME_P)),)
		OSFLAG += -D ARM
	endif
endif



all: $(BIN_NAME)
	echo "Finalizado"

$(BIN_NAME): $(SOURCE_FOLDER)/$(MAIN_FILE)
	$(CC) $^ -Wall -o $(BIN_NAME) $(LINKERS) $(OSFLAG)

server_debug: $(SOURCE_FOLDER)/$(MAIN_FILE)
	$(CC) $^ -Wall -g -o debug_$(BIN_NAME) $(LINKERS) $(OSFLAG)

test: 
	rm -f server_debug
	make server_debug
	valgrind ./server_debug


clean:
	$(CLEAN_DELETE_COMMAND) $(BIN_NAME)