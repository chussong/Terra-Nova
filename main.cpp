#include "gamevars.hpp"
#include "game.hpp"
#include "file.hpp"
#include "random.hpp"

#include "window.hpp"

TTF_Font* defaultFont;
TTF_Font* dialogueFont;
TTF_Font* uiFont;

int main(){
	TerraNova::File::Initialize();
	TerraNova::Random::Initialize();

	TerraNova::Window win("Terra Nova", 128, 128, SCREEN_WIDTH, SCREEN_HEIGHT);
	win.InputLoop();

	return EXIT_SUCCESS;
}
