#pragma once

#include "editor/DiffUtil.h"

namespace JR::Components {

namespace IdentificationInternal {
static constexpr std::size_t MAX_LENGTH = 64;
}  // namespace IdentificationInternal

struct Identification {
	Identification() { sprintf(name, "%s", "New Entity"); }
	Identification(const std::string& strName) { sprintf(name, "%s", strName.c_str()); }

	char name[64];

	[[nodiscard]] constexpr static const char* GetDisplayName() { return "Identification"; }
	[[nodiscard]] constexpr static bool IsUserComponent() { return false; }
};

static void View(Identification& identification) {
	ImGui::InputText(
	    "Name", identification.name, IdentificationInternal::MAX_LENGTH, ImGuiInputTextFlags_AutoSelectAll);
	DiffUtil::HandleTransaction(identification, "Identification Name");
}

}  // namespace JR::Components