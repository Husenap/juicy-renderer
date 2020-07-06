#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>
#include <cmath>

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stb_image.h>

#include <glm/glm.hpp>

#include "util/TypeId.h"
#include "util/MessageEmitter.h"
#include "module/Module.h"
#include "module/ModuleManager.h"
