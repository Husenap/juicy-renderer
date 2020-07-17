set(target_name "app")

set(src_files
    "src/main.cpp")

# Project
add_executable(${target_name} ${src_files} "resources/resources.rc")

source_group("src" FILES "src/main.cpp")

target_link_libraries(${target_name} "juicy-renderer")

set_target_properties(${target_name} PROPERTIES FOLDER ${PROJECT_NAME})
set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set_target_properties(${target_name} PROPERTIES LINK_FLAGS "/ignore:4099")