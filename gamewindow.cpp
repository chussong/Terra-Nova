#include "gamewindow.hpp"

void LogSDLError(std::ostream& os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

gfxObject::gfxObject(SDL_Renderer* ren, const std::string& filename,
		const int x, const int y){
	// if(IMG not initialized) print error
	image = IMG_LoadTexture(ren, filename.c_str());
	if(image == nullptr) LogSDLError(std::cout, "LoadTexture");

	clickType = NONCLICKABLE;
	layout.x = x;
	layout.y = y;
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

gfxObject::gfxObject(SDL_Renderer* ren, const std::string& text, 
		const SDL_Color color, TTF_Font* font,
		const int x, const int y){
	if(font == nullptr) font = defaultFont;
	SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
	if(surf == nullptr){
		LogSDLError(std::cout, "TTF_RenderText");
	} else {
		image = SDL_CreateTextureFromSurface(ren, surf);
		SDL_FreeSurface(surf);
		if(image == nullptr) LogSDLError(std::cout, "CreateTexture");
	}
	layout.x = x;
	layout.y = y;
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

gfxObject::gfxObject(gfxObject&& other) noexcept: image(nullptr){
	std::swap(image, other.image);
	std::swap(layout, other.layout);
}

gfxObject::~gfxObject(){
	SDL_Cleanup(image);
}

void gfxObject::SetClickType(const clickable_t newType){
	clickType = newType;
}

void gfxObject::MoveTo(int x, int y){
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	layout.x = std::min(x, SCREEN_WIDTH - layout.w);
	layout.y = std::min(y, SCREEN_HEIGHT - layout.h);
}

void gfxObject::Resize(int w, int h){
	if(w < 0) w = 0;
	if(h < 0) h = 0;
	layout.w = w;
	layout.h = h;
}

void gfxObject::RenderTo(SDL_Renderer* ren) const{
	if(SDL_RenderCopy(ren, image, nullptr, &layout))
		LogSDLError(std::cout, "RenderCopy");
}

int gfxObject::LeftEdge() const{
	return layout.x;
}

int gfxObject::RightEdge() const{
	return layout.x + layout.w - 1;
}

int gfxObject::TopEdge() const{
	return layout.y;
}

int gfxObject::BottomEdge() const{
	return layout.y + layout.h - 1;
}

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

gameWindow::~gameWindow(){
	SDL_Cleanup(ren, win);
}

void gameWindow::Render(){
	SDL_RenderClear(ren);
	for(unsigned int i = 0; i < background.size(); ++i) background[i].RenderTo(ren);
	for(unsigned int i = 0; i < clickable.size(); ++i) clickable[i].RenderTo(ren);
	for(unsigned int i = 0; i < objects.size(); ++i) objects[i].RenderTo(ren);
	for(unsigned int i = 0; i < buttons.size(); ++i) buttons[i].RenderTo(ren);
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
	defaultFont = TTF_OpenFont(DEFAULT_FONT, DEFAULT_FONT_SIZE);
	return true;
}

void gameWindow::QuitSDL(){
	TTF_CloseFont(defaultFont);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void gameWindow::AddObject(std::string filename, const int x, const int y){
	objects.emplace_back(ren, filename, x, y);
}

gfxObject* gameWindow::Object(const int num){
	if(num > static_cast<long>(objects.size()) || num < 0){
		std::cout << "Error: tried to access gfxObject " << num <<
			", which does not exist." << std::endl;
		return nullptr;
	}
	return &objects[num];
}

gfxObject* gameWindow::ClickedObject(const int x, const int y){
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
	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 255;
	for(int i = 0; i < static_cast<int>(LAST_RESOURCE); ++i){
		objects.emplace_back(ren, col->ResAsString(i), color, defaultFont,
				RESOURCE_X + (2*i+1)*RESOURCE_WIDTH/2, RESOURCE_Y + 60);

	}
	std::string spriteDir = GetSpritePath("sprites");
	buttons.emplace_back(ren, spriteDir + "endturn.png", SCREEN_WIDTH-200, 200);
	buttons[buttons.size()-1].SetClickType(ENDTURN);
}
