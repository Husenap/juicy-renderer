#include "ContentManager.h"

#include <juicy-fd/FileDialog.h>

#include "framework/Window.h"

namespace JR {

constexpr char const* NAME_CONTENT_DIRECTORY = "Content";

static const std::vector<std::wstring> ACCEPTED_FILES{L"png"};

void ContentManager::CreateNewContent(std::filesystem::path projectPath) {
	std::filesystem::create_directory(projectPath / NAME_CONTENT_DIRECTORY);
}

void ContentManager::Load(std::filesystem::path projectPath) {
	mContentPath = projectPath / NAME_CONTENT_DIRECTORY;

	for (auto& p : std::filesystem::recursive_directory_iterator(mContentPath)) {
		auto path = p.path();
		if (std::filesystem::is_directory(path)) {
			continue;
		}

		mFileMap[StringId::FromPath(path.lexically_relative(mContentPath))] = path;
	}
}

void ContentManager::ImportFile() {
	auto importFile = JFD::FileDialog::OpenFile({L"png"});
	if (!importFile) {
		return;
	}

	ImportFile(*importFile);
}

void ContentManager::ImportFile(std::filesystem::path importFile) {
	if (!importFile.has_extension()) {
		return;
	}
	auto ext  = importFile.extension();
	auto wext = ext.wstring();
	wext.erase(wext.begin());
	if (std::find(ACCEPTED_FILES.begin(), ACCEPTED_FILES.end(), wext) == ACCEPTED_FILES.end()) {
		return;
	}

	auto filename = importFile.filename().generic_string();

	std::filesystem::copy_file(importFile, mContentPath / filename, std::filesystem::copy_options::overwrite_existing);

	mFileMap[StringId::FromPath(filename)] = (mContentPath / filename);
}

std::optional<std::filesystem::path> ContentManager::GetPath(StringId id) const {
	auto it = mFileMap.find(id);

	if (it == mFileMap.end()) {
		return std::nullopt;
	}

	return it->second;
}

}  // namespace JR