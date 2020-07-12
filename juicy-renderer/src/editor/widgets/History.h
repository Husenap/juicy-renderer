#pragma once

#include "Widget.h"

namespace JR::Widgets {

class History : public Widget {
public:
	History()
	    : Widget("History") {}

protected:
	virtual void Draw() override;

private:
};

}  // namespace JR::Widgets
