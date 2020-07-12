#pragma once

#include <entt/entity/registry.hpp>

#include "editor/DiffUtil.h"
#include "editor/ProjectManager.h"

namespace JR {

class Editor {
public:
	Editor(entt::registry& ecs);

	template <typename... Components>
	void Init() {
		(RegisterTransactionHandler<Components>(), ...);
		(RegisterComponentDrawer<Components>(), ...);
	}

	void Update();

private:
	void DrawDockSpace();
	void DrawInspector();
	void DrawHierarchy();
	void DrawHistory();
	void DrawContentBrowser();

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

	ProjectManager mProjectManager;

	entt::registry& mECS;
	std::optional<entt::entity> mSelectedEntity;

	bool mShowEditor = true;

	MessageToken mKeyPressToken;
	MessageToken mTransactionToken;

	std::map<std::uint32_t, std::function<void(const EventTransaction&)>> mTransactionHandlers;
	std::vector<std::function<void(entt::entity)>> mComponentDrawers;
};

}  // namespace JR