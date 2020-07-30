#pragma once

namespace JR {

class EditorUtil {
public:
	static void SelectEntity() { Get().mSelectedEntity = entt::null; }
	static void SelectEntity(entt::entity entity) { Get().mSelectedEntity = entity; }

	static entt::entity GetSelectedEntity() { return Get().mSelectedEntity; }

private:
	EditorUtil() {}

	static EditorUtil& Get() {
		static EditorUtil instance;
		return instance;
	}

	entt::entity mSelectedEntity{entt::null};
};

}  // namespace JR