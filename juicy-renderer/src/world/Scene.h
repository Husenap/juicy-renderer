#pragma once

#include <entt/entity/registry.hpp>

#include "editor/DiffUtil.h"

namespace JR {
class Scene {
public:
	bool Init();
	void Update(float time);

private:
	void UpdateEditor();

	void DrawDockSpace();
	void DrawInspector();
	void DrawHierarchy();
	void DrawHistory();

	template <typename... Components>
	void DrawEntityComponents(entt::entity entity) {
		(DrawEntityComponent<Components>(entity), ...);
	}
	template <typename Component>
	void DrawEntityComponent(entt::entity entity) {
		DiffUtil::SetActiveEntity(entity);
		if (mECS.has<Component>(entity)) {
			auto& component = mECS.get<Component>(entity);

			View(component);
		}
	}

	entt::registry mECS;
	std::optional<entt::entity> mSelectedEntity;

	float mCurrentTime;
	float mLastTime;
	float mDeltaTime;

	bool mShowGUI = true;
	MessageToken mShowGUIToken;
};
}  // namespace JR
