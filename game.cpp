#include "game.hpp"

game::game(){
	win = std::make_shared<gameWindow>("Terra Nova", 100, 100, 1024, 768);
	try{
		ReadAttackTypes();
		ReadUnitTypes();
		ReadBuildingTypes();
		ReadTileTypes();
		ReadMap("1");
	}
	catch(const readError &e){
		// instead of complaining, should generate the def files from hard-coded versions
		std::cout << e.what() << std::endl;
		throw;
	}
}

void game::Begin(){
	/*int temp_colony_row = 28;
	int temp_colony_colm = 76;

	std::shared_ptr<map> terraNova = CreateMap();
	std::shared_ptr<colony> aurora = CreateColony(terraNova, 
			temp_colony_row, temp_colony_colm, "Aurora", 1);
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
			unitTypes[1], 2);*/

	StartTurn();

	bool quit = false;
	screentype_t nextScreen = COLONY_SCREEN;
	while(!quit){
		switch(nextScreen){
			case COLONY_SCREEN: nextScreen = ThrowToColonyScreen(colonies[0]);
								break;
			case MAP_SCREEN:	nextScreen = ThrowToMapScreen(maps[0], 4, 8);
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

screentype_t game::ThrowToMapScreen(std::shared_ptr<map> theMap, int centerRow,
		int centerColm){
	while(true){
		switch(win->MapScreen(theMap, centerRow, centerColm)){
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

		newType = std::make_shared<tileType>(name, yield);

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
			if(att == "hilly"){
				newType->SetHilly(true);
				continue;
			}
		}

		tileTypes.push_back(newType);
	}
}

void game::ReadMap(const std::string& mapName){
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	std::vector<std::string> lines;

	static std::string baseChapterDir;
	if(baseChapterDir.empty()){
		char* basePath = SDL_GetBasePath();
		if(basePath){
			baseChapterDir = basePath;
			baseChapterDir += std::string("chapters") + PATH_SEP;
			SDL_free(basePath);
		} else {
			std::cerr << "Error getting chapter path: " << SDL_GetError()
				<< std::endl;
			return;
		}
		size_t pos = baseChapterDir.rfind("bin");
		baseChapterDir = baseChapterDir.substr(0, pos);
	}

	std::ifstream in(baseChapterDir + mapName + PATH_SEP + "map.txt");
	if((in.rdstate() & std::ifstream::failbit) != 0){
		std::cerr << "Error: unable to open map file at "
			<< baseChapterDir + mapName + PATH_SEP + "map.txt"
			<< ". Does it exist?" << std::endl;
		return;
	}

	std::string line;
	do{
		std::getline(in, line);
		if(line.length() > 0 && line[0] != '%') lines.push_back(line);
	}while((in.rdstate() & std::ifstream::eofbit) == 0);

	std::map<char, std::shared_ptr<tileType>> tileDict;
	std::shared_ptr<map> newMap;
	bool runAgain = false;
	int runCount = 0;
	do{
		for(unsigned int i = 0; i < lines.size(); ++i){
			if(lines[i].compare(0, 14, "Terrain types:") == 0){
				tileDict = ParseTerrainDictionary(ExtractMapSection(lines, i));
			}
			if(lines[i].compare(0, 4, "Map:") == 0){
				if(tileDict.size() == 0){
					std::cout << "Map found but terrain types have not yet been "
						<< "defined. Will read to end of file and start again."
						<< std::endl;
					runAgain = true;
					continue;
				}
				newMap = std::make_shared<map>(Window()->Renderer(), 
						ParseMap(tileDict, ExtractMapSection(lines, i)) );
				maps.push_back(newMap);
			}
			if(lines[i].compare(0, 9, "Features:") == 0){
				if(!newMap){
					std::cout << "Features enumeration found but map has not yet been "
						<< "created. Will read to end of file and start again." 
						<< std::endl;
					runAgain = true;
					continue;
				}
				ParseFeatures(newMap, ExtractMapSection(lines, i));
			}
			if(lines[i].compare(0, 6, "Units:") == 0){
				if(!newMap){
					std::cout << "Units enumeration found but map has not yet been "
						<< "created. Will read to end of file and start again." 
						<< std::endl;
					runAgain = true;
					continue;
				}
				ParseUnits(newMap, ExtractMapSection(lines, i));
			}
		}
	} while(runAgain && runCount < 10);
}

std::vector<std::string> game::ExtractMapSection(
		const std::vector<std::string>& lines, const unsigned int i){
	unsigned int j;
	for(j = i+1; j < lines.size(); ++j){
		if(lines[j].find(':') < std::string::npos) break;
	}
	std::vector<std::string> ret;
	ret.resize(j - i - 1);
	for(unsigned int k = 0; k < ret.size(); ++k) ret[k] = lines[i+1+k];
	return ret;
}

std::map<char, std::shared_ptr<tileType>> game::ParseTerrainDictionary(
		const std::vector<std::string> desc){
	std::map<char, std::shared_ptr<tileType>> tileDic;
	for(auto& line : desc){
		tileDic.emplace(line[0], FindByName(tileTypes,
					boost::trim_copy(line.substr(line.find('|')+1)) ));
	}
	return tileDic;
}

std::string game::ParseDescLine(const std::string& line, const std::string& key){
	if(line.find(key) == std::string::npos) return "";
	return boost::trim_copy(line.substr(line.find('=') + 1));
}

std::vector<std::vector<std::shared_ptr<tile>>> game::ParseMap(
		const std::map<char, std::shared_ptr<tileType>> tileDict, 
		const std::vector<std::string> desc){
	std::vector<std::string> mapAsStrings;
	std::vector<std::vector<std::shared_ptr<tile>>> newTiles;
	for(auto& ln : desc){
		newTiles.resize(newTiles.size() + 1);
		boost::split(mapAsStrings, ln, boost::is_any_of(" "));
		for(unsigned int i = mapAsStrings.size()-1; i < mapAsStrings.size(); --i){
			if(mapAsStrings[i].size() == 0) mapAsStrings.erase(mapAsStrings.begin()+i);
		}
		newTiles[newTiles.size()-1].resize(2*mapAsStrings.size());
		for(unsigned int i = 0; i < mapAsStrings.size(); ++i){
			if(mapAsStrings[i].size() == 0) std::cerr << "Tile alias error!" << std::endl;
			newTiles[newTiles.size()-1][2*i + (newTiles.size()-1)%2]
				= std::make_shared<tile>(
						tileDict.find(mapAsStrings[i][0])->second, Window()->Renderer(),
						newTiles.size()-1, 2*i + (newTiles.size()-1)%2);
			//std::cout << " survived." << std::endl;
		}
	}
	return newTiles;
}

void game::ParseFeatures(std::shared_ptr<map> parentMap, 
		const std::vector<std::string> desc){
	unsigned int i = 0;
	unsigned int j = 0;
	while(i < desc.size()){
		if(desc[i].find("Colony") < std::string::npos){
			j = i+1;
			while(j < desc.size() && desc[j].find('}') == std::string::npos) j += 1;
			std::vector<std::string> colonyDesc;
			colonyDesc.resize(j-i-1);
			for(unsigned int k = 0; k < colonyDesc.size(); ++k){
				colonyDesc[k] = boost::trim_copy(desc[i+1+k]);
			}
			colonies.push_back(ParseColony(parentMap, colonyDesc));
			i = j+1;
			continue;
		}
		++i;
	}
}

std::shared_ptr<colony> game::ParseColony(std::shared_ptr<map> parentMap,
		const std::vector<std::string>& desc){
	std::string name("");
	int row = -1;
	int colm = -1;
	int owner = -1;
	for(auto& line : desc){
		if(line.compare(0, 4, "name") == 0){
			name = boost::trim_copy(line.substr(line.find('=') + 1));
			continue;
		}
		if(line.compare(0, 3, "pos") == 0){
			row = std::stoi(line.substr(line.find('=') + 1,
						line.find(',') - line.find('=') - 1));
			colm = std::stoi(line.substr(line.find(',') + 1));
			continue;
		}
		if(line.compare(0, 5, "owner") == 0){
			owner = std::stoi(line.substr(line.find('=') + 1));
			continue;
		}
	}
	if(name == "" || row == -1 || colm == -1 || owner == -1){
		std::cerr << "Error: identified a colony feature in map.txt which does "
			<< "not appear to have all of the information needed for "
			<< "instantiation." << std::endl;
		return nullptr;
	}
	return CreateColony(parentMap, row-1, 2*(colm-1) + (row-1)%2, name, owner);
}

void game::ParseUnits(std::shared_ptr<map> parentMap, 
		const std::vector<std::string>& desc){
	for(unsigned int i = 0; i < desc.size(); ++i){
		if(desc[i].find("Unique{") < std::string::npos){
			std::vector<std::string> unitDesc;
			for(unsigned int j = i+1; j < desc.size(); ++j){
				if(desc[j].find('}') < std::string::npos){
					unitDesc.resize(j-i-1);
					for(unsigned int k = 0; k < unitDesc.size(); ++k){
						unitDesc[k] = boost::trim_copy(desc[i+1+k]);
					}
					break;
				}
			}
			people.push_back(ParseUniqueUnit(parentMap, unitDesc));
			continue;
		}
		if(desc[i].find('{') < std::string::npos){
			std::shared_ptr<unitType> genericType;
			genericType = FindByName(unitTypes, desc[i].substr(0, desc[i].find('{')));
			if(!genericType){
				std::cerr << "Error: a unit entry appears to have been found "
					<< "which is neither unique nor a recognized generic type."
					<< std::endl;
				continue;
			}
			std::vector<std::string> unitDesc;
			for(unsigned int j = i+1; j < desc.size(); ++j){
				if(desc[j].find('}') < std::string::npos){
					unitDesc.resize(j-i-1);
					for(unsigned int k = 0; k < unitDesc.size(); ++k){
						unitDesc[k] = boost::trim_copy(desc[i+1+k]);
					}
					break;
				}
			}
			std::vector<std::shared_ptr<person>> genericUnits =
				ParseGenericUnits(parentMap, genericType, unitDesc);
			for(auto& gu : genericUnits) people.push_back(gu);
		}
	}
}

std::shared_ptr<person> game::ParseUniqueUnit(std::shared_ptr<map> parentMap,
		const std::vector<std::string>& unitDesc){
	std::string name = "";
	std::shared_ptr<unitType> spec;
	int row = -1;
	int colm = -1;
	int owner = -1;
	std::string descLine;
	for(auto& line : unitDesc){
		if((descLine = ParseDescLine(line, "name")) != ""){
			name = descLine;
		}
		if((descLine = ParseDescLine(line, "spec")) != ""){
			spec = FindByName(unitTypes, descLine);
		}
		if((descLine = ParseDescLine(line, "pos")) != ""){
			row = std::stoi(descLine.substr(0, descLine.find(',')));
			colm = std::stoi(descLine.substr(descLine.find(',') + 1));
		}
		if((descLine = ParseDescLine(line, "owner")) != ""){
			owner = std::stoi(descLine);
		}
	}
	if(name == "" || !spec || row == -1 || colm == -1 || owner == -1){
		std::cerr << "Error: found an entry for a unique unit but it did not "
			<< "have all of the required information (name, spec, position, "
			<< "and owner)." << std::endl;
		return nullptr;
	}
	std::shared_ptr<person> ret(CreatePerson(
				parentMap.get(), row-1, 2*(colm-1)+(row-1)%2, spec, owner ));
	ret->ChangeName(name);
	return ret;
}

std::vector<std::shared_ptr<person>> game::ParseGenericUnits(
		std::shared_ptr<map> parentMap, 
		const std::shared_ptr<unitType> genericType, 
		std::vector<std::string> unitDesc){
	std::vector<std::shared_ptr<person>> units;
	std::vector<std::string> details;
	for(auto& ud : unitDesc){
		boost::split(details, ud, boost::is_any_of(","));
		int row = std::stoi(details[0]);
		int colm = std::stoi(details[1]);
		int owner = std::stoi(details[2]);
		units.push_back(CreatePerson(parentMap.get(), row-1, 2*(colm-1)+(row-1)%2,
					genericType, owner));
	}
	return units;
}

std::shared_ptr<person> game::CreatePerson(map* theMap, const int row,
		const int colm,
		const std::shared_ptr<unitType> spec, const char faction){
	if(unitTypes.size() < 1){
		std::cerr << "Error: attempted to create a person but no unit types "
			<< "have been loaded." << std::endl;
		return nullptr;
	}
	std::shared_ptr<person> newPerson(std::make_shared<person>(Window()->Renderer(),
				spec, faction));
	people.push_back(newPerson);
	theMap->AddRoamer(newPerson, row, colm);
	return newPerson;
}

std::shared_ptr<colony> game::CreateColony(std::shared_ptr<map> parentMap,
		const int row, const int colm, const std::string& name, const int faction){
	if(row < 0 || colm < 0 || static_cast<unsigned int>(row) > parentMap->NumberOfRows() 
			|| static_cast<unsigned int>(colm) > parentMap->NumberOfColumns()){
		std::cerr << "Error: attempted to create a colony out of bounds.";
		return nullptr;
	}
	std::shared_ptr<colony> newColony(std::make_shared<colony>(Window()->Renderer(),
				parentMap->SurroundingTerrain(row, colm), faction));
	newColony->SetBuildingTypes(buildingTypes);
	newColony->ChangeName(name);
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
	EndTurn();
	std::cout << "A new turn dawns..." << std::endl;
	StartTurn();
}

void game::StartTurn(){
	for(auto& m : maps) m->StartTurn();
	for(unsigned int i = 0; i < colonies.size(); ++i) colonies[i]->ProcessTurn();
	for(auto& u : people) u->ProcessTurn(); // does this actually do anything?
	ClearDeadPeople(); // after everyone else has cleared their pointers
}

void game::EndTurn(){
	for(auto& m : maps) m->EndTurn();
}

void game::ClearDeadPeople(){
	people.erase(
			std::remove_if(people.begin(), people.end(),
				[](std::shared_ptr<person>& p){return p->Dead();}),
			people.end());
}
