#ifndef GFXOBJECT_HPP
#define GFXOBJECT_HPP

#include <memory>
#include "sprite.hpp"

class GFXObject : public std::enable_shared_from_this<GFXObject> {
	protected:
		std::shared_ptr<Sprite> sprite;
		std::shared_ptr<Sprite> selectedSprite;
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

		GFXObject() = delete;
		GFXObject(SDL_Renderer* ren, const std::string& spriteFile, const int x,
				const int y, const bool selectable = false);
		GFXObject(const GFXObject& other) = delete;
		GFXObject(GFXObject&& other) noexcept = default;
		GFXObject& operator=(const GFXObject& other) = delete;
		virtual ~GFXObject() = default;

		// Click() represents a thing something does when you click on it which
		// does not result in the selection of the object; if Click() returns
		// true, then no selection is attempted.
		virtual void ChangeSprite(const std::string& spriteName);
		virtual void Render() const;
		virtual int Select();
		virtual void Deselect();
		virtual bool Click() { return false; }
		void SetVisible(const bool newVal) { visible = newVal; }

		virtual std::string HoverText() const;

		virtual void MoveTo(int x, int y);
		virtual void MoveTo(SDL_Rect newLayout);
		virtual void Resize(int w, int h);
		virtual void Resize(SDL_Rect newLayout);

		int X() const;
		int Y() const;
		int W() const;
		int H() const;
		const SDL_Rect& Layout() const;
		int LeftEdge() const;
		int RightEdge() const;
		int TopEdge() const;
		int BottomEdge() const;
		virtual bool InsideQ(const int x, const int y);

		bool Clickable() const { return selectable; }
		virtual bool IsUnit() const { return false; }
		virtual bool IsTile() const { return false; }
		virtual bool IsBuildingPrototype() const { return false; }
		virtual bool IsButton() const { return false; }
};

#endif
