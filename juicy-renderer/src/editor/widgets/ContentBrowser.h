#pragma once

#include "Widget.h"

namespace JR::Widgets {

class ContentBrowser : public Widget {
public:
	ContentBrowser()
	    : Widget("Content Browser") {}

protected:
	virtual void Draw() override;

private:
};

}  // namespace JR::Views