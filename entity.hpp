#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <memory>
#include "gfxobject.hpp"

class gfxObject;
class entity {
	protected:
		std::unique_ptr<gfxObject> sprite;
		std::unique_ptr<gfxObject> selectedSprite;
		SDL_Renderer* ren;

		SDL_Rect layout;
		bool selectable = false;
		bool selected = false;

	public:
		entity() = delete;
		entity(SDL_Renderer* ren, const std::string& spriteFile, const int x,
				const int y, const bool selectable = false);
		entity(const entity& other) = delete;
		entity(entity&& other) noexcept = default;
		entity& operator=(const entity& other) = delete;

		virtual void Render() const;
		virtual int Select();
		virtual void Deselect();

		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
		void Resize(int w, int h);
		void Resize(SDL_Rect newLayout);

		int X() const;
		int Y() const;
		int W() const;
		int H() const;
		int LeftEdge() const;
		int RightEdge() const;
		int TopEdge() const;
		int BottomEdge() const;
		virtual bool InsideQ(const int x, const int y) const;

		bool Clickable() const { return selectable; }
};

#endif
