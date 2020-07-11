#pragma once

#include "world/Scene.h"

namespace JR {

class App {
public:
	App();

	bool Start();
	bool Run();

private:
	Scene mScene;
};

}  // namespace JR