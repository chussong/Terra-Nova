#include "gfxobject.hpp"

TTF_Font* gfxObject::defaultFont;

gfxObject::gfxObject(SDL_Renderer* ren, const std::string& filename,
		SDL_Rect& layout){
	// if(IMG not initialized) print error
	image = IMG_LoadTexture(ren, filename.c_str());
	if(image == nullptr) LogSDLError(std::cout, "LoadTexture");

	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

gfxObject::gfxObject(SDL_Renderer* ren, const std::string& text,
		SDL_Rect& layout, const SDL_Color color, TTF_Font* font){
	if(font == nullptr) font = defaultFont;
	SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
	if(surf == nullptr){
		LogSDLError(std::cout, "TTF_RenderText");
	} else {
		image = SDL_CreateTextureFromSurface(ren, surf);
		SDL_FreeSurface(surf);
		if(image == nullptr) LogSDLError(std::cout, "CreateTexture");
	}
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

gfxObject::gfxObject(gfxObject&& other) noexcept: image(nullptr){
	std::swap(image, other.image);
}

gfxObject::~gfxObject(){
	SDL_Cleanup(image);
}

void gfxObject::RenderTo(SDL_Renderer* ren, const SDL_Rect& layout) const{
	if(SDL_RenderCopy(ren, image, nullptr, &layout))
		LogSDLError(std::cout, "RenderCopy");
}
