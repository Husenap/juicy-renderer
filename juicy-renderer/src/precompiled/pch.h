#pragma once

// STL Includes
#include <array>
#include <cmath>
#include <cstdint>
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <variant>
#include <vector>

// Variant helpers
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// Windows Includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// Thirdparty Includes
#include <fmt/format.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stb_image.h>
#include <stb_image_write.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <examples/imgui_impl_dx11.h>
#include <examples/imgui_impl_win32.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <entt/entity/registry.hpp>
using ECS = entt::registry;

// Juicy Renderer Includes
#include "module/Module.h"

#include "util/Logger.h"

#include "components/ComponentTypeId.h"
#include "util/TypeId.h"

#include "module/ModuleManager.h"
#include "util/FileWatcher.h"
#include "util/MessageEmitter.h"
#include "util/StringId.h"
