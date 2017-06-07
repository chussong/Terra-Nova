#include "game.hpp"

game::game(){
	win = std::make_shared<gameWindow>("Terra Nova", 100, 100, 1024, 768);
	try{
		ReadAttackTypes();
		ReadUnitTypes();
		ReadBuildingTypes();
	}
	catch(const readError &e){
		// instead of complaining, should generate the def files from hard-coded versions
		std::cout << e.what() << std::endl;
		throw;
	}
}

void game::Begin(){
	int temp_colony_row = 28;
	int temp_colony_colm = 76;

	std::shared_ptr<map> terraNova = CreateMap();
	std::shared_ptr<colony> aurora = CreateColony(terraNova, 
			temp_colony_row, temp_colony_colm, 1);
	aurora->AddResource(FOOD, 60);
	aurora->AddResource(CARBON, 120);
	aurora->AddResource(IRON, -20);
	std::shared_ptr<person> col = CreatePerson(
			terraNova->Terrain(aurora->Row(), aurora->Column()+2),
			unitTypes[0], 1);
	std::shared_ptr<person> lin = CreatePerson(
			terraNova->Terrain(aurora->Row()+1, aurora->Column()+1),
			unitTypes[2], 1);
	lin->ChangeName("Commander","Lin");

	std::shared_ptr<person> enemy = CreatePerson(
			terraNova->Terrain(aurora->Row(), aurora->Column()+4),
			unitTypes[1], 2);

	bool quit = false;
	screentype_t nextScreen = COLONY_SCREEN;
	while(!quit){
		switch(nextScreen){
			case COLONY_SCREEN: nextScreen = ThrowToColonyScreen(aurora);
								break;
			case MAP_SCREEN:	nextScreen = ThrowToMapScreen(terraNova, 
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

std::vector<std::string> game::LoadDefFile(const std::string& name){
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	std::vector<std::string> ret;

	static std::string baseDefDir;
	if(baseDefDir.empty()){
		char* basePath = SDL_GetBasePath();
		if(basePath){
			baseDefDir = basePath;
			baseDefDir += std::string("defs") + PATH_SEP;
			SDL_free(basePath);
		} else {
			std::cerr << "Error getting resource path: " << SDL_GetError()
				<< std::endl;
			return ret;
		}
		size_t pos = baseDefDir.rfind("bin");
		baseDefDir = baseDefDir.substr(0, pos);
	}

	std::ifstream in(baseDefDir + name);
	if((in.rdstate() & std::ifstream::failbit) != 0){
		std::cerr << "Error: unable to open necessary definition file at "
			<< baseDefDir + name << ". Does it exist?" << std::endl;
		return ret;
	}

	std::string line;
	do{
		std::getline(in, line);
		if(line.length() > 0 && line[0] != '%') ret.push_back(line);
	}while((in.rdstate() & std::ifstream::eofbit) == 0);
	return ret;
}

void game::ReadAttackTypes(){
	std::vector<std::string> attackDefs = LoadDefFile("weapons.txt");
	if(attackDefs.size() == 0){
		std::cerr << "This is bad but I don't have the exceptions working "
			<< "yet." << std::endl;
		return;
		//throw readError();
	}
	
	attackTypes.clear();
	std::shared_ptr<attackType> newAttack;
	std::vector<std::string> entries;
	
	for(auto& attack : attackDefs){
		boost::split(entries, attack, boost::is_any_of("|"));
		/*std::cout << entries[0] << " | " << boost::trim_copy(entries[0]) << std::endl;
		std::cout << entries[1] << " | " << std::stof(entries[1]) << std::endl;
		std::cout << entries[2] << " | " << std::stoi(entries[2]) << std::endl;
		std::cout << entries[3] << " | " << std::stoi(entries[3]) << std::endl;*/
		try{
			newAttack = std::make_shared<attackType>(boost::trim_copy(entries[0]),
						std::stof(entries[1]), std::stoi(entries[2]),
						std::stoi(entries[3]));
		}
		catch(const std::exception &e){
			std::cout << "An attack definition file was detected, but its "
				<< "contents were not in a readable format. Please fix it or "
				<< "delete it so that it can be regenerated." << std::endl;
			std::cout << e.what() << std::endl;
			throw;
		}
		/*std::cout << newAttack->Name() << " " << newAttack->Accuracy() << " "
			<< newAttack->AttackRate() << " " << newAttack->Damage() << std::endl;*/
		attackTypes.push_back(newAttack);
	}
}

void game::ReadUnitTypes(){
	std::vector<std::string> unitDefs = LoadDefFile("unit_types.txt");
	if(unitDefs.size() == 0){
		std::cerr << "This is bad but I don't have the exceptions working yet."
			<< std::endl;
		return;
		//throw readError();
	}
	
	unitTypes.clear();
	std::shared_ptr<unitType> newSpec;
	std::vector<std::string> entries;

	std::vector<std::shared_ptr<attackType>> atks;
	std::shared_ptr<attackType> atk;
	std::vector<std::string> atkNames;

	/*std::cout << "Going to loop over these " << unitDefs.size() << " lines:"
		<< std::endl;*/
	for(auto& line : unitDefs){
		//std::cout << line << std::endl;
		boost::split(entries, line, boost::is_any_of("|"));
		boost::split(atkNames, entries[3], boost::is_any_of(","));
		atks.clear();
		for(auto& name : atkNames){
			if((atk = FindByName(attackTypes, name)) != nullptr) atks.push_back(atk);
		}

		try{
			newSpec = std::make_shared<unitType>(boost::trim_copy(entries[0]),
					std::stoi(entries[1]), std::stoi(entries[2]), atks,
					std::stoi(entries[4]));
		}
		catch(const std::exception &e){
			std::cout << "A unit type definition file was detected, but its "
				<< "contents were not in a readable format. Please fix it or "
				<< "delete it so that it can be regenerated." << std::endl;
			std::cout << e.what() << std::endl;
			throw;
		}
		if(entries.size() > 5){
			if(entries[5].find("no respec") < std::string::npos)
				newSpec->SetCanRespec(false);
		}
		unitTypes.push_back(newSpec);
	}
}

// read exported file listing building types; this is a placeholder
void game::ReadBuildingTypes(){
	int idsUsed = 0;
	buildingTypes.clear();
	std::shared_ptr<buildingType> newType;
	std::array<int, 4> costs;
	std::vector<std::shared_ptr<tileType>> allowedTerrain;

	costs = {{0,60,40,40}};
	allowedTerrain.clear();
	//allowedTerrain.push_back(PLAINS);
	newType = std::make_shared<buildingType>(idsUsed++, "Factory Farm", costs,3);
	newType->SetAllowedTerrain(allowedTerrain);
	newType->SetBonusResources({{4,0,0,0}});
	buildingTypes.push_back(newType);

	costs = {{0,20,60,80}};
	allowedTerrain.clear();
	//allowedTerrain.push_back(MOUNTAIN);
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

void game::ReadTileTypes(){
	std::vector<std::string> terrainDefs = LoadDefFile("terrain.txt");
	std::string name;
	std::array<int, LAST_RESOURCE> yield;
	int moveCost;
	std::shared_ptr<tileType> newType;
	for(auto& defLine : terrainDefs){
		std::vector<std::string> defArgs;
		boost::split(defArgs, defLine, boost::is_any_of("|"));
		if(defArgs.size() < 3){
			std::cerr << "Error: terrain description incomplete." << std::endl;
			return;
		}

		name = boost::trim_copy(defArgs[0]);

		std::vector<std::string> yieldStrings;
		boost::split(yieldStrings, defArgs[1], boost::is_any_of(","));
		if(yieldStrings.size() != yield.size()){
			std::cerr << "Error: incorrect number of resource yields provided in "
				<< "definition of " << name << "." << std::endl;
			return;
		}
		try{
			for(unsigned int i = 0; i < yieldStrings.size(); ++i){
				yield[i] = std::stoi(yieldStrings[i]);
			}
		}
		catch(const std::invalid_argument &e){
			std::cerr << "Error: resource yield formatted incorrectly in "
				<< "definition of " << name << "." << std::endl;
			throw;
		}

		try{
			moveCost = std::stoi(defArgs[2]);
		}
		catch(const std::invalid_argument &e){
			std::cerr << "Error: movement cost not a number in definition of "
				<< name << "." << std::endl;
			throw;
		}

		newType = std::make_shared<tileType>(name, yield, moveCost);

		std::vector<std::string> attribs;
		boost::split(attribs, defArgs[3], boost::is_any_of(","));
		for(auto& att : attribs){
			boost::trim(att);
			if(att == "cold"){
				newType->SetCold(true);
				continue;
			}
			if(att == "wooded"){
				newType->SetWooded(true);
				continue;
			}
			if(att == "aquatic"){
				newType->SetAquatic(true);
				continue;
			}
		}

		tileTypes.push_back(newType);
	}
}

//void game::ReadMap();

std::shared_ptr<person> game::CreatePerson(const std::shared_ptr<tile> location,
		const std::shared_ptr<unitType> spec, const char faction){
	if(unitTypes.size() < 1){
		std::cerr << "Error: attempted to create a person but no unit types "
			<< "have been loaded." << std::endl;
		return nullptr;
	}
	std::shared_ptr<person> newPerson(std::make_shared<person>(Window()->Renderer(),
				spec, faction));
	if(!newPerson->MoveToTile(location)){
		std::cerr << "Error: attempted to create a new person but they were not "
			<< "able to occupy the given tile." << std::endl;
		return nullptr;
	}
	people.push_back(newPerson);
	return newPerson;
}

std::shared_ptr<colony> game::CreateColony(std::shared_ptr<map> parentMap,
		const int row, const int colm, const int faction){
	if(row < 0 || colm < 0 || static_cast<unsigned int>(row) > parentMap->NumberOfRows() 
			|| static_cast<unsigned int>(colm) > parentMap->NumberOfColumns()){
		std::cerr << "Error: attempted to create a colony out of bounds.";
		return nullptr;
	}
	std::shared_ptr<colony> newColony(std::make_shared<colony>(Window()->Renderer(),
				parentMap->SurroundingTerrain(row, colm), faction));
	newColony->SetBuildingTypes(buildingTypes);
	parentMap->AddColony(newColony, row, colm);
	colonies.push_back(newColony);
	return newColony;
}

std::shared_ptr<map> game::CreateMap(){
	ReadTileTypes();
	std::shared_ptr<map> newMap(std::make_shared<map>(Window()->Renderer(),
			tileTypes));
	maps.push_back(newMap);
	return newMap;
}

void game::NextTurn(){
	std::cout << "A new turn dawns..." << std::endl;
	for(unsigned int i = 0; i < colonies.size(); ++i) colonies[i]->ProcessTurn();
	for(auto& u : people) u->ProcessTurn();
}
