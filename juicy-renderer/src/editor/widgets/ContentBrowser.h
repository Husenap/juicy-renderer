#pragma once

#include "Widget.h"

namespace JR::Widgets {

class ContentBrowser : public Widget {
public:
	ContentBrowser()
	    : Widget("Content Browser") {}

protected:
	virtual void Draw() override;

private:
	void DrawTree();
	void DrawTreeRecursive(std::filesystem::path path);
	void DrawContent();

	float mZoom = 100.f;

	std::optional<std::filesystem::path> mSelectedDirectory;
	std::optional<StringId> mHoveredFile;
};

}  // namespace JR::Widgets