set(target_name "juicy-renderer")

set(src_renderer
    "src/renderer/Shader.h"
    "src/renderer/Shader.cpp"
    "src/renderer/Renderer.h"
    "src/renderer/Renderer.cpp"
    "src/renderer/Window.h"
    "src/renderer/Window.cpp")

set(src_precompiled
    "src/precompiled/pch.h")

set(src_files
    "src/App.h"
    "src/App.cpp"
    ${src_precompiled}
    ${src_renderer})

# Project
add_library(${target_name} ${src_files})

source_group("src" FILES "src/main.cpp")

target_precompile_headers(${target_name} PUBLIC ${src_precompiled})

target_link_libraries(${target_name} d3d11 d3dcompiler CONAN_PKG::glfw )

target_include_directories(${target_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(${target_name} PROPERTIES FOLDER ${PROJECT_NAME})
set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# Tests
add_executable(${target_name}_test "test/test.cpp")

target_link_libraries(${target_name}_test CONAN_PKG::gtest)

set_target_properties(${target_name}_test PROPERTIES FOLDER ${PROJECT_NAME})

gtest_discover_tests(${target_name}_test)