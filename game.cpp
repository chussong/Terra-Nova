#include "game.hpp"

game::game(){
	win = std::make_shared<gameWindow>("Terra Nova", 100, 100, 1024, 768);
	ReadUnitTypes();
	ReadBuildingTypes();
}

void game::Begin(){
	std::shared_ptr<map> palaven = CreateMap();
	palaven->AddColony(CreateColony(palaven, 50, 50));
	std::shared_ptr<colony> aurora = palaven->Colony(0);
	aurora->AddResource(FOOD, 60);
	aurora->AddResource(CARBON, 120);
	aurora->AddResource(IRON, -20);
	aurora->AddInhabitant(CreatePerson(110, SCREEN_HEIGHT - 60));
	aurora->AddInhabitant(CreatePerson(200, SCREEN_HEIGHT - 60));
	std::shared_ptr<person> urist = aurora->Inhabitant(0);
	urist->TakeDamage(20);
	std::shared_ptr<person> urist2 = aurora->Inhabitant(1);
	urist2->ChangeName("Commander","Lin");
	urist2->ChangeSpec(unitTypes[2]);

	bool quit = false;
	screentype_t nextScreen = COLONY_SCREEN;
	while(!quit){
		switch(nextScreen){
			case COLONY_SCREEN: nextScreen = ThrowToColonyScreen(aurora);
								break;
			case MAP_SCREEN:	nextScreen = ThrowToMapScreen(palaven, 50, 50);
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

// read exported file listing unit specs; this is a placeholder
void game::ReadUnitTypes(){
	int idsUsed = 0;
	unitTypes.clear();
	std::shared_ptr<unitSpec> newSpec;

	newSpec = std::make_shared<unitSpec>(idsUsed++, "Colonist", 100 , 2);
	unitTypes.push_back(newSpec);

	newSpec = std::make_shared<unitSpec>(idsUsed++, "Marine", 200, 4);
	unitTypes.push_back(newSpec);

	newSpec = std::make_shared<unitSpec>(idsUsed++, "Commander", 300, 0);
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

std::shared_ptr<person> game::CreatePerson(const int x, const int y){
	if(unitTypes.size() < 1){
		std::cerr << "Error: attempted to create a person but no unit types "
			<< "have been loaded." << std::endl;
		return nullptr;
	}
	std::shared_ptr<person> newPerson(std::make_shared<person>(Window()->Renderer(),
				unitTypes[0], x, y));
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
	std::shared_ptr<map> newMap(std::make_shared<map>(Window()->Renderer()));
	maps.push_back(newMap);
	return newMap;
}

void game::NextTurn(){
	std::cout << "A new turn dawns..." << std::endl;
	for(unsigned int i = 0; i < colonies.size(); ++i) colonies[i]->ProcessTurn();
}
