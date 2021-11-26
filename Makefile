CC := gcc
CCFLAGS := `sdl2-config --cflags` -ggdb3 -O0 --std=c99 -Wall
LDFLAGS := `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
TARGET := superclock-sdl
SOURCES := $(TARGET).c

$(TARGET):
	$(CC) $(LDFLAGS) $(CCFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

rebuild:
	$(clean)
	$(CC) $(LDFLAGS) $(CCFLAGS) $(SOURCES) -o $(TARGET)
