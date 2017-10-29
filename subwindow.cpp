#include "subwindow.hpp"

namespace TerraNova {

Subwindow::Subwindow(SDL_Renderer* ren, const std::string spriteFile,
		const int x, const int y) : UIElement(ren, spriteFile, x, y) {
	layout.x -= layout.w/2;
	layout.y -= layout.h/2;
}

void Subwindow::AddCaption(const std::string& caption) {
	AddText(caption, MakeSDLRect(W()/2, SUBWINDOW_BUTTON_SPACING, W(), H()));
	if (textSprite != nullptr) layout.y -= textLayout.h/2;
}

void Subwindow::AddButton(std::unique_ptr<Button> toAdd) {
	layout.x += layout.w/2;
	layout.w = std::max(layout.w, toAdd->W() + 2*SUBWINDOW_BUTTON_SPACING);
	layout.x -= layout.w/2;
	//layout.y -= toAdd->H()/2;
	buttons.push_back(std::move(toAdd));
	int bottomY = MoveButtonsTo(layout.x, layout.y);
	layout.h = bottomY - Y();
}

void Subwindow::Render() const {
	UIElement::Render();
	for (auto& button : buttons) button->Render();
}

void Subwindow::MoveTo(int x, int y) {
	UIElement::MoveTo(x, y);
	MoveButtonsTo(x, y);
}

void Subwindow::MoveTo(SDL_Rect newLayout) {
	MoveTo(newLayout.x, newLayout.y);
}

// return the location of the bottom of the last button, plus one BUTTON_SPACING
int Subwindow::MoveButtonsTo(int x, int y) {
	int currentY = layout.y + 2*SUBWINDOW_BUTTON_SPACING;
	if (textSprite != nullptr) currentY += SUBWINDOW_BUTTON_SPACING;
	for (auto& button : buttons) {
		button->MoveTo(layout.x + layout.w/2 - button->W()/2, currentY);
		currentY += SUBWINDOW_BUTTON_SPACING + button->H();
	}
	return currentY;
}

GFXObject* Subwindow::SelectAt(const int x, const int y) {
	for (auto& button : buttons) {
		if (button->InsideQ(x, y)) {
			button->Click();
			return nullptr;
		}
	}
	return nullptr;
}

} // namespace TerraNova
