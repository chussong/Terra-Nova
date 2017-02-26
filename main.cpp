#include <iostream>

#include "person.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "game.hpp"

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
	return EXIT_SUCCESS;
}
