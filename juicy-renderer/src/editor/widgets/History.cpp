#include "History.h"

#include "editor/TransactionManager.h"

namespace JR::Widgets {

void History::Draw() {
	const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth |
	                                     ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen |
	                                     ImGuiTreeNodeFlags_Bullet;

	MM::Get<TransactionManager>().EnumerateUndoStack([](const std::string& commitMessage, bool isActive) {
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if (isActive) {
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}
		ImGui::TreeNodeEx(commitMessage.c_str(), nodeFlags);
	});
	MM::Get<TransactionManager>().EnumerateRedoStack([](const std::string& commitMessage) {
		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		ImGui::TreeNodeEx(commitMessage.c_str(), nodeFlags);
	});
}

}  // namespace JR::Widgets