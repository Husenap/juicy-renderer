#pragma once

namespace JR {

class EditorUtil {
public:
	static void SelectEntity() { Get().mSelectedEntity = std::nullopt; }
	static void SelectEntity(entt::entity entity) { Get().mSelectedEntity = entity; }

	static std::optional<entt::entity> GetSelectedEntity() { return Get().mSelectedEntity; }

private:
	EditorUtil() {}

	static EditorUtil& Get() {
		static EditorUtil instance;
		return instance;
	}

	std::optional<entt::entity> mSelectedEntity;
};

}  // namespace JR