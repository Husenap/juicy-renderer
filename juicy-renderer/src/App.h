#pragma once

#include "renderer/Renderer.h"
#include "renderer/Window.h"

namespace JR {

class App {
public:
	App();

	bool Start();
	bool Run();

private:
	Renderer mRenderer;
	Window mWindow;
};

}  // namespace JR