SOURCEFILES := chip8.c loadrom.c processor.c disassembler.c
HEADERFILES := chip8.h
DISSOURCE := disassembler.c chipdis.c
CHASM := asmbin/chasm
SOURCES := $(addprefix src/, $(SOURCEFILES))
HEADERS := $(addprefix src/, $(HEADERFILES))
LINKER = -lGL -lglut

CC := gcc
CFLAGS := -g -Wall -std=c99
OBJECTS := $(addprefix obj/, $(SOURCEFILES:.c=.o))
DISOBJECTS := $(addprefix obj/, $(DISSOURCE:.c=.o))
EXECUTABLE := chipemu
DISASSEMBLER := chipdis

all: $(SOURCES) $(EXECUTABLE) $(CHASM) $(DISASSEMBLER)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LINKER) $(OBJECTS) -o $@

$(DISASSEMBLER): $(DISOBJECTS) obj/loadrom.o
	$(CC) $(CFLAGS) $(DISOBJECTS) -o $@

$(CHASM): asmbin/src/chasm.cpp
	g++ asmbin/src/chasm.cpp -o $@

#Force recompile if associated header changes
obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) $(LINKER) -c $< -o $@

#Overloaded object compiler for .c files without headers
obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(LINKER) -c $< -o $@

test: $(SOURCES) $(EXECUTABLE) $(CHASM) $(DISASSEMBLER)
	python tests/runsuite.py

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) src/*~ $(CHASM) $(DISOBJECTS) $(DISASSEMBLER)
