#include "gamewindow.hpp"

gameWindow::gameWindow(const std::string& title, const int x, const int y,
		const int w, const int h): selected(nullptr) {
	if((SDL_WasInit(SDL_INIT_EVERYTHING) & SDL_INIT_EVERYTHING) == 0){
		if(!InitSDL()) return;
	}
	win = SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_SHOWN);
	if(win == nullptr){
		LogSDLError(std::cout, "CreateWindow");
	} else {
		ren = SDL_CreateRenderer(win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(ren == nullptr){
			LogSDLError(std::cout, "CreateRenderer");
		}
	}
	gfxManager::Initialize(ren);
}

/*gameWindow::gameWindow(gameWindow&& other): win(nullptr), ren(nullptr),
		background(std::move(other.background)), 
		clickables(std::move(other.clickables)), objects(std::move(other.objects)),
		buttons(std::move(other.buttons)) {
	std::swap(win, other.win);
	std::swap(ren, other.ren);
}*/


gameWindow::~gameWindow(){
	SDL_Cleanup(ren, win);
}

void gameWindow::AddEndTurnButton(std::unique_ptr<Button> newButton){
	endTurnButton = std::move(newButton);
	endTurnButton->SetVisible(false);
}

void gameWindow::Clean(){
	objects.erase(std::remove_if(objects.begin(), objects.end(),
				[](const entity* e) { return !e; }),
			objects.end());
	clickables.erase(std::remove_if(clickables.begin(), clickables.end(),
				[](const entity* e) { return !e; }),
			clickables.end());
}

void gameWindow::Render(){
	SDL_RenderClear(ren);
	Clean();
	for(auto& thing : background) thing->Render();
	for(auto& thing : objects	) thing->Render();
	for(auto& thing : clickables) thing->Render();
	for(auto& thing : UI        ) thing->Render();
	for(auto& thing : topLevelUI) thing->Render();
	if(endTurnButton) endTurnButton->Render();
	SDL_RenderPresent(ren);
}

bool gameWindow::InitSDL(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		LogSDLError(std::cout, "SDL_Init");
		return false;
	}
	if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		LogSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return false;
	}
	if(TTF_Init() != 0){
		LogSDLError(std::cout, "TTF_Init");
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	gfxObject::defaultFont = TTF_OpenFont(DEFAULT_FONT, DEFAULT_FONT_SIZE);
	return true;
}

void gameWindow::QuitSDL(){
	TTF_CloseFont(gfxObject::defaultFont);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

/*void gameWindow::AddObject(std::string filename, const int x, const int y){
	objects.push_back(std::make_shared<entity>(ren, filename, x, y));
}*/

SDL_Renderer* gameWindow::Renderer() const{
	return ren;
}

entity* gameWindow::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access uiElement " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return objects[num];
}

entity* gameWindow::ClickedObject(const int x, const int y){
	for(unsigned int i = UI.size()-1; i < UI.size(); --i){
		if(UI[i]->InsideQ(x, y)) return UI[i].get();
	}
	for(unsigned int i = clickables.size()-1; i < clickables.size(); --i){
		if(clickables[i]->InsideQ(x, y)) return clickables[i];
	}
	for(unsigned int i = objects.size()-1; i < objects.size(); --i){
		if(objects[i]->InsideQ(x, y)) return objects[i];
	}
	/*std::cout << "No clickable object detected at clicked location." << std::endl;
	std::cout << "I am aware of " << clickables.size() << " selectable objects "
		<< "and " << objects.size() << " non-selectable objects." << std::endl;*/
	return nullptr;
}

entity* gameWindow::SelectedObject(const int x, const int y){
	// iterate through these backwards so that the most recent thing is on top
	if(endTurnButton->InsideQ(x, y)){
		if(endTurnButton->Click()) return selected;
		return endTurnButton.get();
	}
	for(unsigned int i = UI.size()-1; i < UI.size(); --i){
		if(UI[i]->InsideQ(x, y)){
			if(UI[i]->Click()) return selected;
			return UI[i].get();
		}
	}
	for(unsigned int i = clickables.size()-1; i < clickables.size(); --i){
		if(clickables[i]->InsideQ(x, y)){
			if(clickables[i]->Click()) return selected;
			return clickables[i];
		}
	}
	return nullptr;
}

std::array<int, 4> gameWindow::Layout() const{
	std::array<int, 4> layout;
	SDL_GetWindowPosition(win, &layout[0], &layout[1]);
	SDL_GetWindowSize(win, &layout[2], &layout[3]);
	return layout;
}

bool gameWindow::Ready() const{
	if(win == nullptr || ren == nullptr) return false;
	return true;
}

void gameWindow::ResetBackground(std::unique_ptr<uiElement> newThing){
	background.clear();
	background.push_back(std::move(newThing));
}

void gameWindow::AddToBackground(std::unique_ptr<uiElement> newThing){
	background.push_back(std::move(newThing));
}

void gameWindow::ResetObjects(){
	objects.clear();
	clickables.clear();
	UI.clear();
	topLevelUI.clear();
}

void gameWindow::AddTopLevelUI(std::shared_ptr<UIAggregate> newThing){
	topLevelUI.push_back(newThing);
}

void gameWindow::AddUI(std::unique_ptr<entity> newThing){
	UI.push_back(std::move(newThing));
}

void gameWindow::AddClickable(entity* newThing){
	clickables.push_back(newThing);
}

void gameWindow::AddClickable(std::shared_ptr<entity> newThing){
	std::cerr << "Warning: a shared_ptr is being added to the clickable list. "
		<< "GameWindow will NOT own it, and you'll probably see a crash "
		<< "momentarily!" << std::endl;
	clickables.push_back(newThing.get());
}

void gameWindow::AddObject(entity* newThing){
	objects.push_back(newThing);
}

signal_t gameWindow::HandleKeyPress(SDL_Keycode key, std::shared_ptr<map> theMap){
	switch(key){
		case SDLK_c:
		case SDLK_s:
			return SCREEN_CHANGE;
		case SDLK_ESCAPE:
			return QUIT;
		case SDLK_RETURN:
			return NEXT_TURN;
		case SDLK_LEFT:
			if(theMap) theMap->MoveView(VIEW_LEFT);
			break;
		case SDLK_RIGHT:
			if(theMap) theMap->MoveView(VIEW_RIGHT);
			break;
		case SDLK_UP:
			if(theMap) theMap->MoveView(VIEW_UP);
			break;
		case SDLK_DOWN:
			if(theMap) theMap->MoveView(VIEW_DOWN);
			break;
		case SDLK_w:
			if(selected && selected->IsUnit() && theMap)
				MoveUpLeft(dynamic_cast<person*>(selected), theMap);
			break;
		case SDLK_e:
			if(selected && selected->IsUnit() && theMap)
				MoveUpRight(dynamic_cast<person*>(selected), theMap);
			break;
		case SDLK_a:
			if(selected && selected->IsUnit() && theMap)
				MoveLeft(dynamic_cast<person*>(selected), theMap);
			break;
		case SDLK_d:
			if(selected && selected->IsUnit() && theMap)
				MoveRight(dynamic_cast<person*>(selected), theMap);
			break;
		case SDLK_z:
			if(selected && selected->IsUnit() && theMap)
				MoveDownLeft(dynamic_cast<person*>(selected), theMap);
			break;
		case SDLK_x:
			if(selected && selected->IsUnit() && theMap)
				MoveDownRight(dynamic_cast<person*>(selected), theMap);
			break;
		case SDLK_h:
			return SIG_ORDER_HARVEST;
		case SDLK_p:
			return SIG_ORDER_PATROL;
		default:
			break;
	}
	return NOTHING;
}

signal_t gameWindow::ColonyScreen(std::shared_ptr<colony> col){
	MakeColonyScreen(col.get());

	SDL_Event e;
	bool quit = false;
	leaveColony = false;
	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:				
					quit = true;
					break;
				case SDL_KEYUP:			
					switch(HandleKeyPress(e.key.keysym.sym, nullptr)){
						case SCREEN_CHANGE:
							return SCREEN_CHANGE;
						case NEXT_TURN:
							return NEXT_TURN;
						case QUIT:
							quit = true;
							break;
						default:
							break;
					}
				case SDL_MOUSEBUTTONDOWN:	
					if(e.button.button == SDL_BUTTON_LEFT){
						entity* newSelected = SelectedObject(e.button.x, e.button.y);
						if(newSelected != selected){
							if(selected) selected->Deselect();
						}
						selected = newSelected;
						/*if(selected){
							switch(selected->Select()/100){
								case NEXT_TURN/100:		return NEXT_TURN;
								case SCREEN_CHANGE/100:	return SCREEN_CHANGE;
								default:				break;
							}
						}*/
					}
					if(selected && e.button.button == SDL_BUTTON_RIGHT){
						entity* obj =
							ClickedObject(e.button.x, e.button.y);
						if(obj && selected->IsUnit() && obj->IsTile()){
							col->AssignWorker(
									dynamic_cast<person*>(selected),
									dynamic_cast<tile*>(obj));
						}
						if(obj && selected->IsBuildingPrototype() 
								&& obj->IsTile()){
							col->EnqueueBuilding(
									dynamic_cast<buildingPrototype*>(selected)->Type(),
									dynamic_cast<tile*>(obj));
						}
					}
					break;
			}
		}
		if(leaveColony) return SCREEN_CHANGE;
		Render();
	}
	return QUIT;
}

void gameWindow::MakeColonyScreen(const colony* col) {
	if(!col){
		std::cerr << "Error: attempted to draw a nonexistent colony."
			<< std::endl;
		return;
	}
	std::unique_ptr<uiElement> colonyBackground = 
		std::make_unique<uiElement>(ren, COLONY_BACKGROUND, 0, 0);

	ResetBackground(std::move(colonyBackground));
	ResetObjects();
	DrawTiles(col);
	DrawResources(col);
	//DrawColonists(col);
	DrawColonyMisc(col);
}

void gameWindow::DrawTiles(const colony* col){
	for(unsigned int i = 0; i < col->TerrainRows(); ++i){
		for(unsigned int j = 0; j < col->RowWidth(i); ++j){
			col->Terrain(i,j)->MoveTo(ColonyTileX(col,i,j), ColonyTileY(col,i));
			if(col->Terrain(i,j)->HasColony()){
				AddClickable(col->Terrain(i,j).get());
			} else {
				AddObject(col->Terrain(i,j).get());
			}
			for(auto& occ : col->Terrain(i,j)->Occupants()) AddClickable(occ);
			/*std::cout << "Tile " << i << ", a " << terrain[i]->TileType() << ", "
				<< "moved to (" << terrain[i]->X() << "," << terrain[i]->Y() << ")."
				<< std::endl;*/
		}
	}
}

int gameWindow::ColonyTileX(const colony* col, const unsigned int row, 
		const unsigned int colm){
	int ret = 0;
	ret -= (col->TerrainRows()-1)/2 * TILE_WIDTH;
	ret += std::abs((int)row - ((int)col->TerrainRows()-1)/2) * TILE_WIDTH/2;
	ret += colm * TILE_WIDTH;
	return ret;
}

int gameWindow::ColonyTileY(const colony* col, const unsigned int row){
	int ret = 0;
	ret -= (static_cast<int>(col->TerrainRows()) - 1)/2 * TILE_HEIGHT;
	ret += row * TILE_HEIGHT;
	return ret;
}

void gameWindow::DrawResources(const colony* col){
	for(unsigned int i = 0; i < col->NumberOfResources(); ++i){
		auto newPanel = std::make_unique<uiElement>(ren, 
					colony::ResourceName(static_cast<resource_t>(i)) + "_panel",
					RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		//newPanel->AddText(std::to_string(col->Resource(i)),
				//RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		newPanel->AddDynamicText(col->Resource(i),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		AddUI(std::move(newPanel));
		/*resourcePanels[i] = std::make_shared<uiElement>(ren,
				ResourceName(static_cast<resource_t>(i)) + "_panel", 
				RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		resourcePanels[i]->AddText(std::to_string(resources[i]),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		AddObject(resourcePanels[i].get());*/
	}
}

/*void colony::DrawColonists(const colony* col){
	for(unsigned int i = 0; i < inhabitants.size(); ++i){
		AddClickable(inhabitants[i].lock());
	}
}*/

void gameWindow::DrawColonyMisc(const colony* col){
	int gridLeftEdge = SCREEN_WIDTH - 50 - 
		BUILDING_GRID_COLUMNS*(BUILDING_WIDTH + 2*BUILDING_GRID_PADDING);
	int gridTopEdge = 350;
	auto buildingGrid = std::make_unique<uiElement>(ren, "buildingGrid", 
			gridLeftEdge, gridTopEdge);
	AddUI(std::move(buildingGrid));

	for(unsigned int i = 0; i < 9; ++i){
		if(i >= col->NumberOfBuildingTypes()) break;
		
		AddUI(std::make_unique<buildingPrototype>(ren, "building",
					gridLeftEdge + (2*(i%3) + 1)*BUILDING_GRID_PADDING
						+ (i%3)*BUILDING_WIDTH,
					gridTopEdge + (2*(i/3) + 1)*BUILDING_GRID_PADDING
						+ (i/3)*BUILDING_HEIGHT,
					col->BuildingType(i)));
	}

	/*SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;*/
	/*std::unique_ptr<uiElement> leaveColonyButton = 
		std::make_unique<uiElement>(ren, "leavecolony", SCREEN_WIDTH-200, 100);
	leaveColonyButton->EnableButton(LEAVE_COLONY);
	AddUI(std::move(leaveColonyButton));*/
	std::unique_ptr<Button> leaveColonyButton = std::make_unique<Button>(ren,
			"leavecolony", SCREEN_WIDTH-200, 100, 
			std::function<void()>(std::bind(&gameWindow::LeaveColony, this)));
	AddUI(std::move(leaveColonyButton));

	/*std::unique_ptr<uiElement> endTurnButton = 
		std::make_unique<uiElement>(ren, "endturn", SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
	AddUI(std::move(endTurnButton));*/
}

signal_t gameWindow::MapScreen(std::shared_ptr<map> theMap, int centerRow,
		int centerColm){
	// we should be able to handle NEXT_TURN without resetting the screen
	// entirely. Put the loop in a separate function.
	MapScreenCenteredOn(theMap, centerRow, centerColm);

	SDL_Event e;
	bool quit = false;
	while(!quit){
		if(selected && selected->IsUnit() && dynamic_cast<person*>(selected)->Dead()){
			selected = nullptr;
		}
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:{
					quit = true;
					break;
							  }
				case SDL_KEYUP:{
					signal_t keySig = HandleKeyPress(e.key.keysym.sym, theMap);
					switch(keySig/100){
						case SCREEN_CHANGE/100:
							return SCREEN_CHANGE;
						case NEXT_TURN/100:
							endTurnButton->Click();
							break;
							//return NEXT_TURN;
						case QUIT/100:
							quit = true;
							break;
						case SIG_GIVE_ORDER/100:
							if(selected && selected->IsUnit()){
								switch(keySig%100){
									case ORDER_PATROL:
										dynamic_cast<person*>(selected)->OrderPatrol();
										break;
									case ORDER_HARVEST:
										dynamic_cast<person*>(selected)->OrderHarvest();
										break;
								}
							}
							break;
						default:
							break;
					}
					break;
							   }
				case SDL_MOUSEBUTTONDOWN:{
					if(e.button.button == SDL_BUTTON_LEFT){
						if(selected){
							selected->Deselect();
							RemoveUnitInfoPanel();
						}
						selected = SelectedObject(e.button.x, e.button.y);
						if(selected){
							switch(selected->Select()/100){
								case NEXT_TURN/100:
									selected = nullptr;
									return NEXT_TURN;
								case SCREEN_CHANGE/100:
									selected = nullptr;
									return SCREEN_CHANGE;
								default:
									break;
							}
							if(selected->IsUnit()){
								MakeUnitInfoPanel(dynamic_cast<person*>(selected));
							}
						}
					}
					if(selected && e.button.button == SDL_BUTTON_RIGHT){
						entity* obj =
							ClickedObject(e.button.x, e.button.y);
						tile* clickedTile = dynamic_cast<tile*>(obj);
						if(selected->IsUnit() && clickedTile){
							/*std::cout << "This character will now attempt to "
								"construct a path to that tile." << std::endl;*/
							person* selectedUnit = dynamic_cast<person*>(selected);
							selectedUnit->OrderMove(theMap->PathTo(
									selectedUnit->Row(), selectedUnit->Colm(),
									clickedTile->Row(), clickedTile->Colm(),
									selectedUnit->MoveCosts()));
						}
						person* clickedUnit = dynamic_cast<person*>(obj);
						if(clickedUnit && selected->IsUnit()){
							person* selectedUnit = dynamic_cast<person*>(selected);
							selectedUnit->OrderMove(theMap->PathTo(
									selectedUnit->Row(), selectedUnit->Colm(),
									clickedUnit->Row(), clickedUnit->Colm(),
									selectedUnit->MoveCosts()));
						}
					}
					break;
										 }
			}
		}
		Render();
	}
	return QUIT;
}

void gameWindow::MapScreenCenteredOn(std::shared_ptr<map> theMap, const int centerRow,
		const int centerColm){
	if(!theMap){
		std::cerr << "Error: attempted to draw from a non-existent map."
			<< std::endl;
		return;
	}
	ResetObjects();
	AddMapTiles(theMap, centerRow, centerColm);

	/*std::unique_ptr<uiElement> endTurnButton = std::make_unique<uiElement>(ren,
			"endturn", SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
	AddUI(std::move(endTurnButton));*/

	if(!endTurnButton){
		std::cerr << "Error: told to make a map screen but the end turn button "
			<< "was a null pointer." << std::endl;
	} else {
		endTurnButton->MoveTo(SCREEN_WIDTH-200, 200);
		endTurnButton->SetVisible(true);
	}
}

void gameWindow::AddMapTiles(std::shared_ptr<map> theMap, const int centerRow, 
		const int centerColm){
	/*std::cout << "Constructing a map centered on [" << centerRow << "," 
		<< centerColm << "]." << std::endl;*/
	for(unsigned int i = 0; i < theMap->NumberOfRows(); ++i){
		for(unsigned int j = i%2; j < theMap->NumberOfColumns(); j+=2){
			theMap->Terrain(i,j)->MoveTo(
					0 + (static_cast<int>(j)-centerColm)*TILE_WIDTH/2,
					0 + (static_cast<int>(i)-centerRow)*TILE_HEIGHT);
			if(theMap->Terrain(i,j)->X() > 0
					&& theMap->Terrain(i,j)->X() < SCREEN_WIDTH
					&& theMap->Terrain(i,j)->Y() > 0
					&& theMap->Terrain(i,j)->Y() < SCREEN_HEIGHT){
				/*std::cout << "The tile at [" << i << "," << j << "] should "
					<< "appear on the screen at (" << theMap->Terrain(i,j)->X()
					<< "," << theMap->Terrain(i,j)->Y() << ")." << std::endl;*/
			}
			if(theMap->Terrain(i,j)->HasColony()){
				AddClickable(theMap->Terrain(i,j).get());
			} else {
				AddObject(theMap->Terrain(i,j).get());
			}
			for(auto& occ : theMap->Terrain(i,j)->Occupants()){
				AddClickable(occ);
			}
		}
	}
}

void gameWindow::MakeUnitInfoPanel(const entity* source){
	if(!source->IsUnit()) return;
	AddTopLevelUI(std::make_shared<UnitInfoPanel>(ren, 
				dynamic_cast<const person*>(source)));
}

void gameWindow::UpdateUnitInfoPanel(const entity* source){
	if(!source->IsUnit()) return;
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])->UpdateHealth(
					dynamic_cast<const person*>(source));
		}
	}
}

void gameWindow::RemoveUnitInfoPanel(){
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			topLevelUI.erase(topLevelUI.begin() + i);
		}
	}
}

void gameWindow::MoveUpLeft(person* mover, std::shared_ptr<map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()-1, mover->Colm()-1);
	UpdateUnitInfoPanel(mover);
}

void gameWindow::MoveUpRight(person* mover, std::shared_ptr<map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()-1, mover->Colm()+1);
	UpdateUnitInfoPanel(mover);
}

void gameWindow::MoveLeft(person* mover, std::shared_ptr<map> theMap){
	theMap->MoveUnitTo(mover, mover->Row(), mover->Colm()-2);
	UpdateUnitInfoPanel(mover);
}

void gameWindow::MoveRight(person* mover, std::shared_ptr<map> theMap){
	theMap->MoveUnitTo(mover, mover->Row(), mover->Colm()+2);
	UpdateUnitInfoPanel(mover);
}

void gameWindow::MoveDownLeft(person* mover, std::shared_ptr<map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()+1, mover->Colm()-1);
	UpdateUnitInfoPanel(mover);
}

void gameWindow::MoveDownRight(person* mover, std::shared_ptr<map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()+1, mover->Colm()+1);
	UpdateUnitInfoPanel(mover);
}
