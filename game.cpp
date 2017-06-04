#include "game.hpp"

game::game(){
	win = std::make_shared<gameWindow>("Terra Nova", 100, 100, 1024, 768);
	ReadAttackTypes();
	ReadUnitTypes();
	ReadBuildingTypes();
}

void game::Begin(){
	int temp_colony_row = 28;
	int temp_colony_colm = 76;

	std::shared_ptr<map> palaven = CreateMap();
	CreateColony(palaven, temp_colony_row, temp_colony_colm);
	std::shared_ptr<colony> aurora = palaven->Colony(0);
	aurora->AddResource(FOOD, 60);
	aurora->AddResource(CARBON, 120);
	aurora->AddResource(IRON, -20);
	aurora->AddInhabitant(CreatePerson(unitTypes[0], 1));
	aurora->AddInhabitant(CreatePerson(unitTypes[2], 1));
	std::shared_ptr<person> urist = aurora->Inhabitant(0);
	urist->TakeDamage(20);
	std::shared_ptr<person> urist2 = aurora->Inhabitant(1);
	urist2->ChangeName("Commander","Lin");

	std::shared_ptr<person> enemy = CreatePerson(unitTypes[1], 2);
	enemy->MoveToTile(palaven->Terrain(temp_colony_row, temp_colony_colm + 4));

	bool quit = false;
	screentype_t nextScreen = COLONY_SCREEN;
	while(!quit){
		switch(nextScreen){
			case COLONY_SCREEN: nextScreen = ThrowToColonyScreen(aurora);
								break;
			case MAP_SCREEN:	nextScreen = ThrowToMapScreen(palaven, 
										temp_colony_row, temp_colony_colm);
								break;
			case QUIT_SCREEN:	quit = true;
								break;
		}
	}
}

screentype_t game::ThrowToColonyScreen(std::shared_ptr<colony> col){
	while(true){
		switch(win->ColonyScreen(col)){
			case NEXT_TURN:		NextTurn();
								break;
			case SCREEN_CHANGE:	return MAP_SCREEN;
			case QUIT:			return QUIT_SCREEN;
			default:			return QUIT_SCREEN;
		}
	}
}

screentype_t game::ThrowToMapScreen(std::shared_ptr<map> theMap, int centerColm,
		int centerRow){
	while(true){
		switch(win->MapScreen(theMap, centerColm, centerRow)){
			case NEXT_TURN:		NextTurn();
								break;
			case SCREEN_CHANGE:	return COLONY_SCREEN;
			case QUIT:			return QUIT_SCREEN;
			default:			return QUIT_SCREEN;
		}
	}
}

bool game::Tick(){
	return true;
}

// read exported file listing attack types; this is a placeholder
void game::ReadAttackTypes(){
	attackTypes.clear();
	std::shared_ptr<attackType> newAttack;

	newAttack = std::make_shared<attackType>("Ballistic SMG", 10, 7, 0.67f);
	attackTypes.push_back(newAttack);
}

// read exported file listing unit specs; this is a placeholder
void game::ReadUnitTypes(){
	int idsUsed = 0;
	unitTypes.clear();
	std::shared_ptr<unitType> newSpec;
	std::vector<std::shared_ptr<attackType>> atks;

	newSpec = std::make_shared<unitType>(idsUsed++, "Colonist", 100, atks, 2);
	unitTypes.push_back(newSpec);

	atks.push_back(attackTypes[0]);
	newSpec = std::make_shared<unitType>(idsUsed++, "Marine", 200, atks, 4);
	unitTypes.push_back(newSpec);

	newSpec = std::make_shared<unitType>(idsUsed++, "Commander", 300, atks, 0);
	newSpec->SetCanRespec(false);
	unitTypes.push_back(newSpec);

	if(unitTypes.size() > 100){
		std::cerr << "Error: only up to 100 unit types are supported." << std::endl;
	}
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

	costs = {{0,20,20,40}};
	newType = std::make_shared<buildingType>(idsUsed++, "Academy", costs, 4);
	newType->SetCanHarvest(false);
	newType->SetCanTrain(unitTypes[1]);
	buildingTypes.push_back(newType);

	if(buildingTypes.size() > 100){
		std::cerr << "Error: only up to 100 building types are supported." << std::endl;
	}
}

std::vector<std::shared_ptr<buildingType>> game::BuildingTypes(){
	return buildingTypes;
}

std::shared_ptr<person> game::CreatePerson(const std::shared_ptr<unitType> spec, const char faction){
	if(unitTypes.size() < 1){
		std::cerr << "Error: attempted to create a person but no unit types "
			<< "have been loaded." << std::endl;
		return nullptr;
	}
	std::shared_ptr<person> newPerson(std::make_shared<person>(Window()->Renderer(),
				spec, faction));
	people.push_back(newPerson);
	return newPerson;
}

std::shared_ptr<colony> game::CreateColony(std::shared_ptr<map> parentMap,
		const int row, const int colm){
	if(row < 0 || colm < 0 || static_cast<unsigned int>(row) > parentMap->NumberOfRows() 
			|| static_cast<unsigned int>(colm) > parentMap->NumberOfColumns()){
		std::cerr << "Error: attempted to create a colony out of bounds.";
		return nullptr;
	}
	std::shared_ptr<colony> newColony(std::make_shared<colony>(Window()->Renderer(),
				parentMap->SurroundingTerrain(row, colm)));
	newColony->SetBuildingTypes(buildingTypes);
	parentMap->AddColony(newColony, row, colm);
	colonies.push_back(newColony);
	return newColony;
}

std::shared_ptr<map> game::CreateMap(){
	std::shared_ptr<map> newMap(std::make_shared<map>(Window()->Renderer()));
	maps.push_back(newMap);
	return newMap;
}

void game::NextTurn(){
	std::cout << "A new turn dawns..." << std::endl;
	for(unsigned int i = 0; i < colonies.size(); ++i) colonies[i]->ProcessTurn();
}
