SOURCEFILES := chip8.c
HEADERFILES := chip8.h
SOURCES := $(addprefix src/, $(SOURCEFILES))
HEADERS := $(addprefix src/, $(HEADERFILES))
SDL = `sdl-config --cflags --libs`

CC := gcc
CFLAGS := -g -Wall -std=c99
OBJECTS := $(addprefix obj/, $(SOURCEFILES:.c=.o))
EXECUTABLE := chipemu

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(SDL) $(OBJECTS) -o $@

#Force recompile if associated header changes
obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) $(SDL) -c $< -o $@

#Overloaded object compiler for .c files without headers
obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(SDL) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) src/*~
