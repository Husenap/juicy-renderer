#include "App.h"

namespace JR {

App::App()
    : mRenderer(mWindow) {}

bool App::Start() {
	if (!mWindow.Create("Juicy", 800, 600)) {
		return false;
	}

	if (!mRenderer.Initialize()) {
		return false;
	}

	if (!Run()) {
		return false;
	}

	return true;
}

bool App::Run() {
	while (!mWindow.ShouldClose()) {
		glfwPollEvents();

		mRenderer.Render();
	}

	return true;
}

}  // namespace JR