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
/*	std::cout << "Writing \"" << text << "\" at (" << layout.x << "," <<
		layout.y << ") using the font at " << font << "." << std::endl;*/
	SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
	if(surf == nullptr){
		LogSDLError(std::cout, "TTF_RenderText");
	} else {
		image = SDL_CreateTextureFromSurface(ren, surf);
		SDL_FreeSurface(surf);
		if(image == nullptr) LogSDLError(std::cout, "CreateTexture");
	}
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
/*	std::cout << "The image we created from the text \"" << text <<
		"\" is at (" << layout.x << "," << layout.y << ");(" <<
		layout.w << "," << layout.h << "). Its color is (" <<
		color.r << "," << color.g << "," << color.b << "," <<
		color.a << ")." << std::endl;*/
	layout.x -= layout.w/2;
	layout.y -= layout.h/2;
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
/*std::cout << "Rendered the image at " << image <<
		" to the renderer at " << ren << " in position (" <<
		layout.x << "," << layout.y << "), (" << layout.w << "," << layout.h
		<< ")." << std::endl;*/
}
