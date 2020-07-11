#pragma once

#include <entt/entity/registry.hpp>

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

	template <typename... Components>
	void DrawEntityComponents(entt::entity entity) {
		(DrawEntityComponent<Components>(entity), ...);
	}
	template <typename Component>
	void DrawEntityComponent(entt::entity entity) {
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
};
}  // namespace JR
