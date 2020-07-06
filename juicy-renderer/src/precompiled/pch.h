#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stb_image.h>

#include <glm/glm.hpp>

#include "module/Module.h"

#include "util/Logger.h"

#include "module/ModuleManager.h"
#include "util/FileWatcher.h"
#include "util/MessageEmitter.h"
#include "util/TypeId.h"
