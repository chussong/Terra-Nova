#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include "gamevars.hpp"
#include "gfxobject.hpp"
#include "unit.hpp"

class UIElement : public GFXObject {
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

	// probably this should be moved to Button; it represents things like which
	// type of Building it is, not things like how many resources are displayed
	//std::vector<int> values;

	public:
		UIElement(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y) : 
			GFXObject(ren, spriteFile, x, y) {}
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

		/*void AddValue(const int val);
		void SetValue(const unsigned int entry, const int newVal);
		int Value(const unsigned int entry) const;*/

		void AddText(const std::string& text, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetText(const std::string& text, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);
		
		// WARNING: you have to make sure the source outlives this object!
		void AddDynamicText(const int& source, const int x, const int y, 
				TTF_Font* font = nullptr, const textcolor_t color = BLACK);
		void SetDynamicText(const int& source, TTF_Font* font = nullptr,
				const textcolor_t color = BLACK);

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
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

		virtual void Render() const
			{ std::cerr << "UIAggregate::Render() shouldn't run!" << std::endl;}
};

class UnitInfoPanel : public UIAggregate {
	std::unique_ptr<UIElement> background;
	std::unique_ptr<UIElement> portrait;
	std::unique_ptr<UIElement> factionIcon;
	std::unique_ptr<UIElement> healthIcon;
	std::unique_ptr<UIElement> attackIcon;

	public:
		UnitInfoPanel(SDL_Renderer* ren, const Unit* source);

		void Render() const;

		void Update(const Unit* source);
		// should use dynamic text instead of doing UpdateHealth explicitly
		void UpdateHealth(const Unit* source);
};

class UnitOrderPanel : public GFXObject {
	//UIElement background;
	std::vector<Button> buttons;
	unsigned int activeButton;

	public:
		UnitOrderPanel(SDL_Renderer* ren, Unit* source);

		void Render() const;

		void Update(Unit* source);
		void AddButton(Button&& newButton);
		bool InsideQ(const int x, const int y); 
		bool Click();

		void MoveTo(int x, int y);

};

#endif
