#include "App.h"

#include "renderer/Framework.h"
#include "renderer/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace JR {

App::App() {}

bool App::Start() {
	MM::AddModule<Logger>();
	MM::AddModule<FileWatcher>();
	MM::AddModule<Window>();
	MM::AddModule<Framework>();

	if (!MM::Get<Window>().Create("Juicy", 800, 800)) {
		return false;
	}

	if (!MM::Get<Framework>().Initialize()) {
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

		MM::Get<FileWatcher>().FlushChanges();

		MM::Get<Framework>().Render();

		std::this_thread::yield();
	}

	return true;
}

}  // namespace JR