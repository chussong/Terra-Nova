#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <memory>
#include "gfxobject.hpp"

enum signal_t { ERROR = 0, SELECTED = 100, NEXT_TURN = 200 };

class gfxObject;
class entity {
	protected:
		std::unique_ptr<gfxObject> sprite;
		SDL_Renderer* ren;

		SDL_Rect layout;
		bool selectable = false;
		bool selected = false;

	public:
		entity() = delete;
		entity(SDL_Renderer* ren, const std::string& spriteFile, const int x,
				const int y);
		entity(const entity& other) = delete;
		entity(entity&& other) noexcept = default;
		entity& operator=(const entity& other) = delete;

		void Render() const;
		virtual signal_t Select();
		virtual void Deselect();

		void MoveTo(int x, int y);
		void Resize(int w, int h);

		int LeftEdge() const;
		int RightEdge() const;
		int TopEdge() const;
		int BottomEdge() const;
		virtual bool InsideQ(const int x, const int y) const;

		bool Clickable() const { return selectable; }
};

#endif
