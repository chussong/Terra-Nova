#include "gamewindow.hpp"

GameWindow::GameWindow(const std::string& title, const int x, const int y,
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
	GFXManager::Initialize(ren);
}

/*GameWindow::GameWindow(GameWindow&& other): win(nullptr), ren(nullptr),
		background(std::move(other.background)), 
		clickables(std::move(other.clickables)), objects(std::move(other.objects)),
		buttons(std::move(other.buttons)) {
	std::swap(win, other.win);
	std::swap(ren, other.ren);
}*/


GameWindow::~GameWindow(){
	SDL_Cleanup(ren, win);
}

void GameWindow::AddEndTurnButton(std::unique_ptr<Button> newButton){
	endTurnButton = std::move(newButton);
	endTurnButton->SetVisible(false);
}

void GameWindow::Clean(){
	objects.erase(std::remove_if(objects.begin(), objects.end(),
				[](const GFXObject* e) { return !e; }),
			objects.end());
	clickables.erase(std::remove_if(clickables.begin(), clickables.end(),
				[](const GFXObject* e) { return !e; }),
			clickables.end());
}

void GameWindow::Render(){
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

bool GameWindow::InitSDL(){
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
	Sprite::defaultFont = TTF_OpenFont(DEFAULT_FONT, DEFAULT_FONT_SIZE);
	return true;
}

void GameWindow::QuitSDL(){
	TTF_CloseFont(Sprite::defaultFont);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

/*void GameWindow::AddObject(std::string filename, const int x, const int y){
	objects.push_back(std::make_shared<GFXObject>(ren, filename, x, y));
}*/

SDL_Renderer* GameWindow::Renderer() const{
	return ren;
}

GFXObject* GameWindow::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access UIElement " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return objects[num];
}

GFXObject* GameWindow::ClickedObject(const int x, const int y){
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

// this iterates through stuff backwards so that the most recent thing is on top
GFXObject* GameWindow::SelectedObject(const int x, const int y){
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

std::array<int, 4> GameWindow::Layout() const{
	std::array<int, 4> layout;
	SDL_GetWindowPosition(win, &layout[0], &layout[1]);
	SDL_GetWindowSize(win, &layout[2], &layout[3]);
	return layout;
}

bool GameWindow::Ready() const{
	if(win == nullptr || ren == nullptr) return false;
	return true;
}

void GameWindow::ClearSelected(){
	if(!selected) return;
	selected->Deselect();
	RemoveInfoPanel();
	RemoveOrderPanel();
}

void GameWindow::ResetBackground(std::unique_ptr<UIElement> newThing){
	background.clear();
	background.push_back(std::move(newThing));
}

void GameWindow::AddToBackground(std::unique_ptr<UIElement> newThing){
	background.push_back(std::move(newThing));
}

void GameWindow::ResetObjects(){
	weakObjects.clear();
	objects.clear();
	weakClickables.clear();
	clickables.clear();
	UI.clear();
	topLevelUI.clear();
}

void GameWindow::AddTopLevelUI(std::shared_ptr<UIAggregate> newThing){
	topLevelUI.push_back(newThing);
}

void GameWindow::AddUI(std::unique_ptr<GFXObject> newThing){
	UI.push_back(std::move(newThing));
}

void GameWindow::AddClickable(std::shared_ptr<GFXObject> newThing){
	weakClickables.push_back(newThing);
}

void GameWindow::AddObject(std::shared_ptr<GFXObject> newThing){
	weakObjects.push_back(newThing);
}

signal_t GameWindow::HandleKeyPress(SDL_Keycode key, std::shared_ptr<Map> theMap){
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
				MoveUpLeft(dynamic_cast<Unit*>(selected), theMap);
			break;
		case SDLK_e:
			if(selected && selected->IsUnit() && theMap)
				MoveUpRight(dynamic_cast<Unit*>(selected), theMap);
			break;
		case SDLK_a:
			if(selected && selected->IsUnit() && theMap)
				MoveLeft(dynamic_cast<Unit*>(selected), theMap);
			break;
		case SDLK_d:
			if(selected && selected->IsUnit() && theMap)
				MoveRight(dynamic_cast<Unit*>(selected), theMap);
			break;
		case SDLK_z:
			if(selected && selected->IsUnit() && theMap)
				MoveDownLeft(dynamic_cast<Unit*>(selected), theMap);
			break;
		case SDLK_x:
			if(selected && selected->IsUnit() && theMap)
				MoveDownRight(dynamic_cast<Unit*>(selected), theMap);
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

void GameWindow::StartTurn(){
	if(selected) UpdateInfoPanel(selected);
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
}

void GameWindow::EndTurn(){
	clickables.clear();
	objects.clear();
	if(dynamic_cast<Unit*>(selected) && dynamic_cast<Unit*>(selected)->Dead()) 
		ClearSelected();
}

signal_t GameWindow::ColonyScreen(std::shared_ptr<Colony> col){
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
						GFXObject* newSelected = SelectedObject(e.button.x, e.button.y);
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
						GFXObject* obj =
							ClickedObject(e.button.x, e.button.y);
						/*if(obj && selected->IsUnit() && obj->IsTile()){
							col->AssignWorker(
									dynamic_cast<Unit*>(selected),
									dynamic_cast<Tile*>(obj));
						}*/
						if(obj && selected->IsBuildingPrototype() 
								&& obj->IsTile()){
							col->EnqueueBuilding(
									dynamic_cast<BuildingPrototype*>(selected)->Type(),
									dynamic_cast<Tile*>(obj));
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

void GameWindow::MakeColonyScreen(const Colony* col) {
	if(!col){
		std::cerr << "Error: attempted to draw a nonexistent Colony."
			<< std::endl;
		return;
	}
	std::unique_ptr<UIElement> ColonyBackground = 
		std::make_unique<UIElement>(ren, COLONY_BACKGROUND, 0, 0);

	ResetBackground(std::move(ColonyBackground));
	ResetObjects();
	DrawTiles(col);
	DrawResources(col);
	//DrawColonists(col);
	DrawColonyMisc(col);
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
}

void GameWindow::DrawTiles(const Colony* col){
	for(unsigned int i = 0; i < col->TerrainRows(); ++i){
		for(unsigned int j = 0; j < col->RowWidth(i); ++j){
			col->Terrain(i,j)->MoveTo(ColonyTileX(col,i,j), ColonyTileY(col,i));
			if(col->Terrain(i,j)->HasColony()){
				AddClickable(col->Terrain(i,j));
			} else {
				AddObject(col->Terrain(i,j));
			}
			for(auto k = 0u; k < col->Terrain(i,j)->NumberOfOccupants(); ++k){
				AddClickable(col->Terrain(i,j)->SharedOccupant(k));
			}
			/*std::cout << "Tile " << i << ", a " << terrain[i]->TileType() << ", "
				<< "moved to (" << terrain[i]->X() << "," << terrain[i]->Y() << ")."
				<< std::endl;*/
		}
	}
}

int GameWindow::ColonyTileX(const Colony* col, const unsigned int row, 
		const unsigned int colm){
	int ret = 0;
	ret -= (col->TerrainRows()-1)/2 * TILE_WIDTH;
	ret += std::abs((int)row - ((int)col->TerrainRows()-1)/2) * TILE_WIDTH/2;
	ret += colm * TILE_WIDTH;
	return ret;
}

int GameWindow::ColonyTileY(const Colony* col, const unsigned int row){
	int ret = 0;
	ret -= (static_cast<int>(col->TerrainRows()) - 1)/2 * TILE_HEIGHT;
	ret += row * TILE_HEIGHT;
	return ret;
}

void GameWindow::DrawResources(const Colony* col){
	for(unsigned int i = 0; i < col->NumberOfResources(); ++i){
		auto newPanel = std::make_unique<UIElement>(ren, 
					Colony::ResourceName(static_cast<resource_t>(i)) + "_panel",
					RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		//newPanel->AddText(std::to_string(col->Resource(i)),
				//RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		newPanel->AddDynamicText(col->Resource(i),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		AddUI(std::move(newPanel));
		/*resourcePanels[i] = std::make_shared<UIElement>(ren,
				ResourceName(static_cast<resource_t>(i)) + "_panel", 
				RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		resourcePanels[i]->AddText(std::to_string(resources[i]),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		AddObject(resourcePanels[i].get());*/
	}
}

/*void Colony::DrawColonists(const Colony* col){
	for(unsigned int i = 0; i < inhabitants.size(); ++i){
		AddClickable(inhabitants[i].lock());
	}
}*/

void GameWindow::DrawColonyMisc(const Colony* col){
	int gridLeftEdge = SCREEN_WIDTH - 50 - 
		BUILDING_GRID_COLUMNS*(BUILDING_WIDTH + 2*BUILDING_GRID_PADDING);
	int gridTopEdge = 350;
	auto buildingGrid = std::make_unique<UIElement>(ren, "buildingGrid", 
			gridLeftEdge, gridTopEdge);
	AddUI(std::move(buildingGrid));

	for(unsigned int i = 0; i < 9; ++i){
		if(i >= col->NumberOfBuildingTypes()) break;
		
		AddUI(std::make_unique<BuildingPrototype>(ren, "building",
					gridLeftEdge + (2*(i%3) + 1)*BUILDING_GRID_PADDING
						+ (i%3)*BUILDING_WIDTH,
					gridTopEdge + (2*(i/3) + 1)*BUILDING_GRID_PADDING
						+ (i/3)*BUILDING_HEIGHT,
					col->KnownBuildingType(i)));
	}

	/*SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;*/
	/*std::unique_ptr<UIElement> leaveColonyButton = 
		std::make_unique<UIElement>(ren, "leaveColony", SCREEN_WIDTH-200, 100);
	leaveColonyButton->EnableButton(LEAVE_COLONY);
	AddUI(std::move(leaveColonyButton));*/
	std::unique_ptr<Button> leaveColonyButton = std::make_unique<Button>(ren,
			"leavecolony", SCREEN_WIDTH-200, 100, 
			std::function<void()>(std::bind(&GameWindow::LeaveColony, this)));
	AddUI(std::move(leaveColonyButton));

	/*std::unique_ptr<UIElement> endTurnButton = 
		std::make_unique<UIElement>(ren, "endturn", SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
	AddUI(std::move(endTurnButton));*/
}

signal_t GameWindow::MapScreen(std::shared_ptr<Map> theMap, int centerRow,
		int centerColm){
	// we should be able to handle NEXT_TURN without resetting the screen
	// entirely. Put the loop in a separate function.
	MapScreenCenteredOn(theMap, centerRow, centerColm);

	SDL_Event e;
	bool quit = false;
	while(!quit){
		if(selected && selected->IsUnit() && dynamic_cast<Unit*>(selected)->Dead()){
			ClearSelected();
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
										dynamic_cast<Unit*>(selected)->OrderPatrol();
										break;
									case ORDER_HARVEST:
										dynamic_cast<Unit*>(selected)->OrderHarvest();
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
						GFXObject* newSelected = SelectedObject(e.button.x, e.button.y);
						if(!newSelected && selected){
							selected->Deselect();
							RemoveInfoPanel();
							RemoveOrderPanel();
						} else if(selected != newSelected) {
							if(selected) selected->Deselect();
							newSelected->Select();
							if(newSelected->IsUnit()){
								if(selected && selected->IsUnit()){
									SwapInfoPanel(newSelected);
									SwapOrderPanel(newSelected);
								} else {
									MakeInfoPanel(newSelected);
									MakeOrderPanel(newSelected);
								}
							}
						}
						selected = newSelected;
					}
					if(selected && e.button.button == SDL_BUTTON_RIGHT){
						GFXObject* obj =
							ClickedObject(e.button.x, e.button.y);
						Tile* clickedTile = dynamic_cast<Tile*>(obj);
						if(selected->IsUnit() && clickedTile){
							/*std::cout << "This character will now attempt to "
								"construct a Path to that Tile." << std::endl;*/
							Unit* selectedUnit = dynamic_cast<Unit*>(selected);
							selectedUnit->OrderMove(theMap->PathTo(
									selectedUnit->Row(), selectedUnit->Colm(),
									clickedTile->Row(), clickedTile->Colm(),
									selectedUnit->MoveCosts()));
						}
						Unit* clickedUnit = dynamic_cast<Unit*>(obj);
						if(clickedUnit && selected->IsUnit()){
							Unit* selectedUnit = dynamic_cast<Unit*>(selected);
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

void GameWindow::MapScreenCenteredOn(std::shared_ptr<Map> theMap, const int centerRow,
		const int centerColm){
	if(!theMap){
		std::cerr << "Error: attempted to draw from a non-existent Map."
			<< std::endl;
		return;
	}
	ResetObjects();
	AddMapTiles(theMap, centerRow, centerColm);

	/*std::unique_ptr<UIElement> endTurnButton = std::make_unique<UIElement>(ren,
			"endturn", SCREEN_WIDTH-200, 200);
	endTurnButton->EnableButton(END_TURN);
	AddUI(std::move(endTurnButton));*/

	if(!endTurnButton){
		std::cerr << "Error: told to make a Map screen but the end turn button "
			<< "was a null pointer." << std::endl;
	} else {
		endTurnButton->MoveTo(SCREEN_WIDTH-200, 200);
		endTurnButton->SetVisible(true);
	}
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
}

void GameWindow::AddMapTiles(std::shared_ptr<Map> theMap, const int centerRow, 
		const int centerColm){
	/*std::cout << "Constructing a Map centered on [" << centerRow << "," 
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
				/*std::cout << "The Tile at [" << i << "," << j << "] should "
					<< "appear on the screen at (" << theMap->Terrain(i,j)->X()
					<< "," << theMap->Terrain(i,j)->Y() << ")." << std::endl;*/
			}
			if(theMap->Terrain(i,j)->HasColony()){
				AddClickable(theMap->Terrain(i,j));
			} else {
				AddObject(theMap->Terrain(i,j));
			}
			for(auto k = 0u; k < theMap->Terrain(i,j)->NumberOfOccupants(); ++k){
				AddClickable(theMap->Terrain(i,j)->SharedOccupant(k));
			}
		}
	}
}

void GameWindow::MakeInfoPanel(const GFXObject* source){
	if(!(source && source->IsUnit())) return;
	AddTopLevelUI(std::make_shared<UnitInfoPanel>(ren, 
				dynamic_cast<const Unit*>(source)));
}

void GameWindow::SwapInfoPanel(const GFXObject* source){
	if(!(source && source->IsUnit())) return;
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])->Update(
					dynamic_cast<const Unit*>(source));
		}
	}
}

void GameWindow::UpdateInfoPanel(const GFXObject* source){
	if(!(source && source->IsUnit())) return;
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])->UpdateHealth(
					dynamic_cast<const Unit*>(source));
		}
	}
}

void GameWindow::RemoveInfoPanel(){
	for(unsigned int i = 0; i < topLevelUI.size(); ++i){
		if(std::dynamic_pointer_cast<UnitInfoPanel>(topLevelUI[i])){
			topLevelUI.erase(topLevelUI.begin() + i);
		}
	}
}

void GameWindow::MakeOrderPanel(GFXObject* source){
	if(!(source && source->IsUnit())) return;
	AddUI(std::make_unique<UnitOrderPanel>(ren, 
				dynamic_cast<Unit*>(source)));
}

void GameWindow::SwapOrderPanel(GFXObject* source){
	if(!(source && source->IsUnit())) return;
	for(unsigned int i = 0; i < UI.size(); ++i){
		if(dynamic_cast<UnitOrderPanel*>(UI[i].get())){
			dynamic_cast<UnitOrderPanel*>(UI[i].get())->Update(
					dynamic_cast<Unit*>(source));
		}
	}
}

void GameWindow::RemoveOrderPanel(){
	for(unsigned int i = 0; i < UI.size(); ++i){
		if(dynamic_cast<UnitOrderPanel*>(UI[i].get())){
			UI.erase(UI.begin() + i);
		}
	}
}

void GameWindow::MoveUpLeft(Unit* mover, std::shared_ptr<Map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()-1, mover->Colm()-1);
	UpdateInfoPanel(mover);
}

void GameWindow::MoveUpRight(Unit* mover, std::shared_ptr<Map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()-1, mover->Colm()+1);
	UpdateInfoPanel(mover);
}

void GameWindow::MoveLeft(Unit* mover, std::shared_ptr<Map> theMap){
	theMap->MoveUnitTo(mover, mover->Row(), mover->Colm()-2);
	UpdateInfoPanel(mover);
}

void GameWindow::MoveRight(Unit* mover, std::shared_ptr<Map> theMap){
	theMap->MoveUnitTo(mover, mover->Row(), mover->Colm()+2);
	UpdateInfoPanel(mover);
}

void GameWindow::MoveDownLeft(Unit* mover, std::shared_ptr<Map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()+1, mover->Colm()-1);
	UpdateInfoPanel(mover);
}

void GameWindow::MoveDownRight(Unit* mover, std::shared_ptr<Map> theMap){
	theMap->MoveUnitTo(mover, mover->Row()+1, mover->Colm()+1);
	UpdateInfoPanel(mover);
}
