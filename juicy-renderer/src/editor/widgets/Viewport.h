#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Viewport : public Widget {
public:
	Viewport(glm::vec4& backgroundColor)
	    : Widget("Viewport")
	    , mBackgroundColor(backgroundColor) {}

protected:
	virtual void Draw() override;
	virtual void DrawContextMenu() override;

private:
	glm::vec4& mBackgroundColor;
};

}  // namespace JR::Widgets