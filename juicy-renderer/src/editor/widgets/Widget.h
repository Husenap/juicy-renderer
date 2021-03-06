#pragma once

namespace JR::Widgets {

class Widget {
public:
	Widget(const char* name)
	    : mName(name) {}

	virtual ~Widget() {}

	void Update() {
		if (!mVisibility) {
			return;
		}

		if (ImGui::Begin(mName, &mVisibility)) {
			ImGui::BeginChild(ImGui::GetID(this));
			Draw();
			ImGui::EndChild();
			DrawContextMenu();
		}

		mIsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		ImGui::End();
	}

	void ToggleVisibility() { mVisibility = !mVisibility; }

	bool IsFocused() const { return mIsFocused; }

protected:
	virtual void Draw() = 0;
	virtual void DrawContextMenu() {}

private:
	const char* mName;
	bool mVisibility = true;
	bool mIsFocused  = false;
};

}  // namespace JR::Widgets