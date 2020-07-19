#pragma once

#include "editor/DiffUtil.h"
#include "editor/ProjectManager.h"
#include "widgets/ContentBrowser.h"
#include "widgets/Hierarchy.h"
#include "widgets/History.h"
#include "widgets/Inspector.h"
#include "widgets/Viewport.h"

namespace JR {

class Scene;

class Editor {
public:
	Editor(Scene& scene, ECS& ecs);

	template <typename... Components>
	void Init() {
		(RegisterTransactionHandler<Components>(), ...);
		mInspector.Init<Components...>();
		mHierarchy.Init<Components...>();
	}

	void Update();

private:
	void DrawDockSpace();
	void DrawMenuBar();
	void HandleKeyPress(const EventKeyPress& e);

	template <typename Component>
	void RegisterTransactionHandler() {
		auto componentId = Components::ComponentTypeId::Get<Component>();

		mTransactionHandlers[componentId] = [&](const EventComponentTransaction& action) {
			if (!mECS.valid(action.entity)) {
				LOG_ERROR("Tried to handle transaction for invalid entity: component: {}", Component::GetDisplayName());
				return;
			}

			if (!mECS.has<Component>(action.entity)) {
				LOG_ERROR("Tried to handle transaction for entity that's missing a component: component: {}",
				          Component::GetDisplayName());
				return;
			}

			auto& component = mECS.get<Component>(action.entity);
			std::memcpy(&component, action.data.data(), action.data.size());
		};

		mAddComponentHandlers[componentId] = [&](const EventAddComponent& action) {
			if (!mECS.valid(action.entity)) {
				LOG_ERROR("Tried to add component on invalid entity: component: {}", Component::GetDisplayName());
				return;
			}

			if (mECS.has<Component>(action.entity)) {
				LOG_ERROR("Tried to add component on entity that already has component: {}",
				          Component::GetDisplayName());
				return;
			}

			Component& componentData = mECS.emplace<Component>(action.entity);
			if (!action.data.empty()) {
				std::memcpy(&componentData, action.data.data(), sizeof(componentData));
			}
		};

		mRemoveComponentHandlers[componentId] = [&](const EventRemoveComponent& action) {
			if (!mECS.valid(action.entity)) {
				LOG_ERROR("Tried to remove component from invalid entity: component: {}", Component::GetDisplayName());
				return;
			}

			if (!mECS.has<Component>(action.entity)) {
				LOG_ERROR("Tried to remove component from entity that's missing that component: {}",
				          Component::GetDisplayName());
				return;
			}

			mECS.remove<Component>(action.entity);
		};
	}

	Scene& mScene;
	ECS& mECS;

	ProjectManager mProjectManager;

	bool mShowEditor = true;

	MessageToken mKeyPressToken;

	std::map<uint32_t, std::function<void(const EventComponentTransaction&)>> mTransactionHandlers;
	MessageToken mTransactionToken;
	std::map<uint32_t, std::function<void(const EventAddComponent&)>> mAddComponentHandlers;
	MessageToken mAddComponentToken;
	std::map<uint32_t, std::function<void(const EventRemoveComponent&)>> mRemoveComponentHandlers;
	MessageToken mRemoveComponentToken;
	MessageToken mAddEntityToken;
	MessageToken mRemoveEntityToken;

	Widgets::ContentBrowser mContentBrowser;
	Widgets::Hierarchy mHierarchy;
	Widgets::History mHistory;
	Widgets::Inspector mInspector;
	Widgets::Viewport mViewport;
};

}  // namespace JR