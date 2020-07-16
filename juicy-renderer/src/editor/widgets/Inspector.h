#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Inspector : public Widget {
	struct ListedComponent {
		std::string componentName;
		uint32_t componentId;
	};

public:
	Inspector(ECS& ecs)
	    : Widget("Inspector")
	    , mECS(ecs) {}

	template <typename... Components>
	void Init() {
		(RegisterComponent<Components>(), ...);
		(RegisterComponentDrawer<Components>(), ...);
	}

protected:
	virtual void Draw() override;

private:
	template <typename Component>
	void RegisterComponent() {
		if constexpr (Component::IsUserComponent()) {
			ListedComponent listedComponent{.componentName = Component::GetDisplayName(),
			                                .componentId   = Components::ComponentTypeId::Get<Component>()};
			mComponents.push_back(listedComponent);

			mComponentAdders[listedComponent.componentId] = [&](entt::entity entity) {
				if (!mECS.has<Component>(entity)) {
					MM::Get<TransactionManager>().AddComponent(
					    entity,
					    (std::string("Added Component: ") + Component::GetDisplayName()).c_str(),
					    Components::ComponentTypeId::Get<Component>());
				}
			};
		}
	}

	template <typename Component>
	void RegisterComponentDrawer() {
		mComponentDrawers.push_back([&](entt::entity entity) {
			DiffUtil::SetActiveEntity(entity);
			if (mECS.has<Component>(entity)) {
				auto& component = mECS.get<Component>(entity);

				if (Component::IsUserComponent()) {
					bool shouldNotRemove = true;
					if (ImGui::CollapsingHeader(Component::GetDisplayName(), &shouldNotRemove)) {
						View(component);
					}
					if (!shouldNotRemove) {
						std::vector<uint8_t> data;
						data.resize(sizeof(component));
						std::memcpy(data.data(), &component, sizeof(component));

						MM::Get<TransactionManager>().RemoveComponent(
						    entity,
						    data,
						    (std::string("Removed Component: ") + Component::GetDisplayName()).c_str(),
						    Components::ComponentTypeId::Get<Component>());
					}
				} else {
					View(component);
				}
			}
		});
	}

	ECS& mECS;

	std::vector<ListedComponent> mComponents;
	std::vector<std::function<void(entt::entity)>> mComponentDrawers;

	std::unordered_map<uint32_t, std::function<void(entt::entity)>> mComponentAdders;
};

}  // namespace JR::Widgets