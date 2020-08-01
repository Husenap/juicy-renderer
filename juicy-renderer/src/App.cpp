#include "App.h"

#include "editor/ContentManager.h"
#include "framework/Framework.h"
#include "framework/TextureManager.h"
#include "framework/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace JR {

App::App() {}

bool App::Start() {
	MM::AddModule<Logger>();
	MM::AddModule<FileWatcher>();

	MM::AddModule<Window>();
	MM::AddModule<Framework>();
	MM::AddModule<TransactionManager>();
	MM::AddModule<ContentManager>();
	MM::AddModule<TextureManager>();

	if (!MM::Get<Window>().Create("Juicy", 1600, 900)) {
		return false;
	}

	if (!MM::Get<Framework>().Initialize()) {
		return false;
	}

	mScene = std::make_unique<Scene>();

	if (!Run()) {
		return false;
	}

	MM::UnloadAll();

	return true;
}

bool App::Run() {
	auto& framework = MM::Get<Framework>();

	while (!MM::Get<Window>().ShouldClose()) {
		glfwPollEvents();

		if (!framework.IsPaused()) {
			MM::Get<Framework>().BeginFrame();
			mScene->Update(static_cast<float>(glfwGetTime()));
			MM::Get<Framework>().Render();
			MM::Get<Framework>().EndFrame();

			MM::Get<FileWatcher>().FlushChanges();
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		std::this_thread::yield();
	}

	return true;
}

}  // namespace JR