#include "gamevars.hpp"
#include "game.hpp"
#include "file.hpp"
#include "random.hpp"

#include "window.hpp"

TTF_Font* defaultFont;
TTF_Font* dialogueFont;
TTF_Font* uiFont;

int main(){
	File::Initialize();
	Random::Initialize();

	Window win("Terra Nova", 128, 128, SCREEN_WIDTH, SCREEN_HEIGHT);
	win.InputLoop();

	//Game Game1;
	//Game1.Begin();
	return EXIT_SUCCESS;
}
