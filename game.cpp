#include "game.hpp"

game::game(){
	win = std::make_shared<gameWindow>("Terra Nova", 100, 100, 1024, 768);
	ReadBuildingTypes();
}

bool game::Tick(){
	return true;
}

// read exported file listing building types; this is a placeholder
void game::ReadBuildingTypes(){
	int idsUsed = 0;
	buildingTypes.clear();
	std::shared_ptr<buildingType> newType;
	std::array<int, 4> costs;
	std::vector<terrain_t> allowedTerrain;

	costs = {{0,60,40,40}};
	allowedTerrain.clear();
	allowedTerrain.push_back(PLAINS);
	newType = std::make_shared<buildingType>(idsUsed++, "Factory Farm", costs,3);
	newType->SetAllowedTerrain(allowedTerrain);
	newType->SetBonusResources({{4,0,0,0}});
	buildingTypes.push_back(newType);

	costs = {{0,20,60,80}};
	allowedTerrain.clear();
	allowedTerrain.push_back(MOUNTAIN);
	newType = std::make_shared<buildingType>(idsUsed++, "Automated Mine", costs,
			4);
	newType->SetAllowedTerrain(allowedTerrain);
	newType->SetMaxOccupants(0);
	newType->SetAutomatic(true);
	buildingTypes.push_back(newType);

	if(buildingTypes.size() > 100){
		std::cerr << "Error: only up to 100 building types are supported." << std::endl;
	}
}

std::vector<std::shared_ptr<buildingType>> game::BuildingTypes(){
	return buildingTypes;
}

std::shared_ptr<person> game::CreatePerson(const int x, const int y){
	std::shared_ptr<person> newPerson(std::make_shared<person>(Window()->Renderer(),
				GetSpritePath("sprites") + "colonist.png", x, y));
	people.push_back(newPerson);
	return newPerson;
}

std::shared_ptr<colony> game::CreateColony(std::shared_ptr<map> parentMap,
		const int row, const int colm){
	std::shared_ptr<colony> newColony(std::make_shared<colony>(Window()->Renderer(),
				parentMap, row, colm));
	newColony->SetBuildingTypes(buildingTypes);
	colonies.push_back(newColony);
	return newColony;
}

std::shared_ptr<map> game::CreateMap(){
	std::shared_ptr<map> newMap(std::make_shared<map>());
	maps.push_back(newMap);
	return newMap;
}

void game::NextTurn(){
	std::cout << "A new turn dawns..." << std::endl;
	for(unsigned int i = 0; i < colonies.size(); ++i) colonies[i]->ProcessTurn();
	if(colonies.size() > 0) colonies[0]->MakeColonyScreen(win); //obviously debug only
}
