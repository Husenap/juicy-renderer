#pragma once

#include "Widget.h"

namespace JR::Widgets {

class Hierarchy : public Widget {
public:
	Hierarchy(ECS& ecs);

	template <typename... Components>
	void Init() {
		(RegisterComponentCopiers<Components>(), ...);
	}

protected:
	virtual void Draw() override;
	virtual void DrawContextMenu() override;

private:
	template <typename Component>
	void RegisterComponentCopiers() {
		auto componentId = Components::ComponentTypeId::Get<Component>();

		mComponentCopiers[componentId] = [&](entt::entity entity) -> std::optional<std::vector<uint8_t>> {
			if (!mECS.has<Component>(entity)) {
				return std::nullopt;
			}

			auto& component = mECS.get<Component>(entity);

			std::vector<uint8_t> data;
			data.resize(sizeof(component));
			std::memcpy(data.data(), &component, sizeof(component));

			return data;
		};
	}

	auto CreateEntitySnapshot(entt::entity entity) {
		std::unordered_map<uint32_t, std::vector<uint8_t>> snapshot;

		for (auto& [componentId, copier] : mComponentCopiers) {
			auto optData = copier(entity);
			if (optData) {
				snapshot[componentId] = *optData;
			}
		}

		return snapshot;
	}

	void DeleteSelectedEntity();
	void CreateNewEntity();

	ECS& mECS;

	MessageToken mKeyPressToken;

	std::unordered_map<uint32_t, std::function<std::optional<std::vector<uint8_t>>(entt::entity entity)>>
	    mComponentCopiers;
};

}  // namespace JR::Widgets