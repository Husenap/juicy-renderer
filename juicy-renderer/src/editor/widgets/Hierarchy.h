#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Hierarchy : public Widget {
public:
	Hierarchy(ECS& ecs)
	    : Widget("Hierarchy")
	    , mECS(ecs) {}

protected:
	virtual void Draw() override;

private:
	ECS& mECS;
};

}  // namespace JR::Views