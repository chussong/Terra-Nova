#include "gamevars.hpp"
#include "game.hpp"
#include "file.hpp"
#include "random.hpp"

TTF_Font* defaultFont;
TTF_Font* dialogueFont;
TTF_Font* uiFont;

int main(){
	File::Initialize();
	Random::Initialize();

	Game Game1;
	Game1.Begin();
	return EXIT_SUCCESS;
}
