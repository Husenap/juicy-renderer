#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Viewport : public Widget {
public:
	Viewport()
	    : Widget("Viewport") {}

protected:
	virtual void Draw() override;

private:
};

}  // namespace JR::Widgets