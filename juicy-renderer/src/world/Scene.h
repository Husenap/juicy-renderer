#pragma once

#include <entt/entity/entity.hpp>
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
	void RegisterTransactionHandlers() {
		(RegisterTransactionHandler<Components>(), ...);
	}
	template <typename Component>
	void RegisterTransactionHandler() {
		mTransactionHandlers[TypeId::Get<Component>()] = [&](const EventTransaction& transaction) {
			if (!mECS.valid(transaction.entity)) {
				LOG_ERROR("Tried to handle transaction for invalid entity: component: %s", typeid(Component).name());
				return;
			}

			if (!mECS.has<Component>(transaction.entity)) {
				LOG_ERROR("Tried to handle transaction for entity that's missing a component: component: %s",
				          typeid(Component).name());
				return;
			}

			auto& component = mECS.get<Component>(transaction.entity);
			std::memcpy(&component, transaction.data.data(), transaction.data.size());
		};
	}

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
	MessageToken mTransactionToken;

	std::map<std::uint32_t, std::function<void(const EventTransaction&)>> mTransactionHandlers;
};
}  // namespace JR
