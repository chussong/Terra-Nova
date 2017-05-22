#include <iostream>
#include <memory>

#include "person.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "game.hpp"
#include "gamewindow.hpp"

int main(){
	game game1;
	std::shared_ptr<map> palaven = game1.CreateMap();
	palaven->AddColony(game1.CreateColony(palaven, 50, 50));
	std::shared_ptr<colony> aurora = palaven->Colony(0);
	aurora->AddResource(FOOD, 60);
	aurora->AddResource(CARBON, 120);
	aurora->AddResource(IRON, -20);
	aurora->AddInhabitant(game1.CreatePerson(110, SCREEN_HEIGHT - 60));
	aurora->AddInhabitant(game1.CreatePerson(200, SCREEN_HEIGHT - 60));
	std::shared_ptr<person> urist = aurora->Inhabitant(0);
	urist->TakeDamage(20);
	std::shared_ptr<person> urist2 = aurora->Inhabitant(1);
	urist2->ChangeName("Urist","McMiner");
	urist2->ChangeSpec("Miner");

	aurora->SetResourceIncome(SILICON, 20);

	aurora->MakeColonyScreen(game1.Window());

	SDL_Event e;
	bool quit = false;
	std::shared_ptr<entity> selected;
	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:				
					quit = true;
					break;
				case SDL_KEYDOWN:			
					quit = true;
					break;
				case SDL_MOUSEBUTTONDOWN:	
					if(e.button.button == SDL_BUTTON_LEFT){
						selected = game1.Window()->SelectedObject(e.button.x, e.button.y);
						if(selected){
							switch(selected->Select()/100){
								case NEXT_TURN/100:	game1.NextTurn();
													break;
								default:			break;
							}
						}
					}
					if(e.button.button == SDL_BUTTON_RIGHT && selected){
						std::shared_ptr<entity> obj =
							game1.Window()->ClickedObject(e.button.x, e.button.y);
						if(obj && std::dynamic_pointer_cast<person>(selected) &&
								std::dynamic_pointer_cast<tile>(obj)){
							aurora->AssignWorker(
									std::dynamic_pointer_cast<person>(selected),
									std::dynamic_pointer_cast<tile>(obj));
						}
						if(obj && std::dynamic_pointer_cast<buildingPrototype>(selected)
								&& std::dynamic_pointer_cast<tile>(obj)){
							aurora->EnqueueBuilding(
									std::dynamic_pointer_cast<buildingPrototype>(selected)->Type(),
									std::dynamic_pointer_cast<tile>(obj));
						}
					}
					break;
			}
		}
		game1.Window()->Render();
	}
	return EXIT_SUCCESS;
}
