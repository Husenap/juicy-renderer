#pragma once

#include "world/Scene.h"

namespace JR {

class App {
public:
	App();

	bool Start();
	bool Run();

private:
	std::unique_ptr<Scene> mScene;
};

}  // namespace JR