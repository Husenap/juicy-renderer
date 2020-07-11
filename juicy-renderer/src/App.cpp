#include "App.h"

#include "framework/Framework.h"
#include "framework/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace JR {

App::App() {}

bool App::Start() {
	MM::AddModule<Logger>();
	MM::AddModule<FileWatcher>();
	MM::AddModule<TransactionManager>();

	MM::AddModule<Window>();
	MM::AddModule<Framework>();

	if (!MM::Get<Window>().Create("Juicy", 1600, 900)) {
		return false;
	}

	if (!MM::Get<Framework>().Initialize()) {
		return false;
	}

	if (!mScene.Init()) {
		return false;
	}

	if (!Run()) {
		return false;
	}

	MM::UnloadAll();

	return true;
}

bool App::Run() {
	while (!MM::Get<Window>().ShouldClose()) {
		glfwPollEvents();

		MM::Get<Framework>().BeginFrame();
		mScene.Update(static_cast<float>(glfwGetTime()));
		MM::Get<Framework>().Render();
		MM::Get<Framework>().EndFrame();

		MM::Get<FileWatcher>().FlushChanges();

		std::this_thread::yield();
	}

	return true;
}

}  // namespace JR