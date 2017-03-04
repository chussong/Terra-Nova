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
	jaffa->AddInhabitant(game1.CreatePerson());
	jaffa->AddInhabitant(game1.CreatePerson());
	std::shared_ptr<person> urist = jaffa->Inhabitant(0);
	urist->TakeDamage(20);
	std::shared_ptr<person> urist2 = jaffa->Inhabitant(1);
	urist2->ChangeName("Urist","McMiner");
	urist2->ChangeSpec("Miner");

	jaffa->SetResourceIncome(SILICON, 20);

	jaffa->MakeColonyScreen(game1.Window());

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
												std::shared_ptr<entity> obj = 
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
