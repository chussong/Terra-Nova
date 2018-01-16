#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include "gamevars.hpp"
#include "gfxobject.hpp"
#include "unit.hpp"
#include "building.hpp"

namespace TerraNova{

class UIElement : public GFXObject {
	protected:
		std::unique_ptr<Sprite> textSprite = nullptr;
		SDL_Rect textLayout;

		// dynamic text should probably be a class/struct of its own that has
		// formatting codes and knows how to render itself
		int const* dynamicTextSource;
		TTF_Font* dynamicTextFont;
		SDL_Color dynamicTextColor;
		void UpdateDynamicText() const;
		mutable std::unique_ptr<Sprite> dynamicTextSprite = nullptr;
		mutable int dynamicTextCached = 0;
		mutable SDL_Rect dynamicTextLayout = SDL_Rect();

	public:
		UIElement(SDL_Renderer* ren, const std::string& spriteFile,
				const int x, const int y) : 
			GFXObject(ren, File::SpritePath() / "ui" / spriteFile, x, y) {}
		UIElement(const UIElement& other) = delete;
		/*UIElement(UIElement&& other) noexcept : 
			GFXObject(std::move(other)), textSprite(std::move(other.textSprite)),
			textLayout(std::move(other.textLayout)), 
			dynamicTextSource(std::move(other.dynamicTextSource)),
			dynamicTextFont(std::move(other.dynamicTextFont)),
			dynamicTextColor(std::move(other.dynamicTextColor)) {}*/
			//values(std::move(other.values)){}
		UIElement(UIElement&& other) noexcept = default;
		UIElement& operator=(const UIElement& other) = delete;

		void AddText(const std::string& text, const SDL_Rect boundingBox,
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetText(const std::string& text, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);
		
		// WARNING: you have to make sure the source outlives this object!
		void AddDynamicText(const int& source, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetDynamicText(const int& source, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);

		virtual void Render() const;
		virtual void MoveTo(int x, int y);
		virtual void MoveTo(SDL_Rect newLayout);
};

class Button : public UIElement {
	std::function<void()> onClick;

	public:
		Button(SDL_Renderer* ren, const std::string spriteFile, const int x,
				const int y, std::function<void()> onClick): 
			UIElement(ren, spriteFile, x, y), onClick(onClick) {}

		bool IsButton() const { return true; }
		bool Click() { if(!onClick) std::cerr << "Error! Button contains an "
			<< "invalid function!" << std::endl; onClick(); return true; }
};

// this class is dumb, it should just be a GFXObject
class UIAggregate {
	protected:
		SDL_Renderer* ren;
		int x, y;

	public:
		UIAggregate(SDL_Renderer* ren, const int x, const int y): ren(ren),
			x(x), y(y){}
		virtual ~UIAggregate() = default;

		virtual void Render() const;
};

class UnitOrderPanel : public GFXObject {
	//UIElement background;
	std::vector<Button> buttons;
	unsigned int activeButton;

	public:
		UnitOrderPanel(SDL_Renderer* ren, Unit* source);

		void Render() const;

		void Update(Unit& source);
		void AddButton(Button&& newButton);
		bool InsideQ(const int x, const int y); 
		bool Click();

		void MoveTo(int x, int y);

};

} // namespace TerraNova
#endif
