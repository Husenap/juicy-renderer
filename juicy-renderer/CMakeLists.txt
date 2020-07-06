set(target_name "juicy-renderer")

set(src_precompiled
    "src/precompiled/pch.h")

set(src_util
    "src/util/MessageEmitter.h"
    "src/util/TypeId.h"
    "src/util/TypeId.cpp")

set(src_module
    "src/module/Module.h"
    "src/module/ModuleManager.h")

set(src_renderer
    "src/renderer/Buffer.h"
    "src/renderer/Buffer.cpp"
    "src/renderer/Texture.h"
    "src/renderer/Texture.cpp"
    "src/renderer/BlendState.h"
    "src/renderer/BlendState.cpp"
    "src/renderer/ShaderReflection.h"
    "src/renderer/ShaderReflection.cpp"
    "src/renderer/Shader.h"
    "src/renderer/Shader.cpp"
    "src/renderer/Framework.h"
    "src/renderer/Framework.cpp"
    "src/renderer/Window.h"
    "src/renderer/Window.cpp")

set(src_files
    "src/App.h"
    "src/App.cpp"
    ${src_precompiled}
    ${src_util}
    ${src_module}
    ${src_renderer})

# Project
add_library(${target_name} ${src_files})

source_group("src" FILES "src/main.cpp")

target_precompile_headers(${target_name} PUBLIC ${src_precompiled})

target_link_libraries(${target_name} d3d11 d3dcompiler CONAN_PKG::glfw CONAN_PKG::stb CONAN_PKG::imgui CONAN_PKG::glm)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(${target_name} PROPERTIES FOLDER ${PROJECT_NAME})
set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# Tests
add_executable(${target_name}_test "test/test.cpp")

target_link_libraries(${target_name}_test CONAN_PKG::gtest)

set_target_properties(${target_name}_test PROPERTIES FOLDER ${PROJECT_NAME})

gtest_discover_tests(${target_name}_test)