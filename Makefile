CXX = clang++

SDL_LIB = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf
SDL_INCLUDE = -I/usr/include/SDL2/

CXXFLAGS = -Wall -Wextra -pedantic -g -c -std=c++14 $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
EXECUTABLE = TerraNova

SOURCES = main.cpp unit.cpp colony.cpp map.cpp game.cpp gamewindow.cpp \
		  sprite.cpp gfxobject.cpp ui.cpp tile.cpp building.cpp path.cpp \
		  dialogue.cpp
OBJECTS = main.o unit.o colony.o map.o game.o gamewindow.o sprite.o \
		  gfxobject.o ui.o tile.o building.o path.o dialogue.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $+ $(LDFLAGS) -o $@

main.o: main.cpp game.hpp gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

unit.o: unit.cpp unit.hpp gfxobject.hpp gamevars.hpp path.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

colony.o: colony.cpp colony.hpp templates.hpp building.hpp gamevars.hpp \
		faction.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

map.o: map.cpp map.hpp templates.hpp gamevars.hpp tile.hpp path.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

game.o: game.cpp game.hpp templates.hpp exceptions.hpp unit.hpp building.hpp \
		colony.hpp map.hpp gamewindow.hpp gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

gamewindow.o: gamewindow.cpp gamewindow.hpp gfxobject.hpp ui.hpp unit.hpp \
		building.hpp tile.hpp colony.hpp map.hpp gamevars.hpp dialogue.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

sprite.o: sprite.cpp sprite.hpp templates.hpp gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

gfxobject.o: gfxobject.cpp gfxobject.hpp sprite.hpp gamewindow.hpp gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

ui.o: ui.cpp ui.hpp gfxobject.hpp gamevars.hpp unit.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

tile.o: tile.cpp tile.hpp gfxobject.hpp unit.hpp building.hpp gamevars.hpp \
		colony.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

building.o: building.cpp building.hpp unit.hpp tile.hpp gfxobject.hpp \
		gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

path.o: path.cpp path.hpp map.hpp templates.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

dialogue.o: dialogue.cpp dialogue.hpp ui.hpp unit.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o && rm -f $(EXECUTABLE)
