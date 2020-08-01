#pragma once

namespace JR {

class ExporterManager {
	enum class State {
		Idle,
		Prompt,
		Export,
	};

	struct ExportConfig {
		int frameRate = 24;
		int seconds = 10;
		std::filesystem::path path;
		int currentFrame = 0;
	};

public:
	ExporterManager();

	void Update();

private:
	void Prompt();
	void Export();
	void GoToState(State state);

	MessageToken mKeyPressToken;
	State mState = State::Idle;

	ExportConfig mConfig;
};

}  // namespace JR
