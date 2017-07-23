#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <memory>
#include "gfxobject.hpp"

class gfxObject;
class entity : public std::enable_shared_from_this<entity> {
	protected:
		std::shared_ptr<gfxObject> sprite;
		std::shared_ptr<gfxObject> selectedSprite;
		SDL_Renderer* ren;

		SDL_Rect layout;
		bool visible = true;
		bool selectable = false;
		bool selected = false;

	public:
		template <typename Derived>
		std::shared_ptr<Derived> shared_from_base(){
			return std::static_pointer_cast<Derived>(shared_from_this());
		}

		entity() = delete;
		entity(SDL_Renderer* ren, const std::string& spriteFile, const int x,
				const int y, const bool selectable = false);
		entity(const entity& other) = delete;
		entity(entity&& other) noexcept = default;
		entity& operator=(const entity& other) = delete;

		// Click() represents a thing something does when you click on it which
		// does not result in the selection of the object; if Click() returns
		// true, then no selection is attempted.
		virtual void ChangeSprite(const std::string& spriteName);
		virtual void Render() const;
		virtual int Select();
		virtual void Deselect();
		virtual bool Click() { return false; }
		void SetVisible(const bool newVal) { visible = newVal; }


		virtual void MoveTo(int x, int y);
		virtual void MoveTo(SDL_Rect newLayout);
		virtual void Resize(int w, int h);
		virtual void Resize(SDL_Rect newLayout);

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
		virtual bool IsUnit() const { return false; }
		virtual bool IsTile() const { return false; }
		virtual bool IsBuildingPrototype() const { return false; }
		virtual bool IsButton() const { return false; }
};

#endif
