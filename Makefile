CC 	= gcc
CFLAGS 	= -Wall -c -g

LIBS 	= -lncurses -ljsmn -lm
LIBDIR	= lib
INCLUDE	= include

TARGET 	= build/rl
SOURCES = src/main.c src/util.c
SOURCES += src/player/player.c
SOURCES += src/worldmap/worldmap.c
SOURCES += src/worldmap/commands.c
SOURCES += src/worldmap/load_level.c
SOURCES += src/worldmap/tiles/tiles.c
SOURCES += src/worldmap/entity/entity.c
SOURCES += src/interface/interface.c
SOURCES += src/list/list.c
OBJECTS	= $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p build
	$(CC) -o $@ $(OBJECTS) -L$(LIBDIR) $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $<

.PHONY: clean
clean: 
	rm -rf $(OBJECTS) $(TARGET)
test: all
	./$(TARGET) 2> stderr
