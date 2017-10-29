#ifndef SUBWINDOW_HPP
#define SUBWINDOW_HPP

#include <vector>

#include "ui.hpp"

namespace TerraNova {

// automatically adjusts its width and height to accommodate its buttons
class Subwindow : public UIElement {
	public:
		Subwindow(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y);

		void AddCaption(const std::string& caption);
		void AddButton(std::unique_ptr<Button> toAdd);

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout); // pretty sure this one is unnecessary
		int MoveButtonsTo(int x, int y);

		GFXObject* SelectAt(const int x, const int y);

	private:
		std::vector<std::unique_ptr<Button>> buttons;
};

} // namespace TerraNova

#endif
