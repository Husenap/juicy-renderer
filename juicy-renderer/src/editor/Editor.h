#pragma once

#include "editor/DiffUtil.h"
#include "editor/ProjectManager.h"
#include "widgets/ContentBrowser.h"
#include "widgets/Hierarchy.h"
#include "widgets/History.h"
#include "widgets/Inspector.h"

namespace JR {

class Editor {
public:
	Editor(ECS& ecs);

	template <typename... Components>
	void Init() {
		(RegisterTransactionHandler<Components>(), ...);
		mInspector.Init<Components...>();
	}

	void Update();

private:
	void DrawDockSpace();
	void DrawMenuBar();
	void HandleKeyPress(const EventKeyPress& e);

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

	ECS& mECS;

	ProjectManager mProjectManager;

	bool mShowEditor = true;

	MessageToken mKeyPressToken;
	MessageToken mTransactionToken;

	std::map<std::uint32_t, std::function<void(const EventTransaction&)>> mTransactionHandlers;

	Widgets::ContentBrowser mContentBrowser;
	Widgets::Hierarchy mHierarchy;
	Widgets::History mHistory;
	Widgets::Inspector mInspector;
};

}  // namespace JR