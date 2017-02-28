CXX = g++

SDL_LIB = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf
SDL_INCLUDE = -I/usr/include/SDL2/

CXXFLAGS = -Wall -Wextra -pedantic -c -std=c++14 $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
EXECUTABLE = TerraNova

SOURCES = main.cpp person.cpp colony.cpp map.cpp game.cpp rendering.cpp
OBJECTS = main.o person.o colony.o map.o game.o rendering.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $+ $(LDFLAGS) -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

person.o: person.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

colony.o: colony.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

map.o: map.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

game.o: game.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

rendering.o: rendering.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXECUTABLE)
