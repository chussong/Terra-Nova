#ifndef GFXOBJECT_HPP
#define GFXOBJECT_HPP

#include <memory>
#include "sprite.hpp"
#include "file.hpp"

namespace TerraNova {

class GFXObject : public std::enable_shared_from_this<GFXObject> {
	public:
		template <typename Derived>
		std::shared_ptr<Derived> shared_from_base(){
			return std::static_pointer_cast<Derived>(shared_from_this());
		}

		GFXObject() = delete;
		GFXObject(SDL_Renderer* ren, const File::Path& spritePath, const int x,
				const int y, const bool selectable = false);
		GFXObject(const GFXObject& other) = delete;
		GFXObject(GFXObject&& other) noexcept = default;
		GFXObject& operator=(const GFXObject& other) = delete;
		virtual ~GFXObject() = default;

		// Click() represents a thing something does when you click on it which
		// does not result in the selection of the object; if Click() returns
		// true, then no selection is attempted.
		SDL_Renderer* Renderer() const { return ren; }
		virtual void ChangeSprite(File::Path spritePath);
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
		void FlipHorizontal()   { sprite->FlipHorizontal();   }
		void UnflipHorizontal() { sprite->UnflipHorizontal(); }
		void FlipVertical()     { sprite->FlipVertical();     }
		void UnflipVertical()   { sprite->UnflipVertical();   }
		void Lighten()			{ sprite->Lighten();          }
		void Darken()			{ sprite->Darken();           }

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
		virtual GFXObject* SelectAt(const int x, const int y);

		bool Clickable() const { return selectable; }
		virtual bool IsUnit() const { return false; }
		virtual bool IsTile() const { return false; }
		virtual bool IsBuildingPrototype() const { return false; }
		virtual bool IsBuilding() const { return false; }
	 	virtual bool IsButton() const { return false; }
	 	virtual bool IsInfoPanel() const { return false; }

	protected:
		// sprite is this object's main sprite; selectedSprite is an overlay
		// that's rendered after sprite when the object is selected
		std::shared_ptr<Sprite> sprite;
		std::shared_ptr<Sprite> selectedSprite;
		SDL_Renderer* ren;

		SDL_Rect layout;
		bool visible = true;
		bool selectable = false;
		bool selected = false;
};

} // namespace TerraNova
#endif
