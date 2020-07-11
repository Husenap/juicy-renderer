#pragma once

namespace JR::Components {

struct Identification {
	std::string name;
};

void View(Identification& identification) {
	ImGui::InputText("Name", &identification.name, ImGuiInputTextFlags_AutoSelectAll);
}

}  // namespace JR::Components