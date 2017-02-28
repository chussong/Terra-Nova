#include "game.hpp"

bool game::Tick(){
	return true;
}

std::shared_ptr<person> game::CreatePerson(){
	std::shared_ptr<person> newPerson(std::make_shared<person>());
	people.push_back(newPerson);
	return newPerson;
}

std::shared_ptr<colony> game::CreateColony(){
	std::shared_ptr<colony> newColony(std::make_shared<colony>());
	colonies.push_back(newColony);
	return newColony;
}

std::shared_ptr<map> game::CreateMap(){
	std::shared_ptr<map> newMap(std::make_shared<map>());
	maps.push_back(newMap);
	return newMap;
}
