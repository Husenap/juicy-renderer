set(target_name "juicy-renderer")

set(src_precompiled
    "src/precompiled/pch.h")

set(src_util
    "src/util/Logger.h"
    "src/util/Logger.cpp"
    "src/util/FileWatcher.h"
    "src/util/MessageEmitter.h"
    "src/util/TypeId.h"
    "src/util/TypeId.cpp")

set(src_module
    "src/module/Module.h"
    "src/module/ModuleManager.h")

set(src_renderer
    "src/renderer/RenderCommand.h"
    "src/renderer/RendererManager.h"
    "src/renderer/RendererManager.cpp"
    "src/renderer/JuicyRenderer.h"
    "src/renderer/JuicyRenderer.cpp")

set(src_framework
    "src/framework/Buffer.h"
    "src/framework/Buffer.cpp"
    "src/framework/Texture.h"
    "src/framework/Texture.cpp"
    "src/framework/SamplerState.h"
    "src/framework/SamplerState.cpp"
    "src/framework/BlendState.h"
    "src/framework/BlendState.cpp"
    "src/framework/ShaderReflection.h"
    "src/framework/ShaderReflection.cpp"
    "src/framework/Shader.h"
    "src/framework/Shader.cpp"
    "src/framework/Framework.h"
    "src/framework/Framework.cpp"
    "src/framework/Window.h"
    "src/framework/Window.cpp")

set(src_world
    "src/world/Scene.h"
    "src/world/Scene.cpp")

set(src_files
    "src/App.h"
    "src/App.cpp"
    ${src_precompiled}
    ${src_util}
    ${src_module}
    ${src_renderer}
    ${src_framework}
    ${src_world})

# Project
add_library(${target_name} ${src_files})

source_group("src" FILES "src/main.cpp")

target_precompile_headers(${target_name} PUBLIC ${src_precompiled})

target_link_libraries(${target_name} d3d11 d3dcompiler CONAN_PKG::glfw CONAN_PKG::stb CONAN_PKG::glm CONAN_PKG::entt imgui)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(${target_name} PROPERTIES FOLDER ${PROJECT_NAME})
set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# Tests
add_executable(${target_name}_test "test/test.cpp")

target_link_libraries(${target_name}_test CONAN_PKG::gtest)

set_target_properties(${target_name}_test PROPERTIES FOLDER ${PROJECT_NAME})

gtest_discover_tests(${target_name}_test)