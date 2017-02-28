#include "rendering.hpp"

void LogSDLError(std::ostream& os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

bool RenderingInit(){
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
	return true;
}

void RenderingQuit(){
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// can we make this a unique_ptr or some other RAII class that calls cleanup?
SDL_Window* CreateWindow(const std::string title, const int x, const int y,
	const int width, const int height){
	SDL_Window* win = SDL_CreateWindow(title.c_str(), x, y, width,
			height, SDL_WINDOW_SHOWN);
	if(win == nullptr){
		LogSDLError(std::cout, "CreateWindow");
		RenderingQuit();
		return nullptr;
	}
	return win;
}

// would like to make this a smart pointer too
SDL_Renderer* CreateRenderer(SDL_Window* win){
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == nullptr){
		LogSDLError(std::cout, "CreateRenderer");
		SDL_Cleanup(win);
		RenderingQuit();
		return nullptr;
	}
	return ren;
}

SDL_Texture* LoadTexture(const std::string& file, SDL_Renderer* ren){
	SDL_Texture* texture = IMG_LoadTexture(ren, file.c_str());
	if(texture == nullptr){
		LogSDLError(std::cout, "LoadTexture");
		SDL_Cleanup(texture, ren); // !! this leaks the window containing ren
		RenderingQuit();
		return nullptr;
	}
	return texture;
}

void RenderTexture(SDL_Texture* tex, SDL_Renderer* ren,
		const int x, const int y, const int w, const int h){
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;

	SDL_RenderCopy(ren, tex, NULL, &dst);
}

void RenderTexture(SDL_Texture* tex, SDL_Renderer* ren, const int x, const int y){
	int w,h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	RenderTexture(tex, ren, x, y, w, h);
}

void TileTexture(SDL_Texture* tex, SDL_Renderer* ren){
	int width, height;
	SDL_QueryTexture(tex, NULL, NULL, &width, &height);
	for(int x = 0; x < SCREEN_WIDTH; x += width){
		for(int y = 0; y < SCREEN_HEIGHT; y += height){
			RenderTexture(tex, ren, x, y);
		}
	}
}
