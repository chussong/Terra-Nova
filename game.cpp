#include "game.hpp"

std::vector<BuildingType*> Faction::defaultBuildingTypes;

Game::Game(): Game(nullptr){
}

Game::Game(GameScreen* screen): screen(screen) {
	if(!screen) {
		std::cerr << "Asked to make a Game with no GameScreen to populate."
			<< std::endl;
		throw std::runtime_error("Game constructor");
		//screen = std::make_shared<GameScreen>("Terra Nova", 100, 100, 1024, 768);
	}
	try{
		ReadAttackTypes();
		ReadUnitTypes();
		ReadBuildingTypes();
		ReadTileTypes();
		ReadMap("1");
	}
	catch(const readError& e){
		// instead of complaining, should generate the def files from hard-coded versions
		std::cerr << e.what() << std::endl;
		throw;
	}
	screen->AddEndTurnButton(EndTurnButton(screen->Renderer()));

	std::vector<Event> events = Event::ReadEventDirectory("1");
	for(auto& event : events) {
		if(event.HasStartTrigger()) mapStartEvents.push_back(std::move(event));
		if(event.HasLocationTrigger()) locationEvents.push_back(std::move(event));
	}

	// placeholder
	aiPlayers.emplace_back(*maps[0], 2);
	for(auto& u : units) aiPlayers[0].AddUnit(u);
}

void Game::Begin(){
	for(auto& event : mapStartEvents) ExecuteEvent(event);
	StartTurn();
	screen->MapScreen(maps[0].get(), 4, 8);
}

bool Game::Tick(){
	return true;
}

std::vector<std::string> Game::LoadDefFile(const std::string& name){
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
			std::cerr << "Error getting resource Path: " << SDL_GetError()
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

void Game::ReadAttackTypes(){
	std::vector<std::string> attackDefs = LoadDefFile("weapons.txt");
	if(attackDefs.size() == 0){
		std::cerr << "This is bad but I don't have the exceptions working "
			<< "yet." << std::endl;
		return;
		//throw readError();
	}
	
	attackTypes.clear();
	std::shared_ptr<AttackType> newAttack;
	std::vector<std::string> entries;
	
	for(auto& attack : attackDefs){
		boost::split(entries, attack, boost::is_any_of("|"));
		/*std::cout << entries[0] << " | " << boost::trim_copy(entries[0]) << std::endl;
		std::cout << entries[1] << " | " << std::stof(entries[1]) << std::endl;
		std::cout << entries[2] << " | " << std::stoi(entries[2]) << std::endl;
		std::cout << entries[3] << " | " << std::stoi(entries[3]) << std::endl;*/
		try{
			newAttack = std::make_shared<AttackType>(boost::trim_copy(entries[0]),
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

void Game::ReadUnitTypes(){
	std::vector<std::string> unitDefs = LoadDefFile("unit_types.txt");
	if(unitDefs.size() == 0){
		std::cerr << "This is bad but I don't have the exceptions working yet."
			<< std::endl;
		return;
		//throw readError();
	}
	
	unitTypes.clear();
	std::shared_ptr<UnitType> newSpec;
	std::vector<std::string> entries;

	std::vector<std::shared_ptr<AttackType>> atks;
	std::shared_ptr<AttackType> atk;
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
			newSpec = std::make_shared<UnitType>(boost::trim_copy(entries[0]),
					std::stoi(entries[1]), std::stoi(entries[2]), atks,
					std::stoi(entries[4]));
		}
		catch(const std::exception &e){
			std::cout << "A Unit type definition file was detected, but its "
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

// read exported file listing Building types; this is a placeholder
void Game::ReadBuildingTypes(){
	int idsUsed = 0;
	buildingTypes.clear();
	std::array<int, 4> costs;

	costs = {{0,60,40,40}};
	BuildingType factoryFarm(idsUsed++, "Factory Farm", costs, 3);
	factoryFarm.SetBonusResources({{4,0,0,0}});
	buildingTypes.push_back(std::move(factoryFarm));

	costs = {{0,20,60,80}};
	BuildingType autoMine(idsUsed++, "Automated Mine", costs, 4);
	autoMine.SetMaxOccupants(0);
	autoMine.SetAutomatic(true);
	buildingTypes.push_back(std::move(autoMine));

	costs = {{0,20,20,40}};
	BuildingType academy(idsUsed++, "Academy", costs, 4);
	academy.SetCanHarvest(false);
	academy.SetCanTrain(unitTypes[1]);
	buildingTypes.push_back(std::move(academy));

	std::vector<BuildingType*> factionBuildingTypes;
	for(auto& bt : buildingTypes) factionBuildingTypes.push_back(&bt);
	Faction::SetDefaultBuildingTypes(std::move(factionBuildingTypes));
}

const std::vector<BuildingType>& Game::BuildingTypes() const{
	return buildingTypes;
}

void Game::ReadTileTypes(){
	std::vector<std::string> terrainDefs = LoadDefFile("terrain.txt");
	std::string name;
	std::array<int, LAST_RESOURCE> yield;
	std::shared_ptr<TileType> newType;
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

		newType = std::make_shared<TileType>(name, yield);

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

void Game::ReadMap(const std::string& MapName){
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
			std::cerr << "Error getting chapter Path: " << SDL_GetError()
				<< std::endl;
			return;
		}
		size_t pos = baseChapterDir.rfind("bin");
		baseChapterDir = baseChapterDir.substr(0, pos);
	}

	std::ifstream in(baseChapterDir + MapName + PATH_SEP + "map.txt");
	if((in.rdstate() & std::ifstream::failbit) != 0){
		std::cerr << "Error: unable to open Map file at "
			<< baseChapterDir + MapName + PATH_SEP + "map.txt"
			<< ". Does it exist?" << std::endl;
		return;
	}

	std::string line;
	do{
		std::getline(in, line);
		if(line.length() > 0 && line[0] != '%') lines.push_back(line);
	}while((in.rdstate() & std::ifstream::eofbit) == 0);

	std::map<char, std::shared_ptr<TileType>> TileDict;
	std::shared_ptr<Map> newMap;
	bool runAgain = false;
	int runCount = 0;
	do{
		for(unsigned int i = 0; i < lines.size(); ++i){
			if(lines[i].compare(0, 14, "Terrain types:") == 0){
				TileDict = ParseTerrainDictionary(ExtractMapSection(lines, i));
			}
			if(lines[i].compare(0, 4, "Map:") == 0){
				if(TileDict.size() == 0){
					std::cout << "Map found but terrain types have not yet been "
						<< "defined. Will read to end of file and start again."
						<< std::endl;
					runAgain = true;
					continue;
				}
				newMap = std::make_shared<Map>(Screen()->Renderer(), 
						ParseMap(TileDict, ExtractMapSection(lines, i)) );
				maps.push_back(newMap);
			}
			if(lines[i].compare(0, 9, "Features:") == 0){
				if(!newMap){
					std::cout << "Features enumeration found but Map has not yet been "
						<< "created. Will read to end of file and start again." 
						<< std::endl;
					runAgain = true;
					continue;
				}
				ParseFeatures(newMap, ExtractMapSection(lines, i));
			}
			if(lines[i].compare(0, 6, "Units:") == 0){
				if(!newMap){
					std::cout << "Units enumeration found but Map has not yet been "
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

std::vector<std::string> Game::ExtractMapSection(
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

std::map<char, std::shared_ptr<TileType>> Game::ParseTerrainDictionary(
		const std::vector<std::string> desc){
	std::map<char, std::shared_ptr<TileType>> tileDic;
	for(auto& line : desc){
		tileDic.emplace(line[0], FindByName(tileTypes,
					boost::trim_copy(line.substr(line.find('|')+1)) ));
	}
	return tileDic;
}

std::string Game::ParseDescLine(const std::string& line, const std::string& key){
	if(line.find(key) == std::string::npos) return "";
	return boost::trim_copy(line.substr(line.find('=') + 1));
}

std::vector<std::vector<std::shared_ptr<Tile>>> Game::ParseMap(
		const std::map<char, std::shared_ptr<TileType>> TileDict, 
		const std::vector<std::string> desc){
	std::vector<std::string> mapAsStrings;
	std::vector<std::vector<std::shared_ptr<Tile>>> newTiles;
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
				= std::make_shared<Tile>(
						TileDict.find(mapAsStrings[i][0])->second.get(), Screen()->Renderer(),
						newTiles.size()-1, 2*i + (newTiles.size()-1)%2);
			//std::cout << " survived." << std::endl;
		}
	}
	return newTiles;
}

void Game::ParseFeatures(std::shared_ptr<Map> parentMap, 
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
			ParseColony(parentMap, colonyDesc);
			i = j+1;
			continue;
		}
		++i;
	}
}

void Game::ParseColony(std::shared_ptr<Map> parentMap,
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
		std::cerr << "Error: identified a Colony feature in map.txt which does "
			<< "not appear to have all of the information needed for "
			<< "instantiation." << std::endl;
		return;
	}
	CreateColony(parentMap, row-1, 2*(colm-1) + (row-1)%2, name, owner);
}

void Game::ParseUnits(std::shared_ptr<Map> parentMap, 
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
			units.push_back(ParseUniqueUnit(parentMap, unitDesc));
			continue;
		}
		if(desc[i].find('{') < std::string::npos){
			std::shared_ptr<UnitType> genericType;
			genericType = FindByName(unitTypes, desc[i].substr(0, desc[i].find('{')));
			if(!genericType){
				std::cerr << "Error: a Unit entry appears to have been found "
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
			std::vector<std::shared_ptr<Unit>> genericUnits =
				ParseGenericUnits(parentMap, genericType, unitDesc);
			for(auto& gu : genericUnits) units.push_back(gu);
		}
	}
}

std::shared_ptr<Unit> Game::ParseUniqueUnit(std::shared_ptr<Map> parentMap,
		const std::vector<std::string>& unitDesc){
	std::string name = "";
	std::shared_ptr<UnitType> spec;
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
		std::cerr << "Error: found an entry for a unique Unit but it did not "
			<< "have all of the required information (name, spec, position, "
			<< "and owner)." << std::endl;
		return nullptr;
	}
	std::shared_ptr<Unit> ret(CreatePerson(
				parentMap.get(), row-1, 2*(colm-1)+(row-1)%2, spec, owner ));
	ret->ChangeName(name);
	ret->MarkUnique();
	return ret;
}

std::vector<std::shared_ptr<Unit>> Game::ParseGenericUnits(
		std::shared_ptr<Map> parentMap, 
		const std::shared_ptr<UnitType> genericType, 
		std::vector<std::string> unitDesc){
	std::vector<std::shared_ptr<Unit>> parsedUnits;
	std::vector<std::string> details;
	for(auto& ud : unitDesc){
		boost::split(details, ud, boost::is_any_of(","));
		int row = std::stoi(details[0]);
		int colm = std::stoi(details[1]);
		int owner = std::stoi(details[2]);
		parsedUnits.push_back(CreatePerson(parentMap.get(), row-1, 2*(colm-1)+(row-1)%2,
					genericType, owner));
	}
	return parsedUnits;
}

std::shared_ptr<Unit> Game::CreatePerson(Map* theMap, const int row,
		const int colm,
		const std::shared_ptr<UnitType> spec, const char faction){
	if(unitTypes.size() < 1){
		std::cerr << "Error: attempted to create a Unit but no Unit types "
			<< "have been loaded." << std::endl;
		return nullptr;
	}
	std::shared_ptr<Unit> newPerson(std::make_shared<Unit>(Screen()->Renderer(),
				spec, faction));
	//units.push_back(newPerson);
	theMap->AddRoamer(newPerson, row, colm);
	return newPerson;
}

void Game::CreateColony(std::shared_ptr<Map> parentMap,
		const int row, const int colm, const std::string& name, const int faction){
	if(row < 0 || colm < 0 || static_cast<unsigned int>(row) > parentMap->NumberOfRows() 
			|| static_cast<unsigned int>(colm) > parentMap->NumberOfColumns()){
		std::cerr << "Error: attempted to create a Colony out of bounds.";
		return;
	}
	//std::shared_ptr<Colony> newColony(std::make_shared<Colony>(Screen()->Renderer(),
				//parentMap->SurroundingTerrain(row, colm), faction));
	//newColony->SetBuildingTypes(buildingTypes);
	//newColony->ChangeName(name);
	//parentMap->Terrain(row, colm)->SetHasColony(newColony->Faction());
	Colony* newCol = parentMap->AddColony(row, colm, faction);
	if(newCol && name != "") newCol->ChangeName(name);
	//colonies.push_back(newColony);
}

std::shared_ptr<Map> Game::CreateMap(){
	ReadTileTypes();
	std::shared_ptr<Map> newMap(std::make_shared<Map>(Screen()->Renderer(),
			tileTypes));
	maps.push_back(newMap);
	return newMap;
}

std::unique_ptr<Button> Game::EndTurnButton(SDL_Renderer* ren) {
	/*auto endTurnFunc = std::bind(&Game::NextTurn, this);
	endTurnFunc();
	return std::make_unique<Button>(ren, "end_turn", 0, 0, endTurnFunc);*/
	return std::make_unique<Button>(ren, "end_turn", 0, 0,
			std::function<void()>(std::bind(&Game::NextTurn, this)));
}

void Game::NextTurn(){
	EndTurn();
	std::cout << "A new turn dawns..." << std::endl;
	StartTurn();
}

void Game::StartTurn(){
	ClearDeadUnits(); // before everyone else locks their pointers
	ClearFinishedEvents();
	for(auto& m : maps) m->StartTurn();
	for(auto& event : locationEvents) ExecuteEventIfTriggered(event);
	//locationEvents.erase(
			//std::remove_if(locationEvents.begin(), locationEvents.end(),
				//std::bind(&Game::ExecuteEventIfTriggered, this, 
							//std::placeholders::_1)) );
	//std::cout << "Processing turns in " << colonies.size() << " colonies." << std::endl;
	//for(unsigned int i = 0; i < colonies.size(); ++i) colonies[i]->ProcessTurn();
	for(auto& u : units) u->ProcessTurn(); // resets move allowance
	for(auto& aip : aiPlayers) aip.StartTurn();

	// I don't think we should actually be doing this here
	for(auto& aip : aiPlayers) aip.GiveOrders();

	screen->StartTurn();
}

void Game::EndTurn(){
	for(auto& m : maps) m->EndTurn();
	for(auto& aip : aiPlayers) aip.EndTurn();
	screen->EndTurn();
}

void Game::ClearDeadUnits(){
	units.erase(
			std::remove_if(units.begin(), units.end(),
				[](std::shared_ptr<Unit>& unit){return unit->Dead();}),
			units.end());
}

// warning: since each event owns its associated dialogue, this will cause a 
// segfault if the event is cleared before the player closes the dialogue. As of
// the writing of this comment, such a situation is impossible because you can't
// start a new turn with a dialogue open.
void Game::ClearFinishedEvents(){
	locationEvents.erase(
			std::remove_if(locationEvents.begin(), locationEvents.end(),
				[](const Event& event){return event.Finished();}),
			locationEvents.end() );
}

// Return true if event was triggered and is not repeatable; false otherwise.
//
// this is definitely still pretty hackish. Need to specify map (or have event
// know which map it's on) and a player (or have event know player number).
bool Game::ExecuteEventIfTriggered(Event& event) {
	if(event.HasLocationTrigger()) {
		for(auto& loc : *event.TriggerLocations()) {
			if(maps[0]->Terrain(loc)->Faction() == 1) {
				//std::cout << "Firing event, will erase shortly." << std::endl;
				return ExecuteEvent(event);
			}
		}
	}
	return false;
}

// Return false if event is repeatable and should not be cleared.
bool Game::ExecuteEvent(Event& event) {
	if(event.HasLinkedDialogue()) screen->PlayDialogue(event.LinkedDialogue());
	if(!event.Repeatable()) event.SetFinished();
	return true;
}
