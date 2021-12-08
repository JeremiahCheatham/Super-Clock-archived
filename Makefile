CC ?= gcc
CCFLAGS ?= `sdl2-config --cflags` -ggdb3 -O0 --std=c99 -Wall -Wextra -Wwrite-strings
LDFLAGS := `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
TESTFLAGS := -fsanitize=leak -fsanitize=address -fsanitize=undefined
TARGET := superclock-sdl
SOURCES := $(TARGET).c

$(TARGET):
	$(CC) $(LDFLAGS) $(CCFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

rebuild:
	$(clean)
	$(CC) $(LDFLAGS) $(CCFLAGS) $(SOURCES) -o $(TARGET)

test:
	$(clean)
	$(CC) $(LDFLAGS) $(CCFLAGS) $(TESTFLAGS) $(SOURCES) -o $(TARGET)
