#pragma once

namespace JR::Components {

const std::size_t MAX_LENGTH = 64;

struct Identification {
	Identification(const std::string& strName) { sprintf(name, "%s", strName.c_str()); }

	char name[64];
};

static void View(Identification& identification) {
	ImGui::InputText("Name", identification.name, MAX_LENGTH, ImGuiInputTextFlags_AutoSelectAll);
	DiffUtil::HandleTransaction(identification, "Identification Name");
}

}  // namespace JR::Components