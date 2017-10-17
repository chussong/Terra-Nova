CXX = clang++

SDL_LIB = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
SDL_INCLUDE = -I/usr/include/SDL2/

CXXFLAGS = -Wall -Wextra -pedantic -g -c -std=c++14 $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB) -lboost_system -lboost_filesystem
EXECUTABLE = TerraNova

SOURCES = main.cpp unit.cpp colony.cpp map.cpp game.cpp gamescreen.cpp \
		  sprite.cpp gfxobject.cpp ui.cpp tile.cpp building.cpp path.cpp \
		  dialogue.cpp ai.cpp random.cpp file.cpp event.cpp menu.cpp \
		  window.cpp audio.cpp cutscene.cpp
OBJECTS = main.o unit.o colony.o map.o game.o gamescreen.o sprite.o \
		  gfxobject.o ui.o tile.o building.o path.o dialogue.o ai.o random.o \
		  file.o event.o menu.o window.o audio.o cutscene.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $+ $(LDFLAGS) -o $@

main.o: main.cpp game.hpp gamevars.hpp window.hpp random.hpp file.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

unit.o: unit.cpp unit.hpp gfxobject.hpp gamevars.hpp path.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

colony.o: colony.cpp colony.hpp templates.hpp gamevars.hpp faction.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

map.o: map.cpp map.hpp templates.hpp gamevars.hpp tile.hpp path.hpp random.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

game.o: game.cpp game.hpp templates.hpp exceptions.hpp unit.hpp building.hpp \
		colony.hpp map.hpp gamescreen.hpp gamevars.hpp ai.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

gamescreen.o: gamescreen.cpp gamescreen.hpp gfxobject.hpp ui.hpp unit.hpp \
		building.hpp tile.hpp colony.hpp map.hpp gamevars.hpp dialogue.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

sprite.o: sprite.cpp sprite.hpp templates.hpp gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

gfxobject.o: gfxobject.cpp gfxobject.hpp sprite.hpp gamescreen.hpp gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

ui.o: ui.cpp ui.hpp gfxobject.hpp gamevars.hpp unit.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

tile.o: tile.cpp tile.hpp gfxobject.hpp unit.hpp building.hpp gamevars.hpp \
		colony.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

building.o: building.cpp building.hpp unit.hpp tile.hpp gfxobject.hpp \
		gamevars.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

path.o: path.cpp path.hpp map.hpp templates.hpp sprite.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

dialogue.o: dialogue.cpp dialogue.hpp ui.hpp unit.hpp file.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

random.o: random.cpp random.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

ai.o: ai.cpp ai.hpp random.hpp unit.hpp map.hpp tile.hpp colony.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

file.o: file.cpp file.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

event.o: event.cpp event.hpp file.hpp dialogue.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

menu.o: menu.cpp menu.hpp screen.hpp sprite.hpp ui.hpp audio.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

cutscene.o: cutscene.cpp cutscene.hpp screen.hpp sprite.hpp ui.hpp audio.hpp \
		dialogue.hpp file.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

window.o: window.cpp window.hpp audio.hpp screen.hpp menu.hpp gamescreen.hpp \
		cutscene.hpp game.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

audio.o: audio.cpp audio.hpp file.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o && rm -f $(EXECUTABLE)
