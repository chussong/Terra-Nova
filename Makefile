CXX = g++

SDL_LIB = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf
SDL_INCLUDE = -I/usr/include/SDL2/

CXXFLAGS = -Wall -Wextra -pedantic -c -std=c++14 $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
EXECUTABLE = TerraNova

SOURCES = main.cpp person.cpp colony.cpp map.cpp game.cpp gamewindow.cpp
OBJECTS = main.o person.o colony.o map.o game.o gamewindow.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $+ $(LDFLAGS) -o $@

main.o: main.cpp person.hpp colony.hpp map.hpp game.hpp gamewindow.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

person.o: person.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

colony.o: colony.cpp person.hpp templates.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

map.o: map.cpp colony.hpp templates.hpp gamewindow.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

game.o: game.cpp person.hpp colony.hpp map.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

gamewindow.o: gamewindow.cpp map.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o && rm -f $(EXECUTABLE)
