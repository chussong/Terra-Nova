#include <iostream>

#include "person.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "game.hpp"
#include "rendering.hpp"

int main(){
	game game1;
	std::shared_ptr<map> palaven = game1.CreateMap();
	palaven->AddColony(game1.CreateColony());
	std::shared_ptr<colony> jaffa = palaven->Colony(0);
	jaffa->AddResource(FOOD, 60);
	jaffa->AddResource(CARBON, 120);
	jaffa->AddResource(IRON, -20);
	std::cout << "We've generated a colony called " << jaffa->Name() <<
		". The following resources have been added to its stores:" << std::endl
		<< "Food: " << jaffa->Resource(FOOD) << std::endl <<
		"Carbon: " << jaffa->Resource(CARBON) << std::endl <<
		"Silicon: " << jaffa->Resource(SILICON) << std::endl <<
		"Iron: " << jaffa->Resource(IRON) << std::endl;
	jaffa->AddInhabitant(game1.CreatePerson());
	jaffa->AddInhabitant(game1.CreatePerson());
	std::shared_ptr<person> urist = jaffa->Inhabitant(0);
	urist->TakeDamage(20);
	std::shared_ptr<person> urist2 = jaffa->Inhabitant(1);
	urist2->ChangeName("Urist","McMiner");
	urist2->ChangeSpec("Miner");
	std::cout << "We've added an inhabitant called " << urist->Name() << ". "
		<< urist->NomPronCap() << " is a " << urist->Spec() << " with " <<
		urist->Health() << "/" << urist->MaxHealth() << " health." << std::endl;
	std::cout << "We've added an inhabitant called " << urist2->Name() << ". "
		<< urist2->NomPronCap() << " is a " << urist2->Spec() << " with " <<
		urist2->Health() << "/" << urist2->MaxHealth() << " health.\n";
	std::cout << "I'm taking " << jaffa->TakeResource(FOOD, 20) << " food, " <<
		jaffa->TakeResource(CARBON, 140) << " carbon, " <<
		jaffa->TakeResource(SILICON, -40) << " silicon, and " <<
		jaffa->TakeResource(IRON, 60) << " iron from " << jaffa->Name() << "." <<
		" Its stores are now:" << std::endl;
	std::cout <<"Food: " << jaffa->Resource(FOOD) << std::endl <<
		"Carbon: " << jaffa->Resource(CARBON) << std::endl <<
		"Silicon: " << jaffa->Resource(SILICON) << std::endl <<
		"Iron: " << jaffa->Resource(IRON) << std::endl;

	RenderingInit();

	SDL_Window* win = CreateWindow("Terra Nova", 100, 100, SCREEN_WIDTH,
			SCREEN_HEIGHT);
	SDL_Renderer* ren = CreateRenderer(win);
	std::string spriteDir = GetSpritePath("sprites");
	SDL_Texture* background = LoadTexture(spriteDir + "Space-Colony.png", ren);
	SDL_Texture* resourceGFX = LoadTexture(spriteDir + "resources.png", ren);
	SDL_Texture* colonistGFX = LoadTexture(spriteDir + "colonist.png", ren);
	SDL_Texture* terrainGFX = LoadTexture(spriteDir + "terrain.png", ren);

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) quit = true;
			if(e.type == SDL_KEYDOWN) quit = true;
			if(e.type == SDL_MOUSEBUTTONDOWN) quit = true;
		}
		SDL_RenderClear(ren);
		RenderTexture(background, ren, 0, 0);
		RenderTexture(resourceGFX, ren, SCREEN_WIDTH - 400, SCREEN_HEIGHT - 100);
		RenderTexture(terrainGFX, ren, 0, SCREEN_HEIGHT - 100);
		RenderTexture(colonistGFX, ren, 110, SCREEN_HEIGHT - 60);
		SDL_RenderPresent(ren);
	}

	SDL_Cleanup(ren, win);
	RenderingQuit();
	return EXIT_SUCCESS;
}
