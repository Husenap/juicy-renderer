#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Hierarchy : public Widget {
public:
	Hierarchy(ECS& ecs);

protected:
	virtual void Draw() override;

private:
	ECS& mECS;

	MessageToken mKeyPressToken;
};

}  // namespace JR::Widgets