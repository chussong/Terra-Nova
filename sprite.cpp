#include "sprite.hpp"

namespace TerraNova {

//TTF_Font* Sprite::defaultFont;
SDL_Renderer* GFXManager::ren;
std::unordered_map<std::string, std::shared_ptr<Sprite>> GFXManager::loadedSprites;

SDL_Rect MakeSDLRect(const int x, const int y, const unsigned int w,
		const unsigned int h) {
	SDL_Rect ret;
	ret.x = x;
	ret.y = y;
	ret.w = w;
	ret.h = h;
	return ret;
}

SDL_Color Sprite::SDLifyTextColor(const textcolor_t color){
	SDL_Color colorCode;
	switch(color){
enum textcolor_t { BLACK = 0, RED = 1, BLUE = 2, GREEN = 3,
	LAST_COLOR = 4 };
		case BLACK: 	colorCode.r = 0;
						colorCode.g = 0;
						colorCode.b = 0;
						colorCode.a = 255;
						break;
		case RED  : 	colorCode.r = 150;
						colorCode.g = 25;
						colorCode.b = 50;
						colorCode.a = 255;
						break;
		case BLUE : 	colorCode.r = 50;
						colorCode.g = 25;
						colorCode.b = 150;
						colorCode.a = 255;
						break;
		case GREEN: 	colorCode.r = 25;
						colorCode.g = 150;
						colorCode.b = 50;
						colorCode.a = 255;
						break;
		default:		colorCode.r = 0;
						colorCode.g = 0;
						colorCode.b = 0;
						colorCode.a = 255;
						break;
	}
	return colorCode;
}

SDL_Rect Sprite::PackIntoRect(const int x, const int y){
	SDL_Rect ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

Sprite::Sprite(SDL_Renderer* ren, File::Path filePath,
		SDL_Rect layout) {
	image = IMG_LoadTexture(ren, filePath.c_str());
	if (image == nullptr) {
		LogSDLError(std::cout, "LoadTexture");
		File::Path smPath = File::BasePath() / "sprites" / "sprite_missing.png";
		image = IMG_LoadTexture(ren, smPath.c_str());
		if (image == nullptr) {
			std::cerr << "Error: failed to load " << smPath.c_str() <<std::endl;
			throw std::runtime_error("Sprite files not found.");
		}
	}

	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	// these two lines below seem very bad actually
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

Sprite::Sprite(SDL_Renderer* ren, const File::Path& filePath, const int x, 
		const int y):
		Sprite(ren, filePath, PackIntoRect(x,y)) {}

Sprite::Sprite(SDL_Renderer* ren, const std::string& text,
		SDL_Rect layout, const SDL_Color color, TTF_Font* font) {
	if(font == nullptr) font = defaultFont;
/*	std::cout << "Writing \"" << text << "\" at (" << layout.x << "," <<
		layout.y << ") using the font at " << font << "." << std::endl;*/
	SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color,
			9*layout.w/10);
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

Sprite::Sprite(Sprite&& other) noexcept: image(nullptr){
	std::swap(image, other.image);
}

Sprite::~Sprite(){
	SDL_Cleanup(image);
}

void Sprite::RenderTo(SDL_Renderer* ren, const SDL_Rect& layout) const {
	int errorCode = SDL_RenderCopyEx(ren, image, nullptr, &layout, 0, nullptr,
			orientation);
	if (errorCode != 0) LogSDLError(std::cout, "RenderCopyEx");
/*std::cout << "Rendered the image at " << image <<
		" to the renderer at " << ren << " in position (" <<
		layout.x << "," << layout.y << "), (" << layout.w << "," << layout.h
		<< ")." << std::endl;*/
}

void Sprite::RenderTo(const SDL_Rect& layout) const {
	RenderTo(GFXManager::Ren(), layout);
}

void Sprite::MakeDefaultSize(SDL_Rect& layout) const{
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
}

void Sprite::FlipHorizontal() {
	orientation = static_cast<SDL_RendererFlip>(
			orientation | SDL_FLIP_HORIZONTAL );
}

void Sprite::UnflipHorizontal() {
	orientation = static_cast<SDL_RendererFlip>(
			orientation & ~SDL_FLIP_HORIZONTAL );
}

void Sprite::FlipVertical() {
	orientation = static_cast<SDL_RendererFlip>(
			orientation | SDL_FLIP_VERTICAL );
}

void Sprite::UnflipVertical() {
	orientation = static_cast<SDL_RendererFlip>(
			orientation & ~SDL_FLIP_VERTICAL );
}

void Sprite::Darken() {
	unsigned char* oldR = nullptr;
	unsigned char* oldG = nullptr;
	unsigned char* oldB = nullptr;
	int errorCode = SDL_GetTextureColorMod(image, oldR, oldG, oldB);
	if (errorCode != 0) LogSDLError(std::cout, "GetTextureColorMod");
	unsigned char r, g, b;
	if (oldR == nullptr) {
		r = 255 * 0.5;
	} else {
		r = *oldR * 0.5;
	}
	if (oldG == nullptr) {
		g = 255 * 0.5;
	} else {
		g = *oldG * 0.5;
	}
	if (oldB == nullptr) {
		b = 255 * 0.5;
	} else {
		b = *oldB * 0.5;
	}
	SDL_SetTextureColorMod(image, r, g, b);
}

void Sprite::Lighten() {
	unsigned char* oldR = nullptr;
	unsigned char* oldG = nullptr;
	unsigned char* oldB = nullptr;
	int errorCode = SDL_GetTextureColorMod(image, oldR, oldG, oldB);
	if (errorCode != 0) LogSDLError(std::cout, "GetTextureColorMod");
	unsigned char r, g, b;
	if (oldR == nullptr) {
		r = 255;
	} else {
		r = *oldR * 2;
		if (r < *oldR) r = 255;
	}
	if (oldG == nullptr) {
		g = 255;
	} else {
		g = *oldG * 2;
		if (g < *oldG) g = 255;
	}
	if (oldB == nullptr) {
		b = 255;
	} else {
		b = *oldB * 2;
		if (b < *oldB) b = 255;
	}
	SDL_SetTextureColorMod(image, r, g, b);
}

void GFXManager::Initialize(SDL_Renderer* newRen) {
	GFXManager::ren = newRen;
	//std::cout << "GFXManager initialized." << std::endl;
}

File::Path GFXManager::GetSpritePath(const std::string& subDir) {
	return File::BasePath() / "sprites" / subDir;
}

void GFXManager::LoadSprite(File::Path spritePath) {
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(ren, 
			spritePath, SDL_Rect());
	loadedSprites.insert({spritePath.string(), sprite});
	/*std::cout << "Loaded the sprite \"" << name << "\" at position "
		<< loadedSprites.size()-1 << "=" << loadedSpriteNames.size()-1 << "."
		<< std::endl;*/
}

std::shared_ptr<Sprite> GFXManager::RequestSprite(File::Path spritePath) {
	std::string name = spritePath.filename().string() + ".png";
	boost::replace_all(name, " ", "_");
	boost::to_lower(name);
	// std::cout << "Replacing " << spritePath.filename() << " -> " << name 
		// << std::endl;
	spritePath.remove_filename();
	spritePath /= name;

	if (loadedSprites.count(spritePath.string()) == 0) {
		LoadSprite(spritePath);
	}

	return loadedSprites[spritePath.string()];
}

} // namespace TerraNova
