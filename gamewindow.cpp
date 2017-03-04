#include "gamewindow.hpp"

gameWindow::gameWindow(const std::string& title, const int x, const int y,
		const int w, const int h){
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
}

/*gameWindow::gameWindow(gameWindow&& other): win(nullptr), ren(nullptr),
		background(std::move(other.background)), 
		clickable(std::move(other.clickable)), objects(std::move(other.objects)),
		buttons(std::move(other.buttons)) {
	std::swap(win, other.win);
	std::swap(ren, other.ren);
}*/


gameWindow::~gameWindow(){
	SDL_Cleanup(ren, win);
}

void gameWindow::Render(){
	SDL_RenderClear(ren);
	for(unsigned int i = 0; i < background.size(); ++i) background[i].Render();
	for(unsigned int i = 0; i < clickable.size(); ++i) clickable[i].Render();
	for(unsigned int i = 0; i < objects.size(); ++i) objects[i].Render();
	for(unsigned int i = 0; i < buttons.size(); ++i) buttons[i].Render();
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

void gameWindow::AddObject(std::string filename, const int x, const int y){
	objects.emplace_back(ren, filename, x, y);
}

uiElement* gameWindow::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access uiElement " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return &objects[num];
}

entity* gameWindow::ClickedObject(const int x, const int y){
	for(unsigned int i = buttons.size()-1; i < buttons.size(); --i){
		if(buttons[i].LeftEdge() < x
				&& x < buttons[i].RightEdge()
				&& buttons[i].TopEdge() < y
				&& y < buttons[i].BottomEdge()){
			return &buttons[i];
		}
	}
	for(unsigned int i = clickable.size()-1; i < clickable.size(); --i){
		if(clickable[i].LeftEdge() < x
				&& x < clickable[i].RightEdge()
				&& clickable[i].TopEdge() < y
				&& y < clickable[i].BottomEdge()){
			return &clickable[i];
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

void gameWindow::MakeColonyScreen(const std::shared_ptr<colony> col) {
	std::string spriteDir = GetSpritePath("sprites");
	background.clear();
	clickable.clear();
	objects.clear();
	background.emplace_back(ren, spriteDir + COLONY_BACKGROUND, 0, 0);
	AddResourceElements(); // eventually these will be baked in
	AddInnerRing(col);
	AddOuterRing(col);
	AddColonists(col);
	AddColonyMisc(col);
}

void gameWindow::AddResourceElements(){
	background.emplace_back(ren, GetSpritePath("sprites") + "resources.png",
			RESOURCE_X, RESOURCE_Y);
}

void gameWindow::AddInnerRing(const std::shared_ptr<colony> col){
	std::string spriteDir = GetSpritePath("sprites");
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(0))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH, MAPDISP_ORIGIN_Y);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(1))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(2))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(3))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH, MAPDISP_ORIGIN_Y);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(4))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(5))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
}

void gameWindow::AddOuterRing(const std::shared_ptr<colony> col){
	std::string spriteDir = GetSpritePath("sprites");
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(6))
			+ ".png", MAPDISP_ORIGIN_X + 2*TILE_WIDTH, MAPDISP_ORIGIN_Y);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(7))
			+ ".png", MAPDISP_ORIGIN_X + 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(8))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH, MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(9))
			+ ".png", MAPDISP_ORIGIN_X, MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(10))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH, MAPDISP_ORIGIN_Y + 2*TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(11))
			+ ".png", MAPDISP_ORIGIN_X - 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y + TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(12))
			+ ".png", MAPDISP_ORIGIN_X - 2*TILE_WIDTH, MAPDISP_ORIGIN_Y);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(13))
			+ ".png", MAPDISP_ORIGIN_X - 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(14))
			+ ".png", MAPDISP_ORIGIN_X - TILE_WIDTH, MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(15))
			+ ".png", MAPDISP_ORIGIN_X, MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(16))
			+ ".png", MAPDISP_ORIGIN_X + TILE_WIDTH, MAPDISP_ORIGIN_Y - 2*TILE_HEIGHT);
	background.emplace_back(ren, spriteDir + map::TerrainName(col->Terrain(17))
			+ ".png", MAPDISP_ORIGIN_X + 3*TILE_WIDTH/2, MAPDISP_ORIGIN_Y - TILE_HEIGHT);
}

void gameWindow::AddColonists(const std::shared_ptr<colony> col){
	std::string spriteDir = GetSpritePath("sprites");
	clickable.emplace_back(ren, spriteDir + "colonist.png", 110, SCREEN_HEIGHT-60);
}

void gameWindow::AddColonyMisc(const std::shared_ptr<colony> col){
	std::string spriteDir = GetSpritePath("sprites");
	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;
	SDL_Rect layout;
	layout.x = RESOURCE_X + RESOURCE_WIDTH/2;
	layout.y = RESOURCE_Y + 60;
	for(int i = 0; i < static_cast<int>(LAST_RESOURCE); ++i){
		layout.x += RESOURCE_WIDTH;
		objects.emplace_back(ren, spriteDir + "resources.png", layout.x, layout.y);
		objects[objects.size()-1].AddText(col->ResAsString(i), layout.x, layout.y,
				gfxObject::defaultFont, BLACK);
	}
	buttons.emplace_back(ren, spriteDir + "endturn.png", SCREEN_WIDTH-200, 200);
	buttons[buttons.size()-1].EnableButton(END_TURN);
}
