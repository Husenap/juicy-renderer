#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Inspector : public Widget {
public:
	Inspector(ECS& ecs)
	    : Widget("Inspector")
	    , mECS(ecs) {}

	template <typename... Components>
	void Init() {
		(RegisterComponentDrawer<Components>(), ...);
	}

protected:
	virtual void Draw() override;

private:
	template <typename Component>
	void RegisterComponentDrawer() {
		mComponentDrawers.push_back([&](entt::entity entity) {
			DiffUtil::SetActiveEntity(entity);
			if (mECS.has<Component>(entity)) {
				auto& component = mECS.get<Component>(entity);

				View(component);
			}
		});
	}

	ECS& mECS;

	std::vector<std::function<void(entt::entity)>> mComponentDrawers;
};

}  // namespace JR::Widgets