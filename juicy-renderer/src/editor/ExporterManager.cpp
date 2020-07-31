#include "ExporterManager.h"

#include <juicy-fd/FileDialog.h>

#include "framework/Framework.h"
#include "framework/Window.h"

static constexpr const char* POPUP_EXPORT_SEQUENCE = "Export Sequence";
static constexpr const char* POPUP_PROGRESS        = "Exporting...";

namespace JR {

ExporterManager::ExporterManager() {
	mKeyPressToken = MM::Get<Window>().Subscribe<EventKeyPress>([&](const EventKeyPress& e) {
		if (mState == State::Idle && e.key == GLFW_KEY_E && e.mods == GLFW_MOD_CONTROL) {
			GoToState(State::Prompt);
		}
	});
}

void ExporterManager::Update() {
	switch (mState) {
	case State::Idle:
		break;
	case State::Prompt:
		Prompt();
		break;
	case State::Export:
		Export();
		break;
	}
}

void ExporterManager::Prompt() {
	if (!ImGui::IsPopupOpen(POPUP_EXPORT_SEQUENCE)) {
		ImGui::OpenPopup(POPUP_EXPORT_SEQUENCE);
	}
	if (ImGui::BeginPopupModal(POPUP_EXPORT_SEQUENCE, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::DragInt("Frame Rate", &mConfig.frameRate, 1, 1, 240);
		ImGui::DragInt("Seconds", &mConfig.seconds, 1, 1, 60);

		ImGui::Separator();

		if (ImGui::Button("Export", ImVec2(120, 0))) {
			auto saveFile = JFD::FileDialog::SaveFile({L"png"});
			if (saveFile) {
				mConfig.path = *saveFile;
				if (mConfig.path.has_extension()) {
					mConfig.path.replace_extension();
				}
				GoToState(State::Export);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			GoToState(State::Idle);
		}
		ImGui::EndPopup();
	}
}

void ExporterManager::Export() {
	++mConfig.currentFrame;

	const int numFrames = (mConfig.frameRate * mConfig.seconds);
	if (mConfig.currentFrame > numFrames) {
		GoToState(State::Idle);
		return;
	}

	auto& renderer = MM::Get<Framework>().Renderer();

	renderer.Submit(
	    RCScreenshot{.time       = (mConfig.currentFrame - 1) * (1.f / static_cast<float>(mConfig.frameRate)),
	                 .outputPath = fmt::format("{0}_{1:#04d}.png", mConfig.path.generic_string(), mConfig.currentFrame)});

	if (!ImGui::IsPopupOpen(POPUP_PROGRESS)) {
		ImGui::OpenPopup(POPUP_PROGRESS);
	}
	if (ImGui::BeginPopupModal(POPUP_PROGRESS, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		const auto text = fmt::format("Frame {}/{}", mConfig.currentFrame, numFrames);
		float progress  = mConfig.currentFrame / static_cast<float>(numFrames);

		ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), text.c_str());

		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			GoToState(State::Idle);
		}
		ImGui::EndPopup();
	}
}

void ExporterManager::GoToState(State state) {
	switch (state) {
	case State::Idle:
		break;
	case State::Prompt:
		break;
	case State::Export:
		mConfig.currentFrame = 0;
		break;
	}
	mState = state;
}

}  // namespace JR