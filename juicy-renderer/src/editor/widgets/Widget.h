#pragma once

namespace JR::Widgets {

class Widget {
public:
	Widget(const char* name)
	    : mName(name) {}

	void Update() {
		if (!mVisibility) {
			return;
		}

		if (ImGui::Begin(mName, &mVisibility)) {
			Draw();
		}
		ImGui::End();
	}

	void ToggleVisibility() { mVisibility = !mVisibility; }

protected:
	virtual void Draw() = 0;

private:
	const char* mName;
	bool mVisibility = true;
};

}  // namespace JR::Widgets