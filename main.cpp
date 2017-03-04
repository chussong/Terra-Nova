#include <iostream>

#include "person.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "game.hpp"
#include "gamewindow.hpp"

int main(){
	game game1;
	std::shared_ptr<map> palaven = game1.CreateMap();
	palaven->AddColony(game1.CreateColony(palaven, 50, 50));
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
/*	std::cout << "I'm taking " << jaffa->TakeResource(FOOD, 20) << " food, " <<
		jaffa->TakeResource(CARBON, 140) << " carbon, " <<
		jaffa->TakeResource(SILICON, -40) << " silicon, and " <<
		jaffa->TakeResource(IRON, 60) << " iron from " << jaffa->Name() << "." <<
		" Its stores are now:" << std::endl;
	std::cout <<"Food: " << jaffa->Resource(FOOD) << std::endl <<
		"Carbon: " << jaffa->Resource(CARBON) << std::endl <<
		"Silicon: " << jaffa->Resource(SILICON) << std::endl <<
		"Iron: " << jaffa->Resource(IRON) << std::endl;*/

	jaffa->SetResourceIncome(SILICON, 20);

	game1.Window()->MakeColonyScreen(jaffa);
//	colonyWindow win(jaffa, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT);
//	win.AddObject(spriteDir + "Space-Colony.png", 0, 0);
//	win.AddObject(spriteDir + "resources.png", SCREEN_WIDTH-400, SCREEN_HEIGHT-100);
//	win.AddObject(spriteDir + "terrain.png", 0, SCREEN_HEIGHT-100);
//	win.AddObject(spriteDir + "colonist.png", 110, SCREEN_HEIGHT-60);

	SDL_Event e;
	bool quit = false;
	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:				quit = true;
											break;
				case SDL_KEYDOWN:			quit = true;
											break;
				case SDL_MOUSEBUTTONDOWN:	if(e.button.button == SDL_BUTTON_LEFT){
												entity* obj = 
													game1.Window()->ClickedObject(
															e.button.x,
															e.button.y);
												if(obj && (obj->Select() == NEXT_TURN))
													game1.NextTurn();
											}
											break;
			}
		}
		game1.Window()->Render();
	}
	return EXIT_SUCCESS;
}
