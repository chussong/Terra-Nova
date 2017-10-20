#include "sprite.hpp"

namespace TerraNova {

//TTF_Font* Sprite::defaultFont;
SDL_Renderer* GFXManager::ren;
std::vector<std::string> GFXManager::loadedSpriteNames;
std::vector<std::shared_ptr<Sprite>> GFXManager::loadedSprites;

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

Sprite::Sprite(SDL_Renderer* ren, const std::string filename,
		SDL_Rect layout){
	image = IMG_LoadTexture(ren, filename.c_str());
	if(image == nullptr) LogSDLError(std::cout, "LoadTexture");

	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
	// these two lines below seem very bad actually
	if(layout.w >= SCREEN_WIDTH) layout.w = SCREEN_WIDTH - 1;
	if(layout.h >= SCREEN_HEIGHT) layout.h = SCREEN_HEIGHT - 1;
}

Sprite::Sprite(SDL_Renderer* ren, const std::string filename, const int x, const int y):
		Sprite(ren, filename, PackIntoRect(x,y)) {}

Sprite::Sprite(SDL_Renderer* ren, const std::string text,
		SDL_Rect layout, const SDL_Color color, TTF_Font* font){
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

void Sprite::RenderTo(SDL_Renderer* ren, const SDL_Rect& layout) const{
	if(SDL_RenderCopy(ren, image, nullptr, &layout))
		LogSDLError(std::cout, "RenderCopy");
/*std::cout << "Rendered the image at " << image <<
		" to the renderer at " << ren << " in position (" <<
		layout.x << "," << layout.y << "), (" << layout.w << "," << layout.h
		<< ")." << std::endl;*/
}

void Sprite::RenderTo(const SDL_Rect& layout) const{
	if(SDL_RenderCopy(GFXManager::Ren(), image, nullptr, &layout))
		LogSDLError(std::cout, "RenderCopy");
/*std::cout << "Rendered the image at " << image <<
		" to the renderer at " << ren << " in position (" <<
		layout.x << "," << layout.y << "), (" << layout.w << "," << layout.h
		<< ")." << std::endl;*/
}

void Sprite::MakeDefaultSize(SDL_Rect& layout) const{
	SDL_QueryTexture(image, NULL, NULL, &layout.w, &layout.h);
}

void GFXManager::Initialize(SDL_Renderer* newRen){
	GFXManager::ren = newRen;
	//std::cout << "GFXManager initialized." << std::endl;
}

std::string GFXManager::GetSpritePath(const std::string& subDir){
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	static std::string baseSpriteDir;
	if(baseSpriteDir.empty()){
		char* basePath = SDL_GetBasePath();
		if(basePath){
			baseSpriteDir = basePath;
			baseSpriteDir += std::string("sprites") + PATH_SEP;
			SDL_free(basePath);
		} else {
			std::cerr << "Error getting resource Path: " << SDL_GetError()
				<< std::endl;
			return "";
		}
		size_t pos = baseSpriteDir.rfind("bin");
		baseSpriteDir = baseSpriteDir.substr(0, pos);
	}

	return subDir.empty() ? baseSpriteDir : baseSpriteDir + subDir + PATH_SEP;
}

std::shared_ptr<Sprite> GFXManager::LoadSprite(std::string name){
	if (name.empty()) {
		std::cerr << "Error: asked to load a sprite with an empty name."
			<< std::endl;
		return nullptr;
	}
	boost::replace_all(name, " ", "_");
	boost::to_lower(name);

	SDL_Rect layout;
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(ren, 
			GetSpritePath() + name + ".png", layout);
	loadedSprites.push_back(sprite);
	loadedSpriteNames.push_back(name);
	/*std::cout << "Loaded the sprite \"" << name << "\" at position "
		<< loadedSprites.size()-1 << "=" << loadedSpriteNames.size()-1 << "."
		<< std::endl;*/
	return sprite;
}

/* If this ever gets slow, try swapping each sprite forward by 1 every time
 * it's found, causing more frequently requested sprites to be easier to get.*/
std::shared_ptr<Sprite> GFXManager::RequestSprite(std::string name){
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	for(unsigned int i = 0; i < loadedSpriteNames.size(); ++i){
		if(loadedSpriteNames[i] == name){
			/*std::cout << "Found the requested sprite, \"" << name << "\", at "
				<< "position " << i << "." << std::endl;*/
			return loadedSprites[i];
		}
	}
	return LoadSprite(name);
}

} // namespace TerraNova
