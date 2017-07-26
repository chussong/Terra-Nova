#include "gamewindow.hpp"

GameWindow::GameWindow(const std::string& title, const int x, const int y,
		const int w, const int h): playerNumber(1), selected(nullptr) {
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

GameWindow::~GameWindow(){
	SDL_Cleanup(ren, win);
}

void GameWindow::AddEndTurnButton(std::unique_ptr<Button> newButton){
	endTurnButton = std::move(newButton);
	endTurnButton->SetVisible(false);
}

void GameWindow::Render(){
	SDL_RenderClear(ren);
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

void GameWindow::ClickObject(GFXObject* toClick){
	if(!toClick){
		std::cerr << "Error: somehow asked to ClickObject a nullptr." << std::endl;
		return;
	}
	if(dynamic_cast<Tile*>(toClick)) ClickTile(dynamic_cast<Tile*>(toClick));
	if(dynamic_cast<Unit*>(toClick)) ClickUnit(dynamic_cast<Unit*>(toClick));
}

void GameWindow::ClickTile(Tile* clickedTile){
	viewCenterRow = clickedTile->Row();
	viewCenterCol = clickedTile->Colm();
	if(clickedTile->HasColony()){
		//std::cout << "Clicked on a colony, setting newFocusColony to "
			//<< clickedTile->LinkedColony() << "." << std::endl;
		if(currentFocusColony != clickedTile->LinkedColony()){
			newFocusColony = clickedTile->LinkedColony();
		} else {
			newFocusColony = nullptr;
		}
	}
}

void GameWindow::ClickUnit(Unit*){
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
			if(clickables[i]->Click()){
				ClickObject(clickables[i]);
				return selected;
			}
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

signal_t GameWindow::HandleKeyPress(SDL_Keycode key){
	switch(key){
		case SDLK_ESCAPE:
			return QUIT;
		case SDLK_RETURN:
			return NEXT_TURN;
		case SDLK_LEFT:
			viewCenterCol += 2;
			break;
		case SDLK_RIGHT:
			viewCenterCol -= 2;
			break;
		case SDLK_UP:
			viewCenterRow += 1;
			break;
		case SDLK_DOWN:
			viewCenterRow -= 1;
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

void GameWindow::ChangeSelected(GFXObject* newSelected){
	if(newSelected != selected){
		if(selected) selected->Deselect();
	}
	selected = newSelected;
}

void GameWindow::FocusOnColony(const Map& theMap){
	if(!newFocusColony){
		std::cerr << "Error: told to focus on a colony, but newFocusColony was "
			<< "set to nullptr." << std::endl;
		return;
	}
	ResetObjects();
	const int row = newFocusColony->Row();
	const int colm = newFocusColony->Column();
	ForTwoSurrounding(std::function<std::shared_ptr<Tile>(int,int)>(
				[&] (int i, int j) { return theMap.Terrain(i,j); }),
			row, colm, [this] (std::shared_ptr<Tile> t) { if(t) AddObject(t); });
	objects = CheckAndLock(weakObjects);
	AddClickable(theMap.Terrain(row, colm));
	for(auto& obj : objects){
		Tile* tile = dynamic_cast<Tile*>(obj);
		for(auto i = 0u; i < tile->NumberOfOccupants(); ++i){
			AddClickable(tile->SharedOccupant(i));
		}
	}
	clickables = CheckAndLock(weakClickables);
	DrawResources(newFocusColony);
	DrawColonyMisc(newFocusColony);
	currentFocusColony = newFocusColony;
}

void GameWindow::BreakFocusOnColony(Map& theMap){
	if(!currentFocusColony){
		std::cerr << "Error: asked to break focus on a colony, but was not "
			<< "focusing on one to begin with." << std::endl;
		return;
	}
	const int centerRow = currentFocusColony->Row();
	const int centerColumn = currentFocusColony->Column();
	ResetObjects();
	AddMapTiles(theMap, centerRow, centerColumn);
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
	currentFocusColony = nullptr;
}

void GameWindow::DrawResources(const Colony* col){
	for(unsigned int i = 0; i < col->NumberOfResources(); ++i){
		auto newPanel = std::make_unique<UIElement>(ren, 
					Colony::ResourceName(static_cast<resource_t>(i)) + "_panel",
					RES_PANEL_X + i*RES_PANEL_WIDTH, RES_PANEL_Y);
		newPanel->AddDynamicText(col->Resource(i),
				RES_PANEL_WIDTH/2, 2*RES_PANEL_HEIGHT/3);
		AddUI(std::move(newPanel));
	}
}

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

	std::unique_ptr<Button> leaveColonyButton = std::make_unique<Button>(ren,
			"leavecolony", SCREEN_WIDTH-200, 100, 
			std::function<void()>(std::bind(&GameWindow::LeaveColony, this)));
	AddUI(std::move(leaveColonyButton));
}

void GameWindow::ClearOutOfBounds(){
	if(currentFocusColony){
		int centerRow = currentFocusColony->Row();
		int centerCol = currentFocusColony->Column();

		auto it = clickables.begin();
		while(it != clickables.end()){
			if(!(*it)->IsUnit()){
				++it;
				continue;
			}
			Unit* u = dynamic_cast<Unit*>(*it);
			int rowDiff = centerRow - u->Row();
			int colDiff = centerCol - u->Colm();
			if(std::abs(rowDiff) > 2 || std::abs(colDiff) + std::abs(rowDiff) > 4){
				it = clickables.erase(it);
			} else {
				++it;
			}
		}
	}
}

void GameWindow::SelectNew(const int clickX, const int clickY){
	GFXObject* newSelected = SelectedObject(clickX, clickY);
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

signal_t GameWindow::MapScreen(std::shared_ptr<Map> theMap, int centerRow,
		int centerColm){
	MapScreenCenteredOn(*theMap, centerRow, centerColm);

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
					signal_t keySig = HandleKeyPress(e.key.keysym.sym);
					switch(keySig/100){
						//case SCREEN_CHANGE/100:
							//return SCREEN_CHANGE;
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
						SelectNew(e.button.x, e.button.y);
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
		if(currentFocusColony && !newFocusColony){
			BreakFocusOnColony(*theMap);
		}
		if(newFocusColony && newFocusColony != currentFocusColony) FocusOnColony(*theMap);
		theMap->CenterViewOn(viewCenterRow, viewCenterCol);
		ClearOutOfBounds();
		Render();
	}
	return QUIT;
}

void GameWindow::MapScreenCenteredOn(Map& theMap, const int centerRow,
		const int centerColm){
	std::unique_ptr<UIElement> colonyBackground = 
		std::make_unique<UIElement>(ren, COLONY_BACKGROUND, 0, 0);
	ResetBackground(std::move(colonyBackground));

	ResetObjects();
	AddMapTiles(theMap, centerRow, centerColm);
	viewCenterRow = centerRow;
	viewCenterCol = centerColm;

	if(!endTurnButton){
		std::cerr << "Warning: told to make a Map screen but the end turn "
			<< "button was a null pointer." << std::endl;
	} else {
		endTurnButton->MoveTo(SCREEN_WIDTH-200, 200);
		endTurnButton->SetVisible(true);
	}
	clickables = CheckAndLock(weakClickables);
	objects = CheckAndLock(weakObjects);
}

void GameWindow::AddMapTiles(Map& theMap, const int centerRow, 
		const int centerColm){
	/*std::cout << "Constructing a Map centered on [" << centerRow << "," 
		<< centerColm << "]." << std::endl;*/
	for(unsigned int i = 0; i < theMap.NumberOfRows(); ++i){
		for(unsigned int j = i%2; j < theMap.NumberOfColumns(); j+=2){
			theMap.Terrain(i,j)->MoveTo(
					0 + (static_cast<int>(j)-centerColm)*TILE_WIDTH/2,
					0 + (static_cast<int>(i)-centerRow)*TILE_HEIGHT);
			if(theMap.Terrain(i,j)->X() > 0
					&& theMap.Terrain(i,j)->X() < SCREEN_WIDTH
					&& theMap.Terrain(i,j)->Y() > 0
					&& theMap.Terrain(i,j)->Y() < SCREEN_HEIGHT){
				/*std::cout << "The Tile at [" << i << "," << j << "] should "
					<< "appear on the screen at (" << theMap->Terrain(i,j)->X()
					<< "," << theMap->Terrain(i,j)->Y() << ")." << std::endl;*/
			}
			if(theMap.Terrain(i,j)->HasColony()){
				AddClickable(theMap.Terrain(i,j));
			} else {
				AddObject(theMap.Terrain(i,j));
			}
			// this seems silly; should be able to just get roamers from the map
			for(auto k = 0u; k < theMap.Terrain(i,j)->NumberOfOccupants(); ++k){
				AddClickable(theMap.Terrain(i,j)->SharedOccupant(k));
			}
		}
	}
	theMap.SetViewCenter(centerRow, centerColm);
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

// need Build Colony button handed over from Map
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
