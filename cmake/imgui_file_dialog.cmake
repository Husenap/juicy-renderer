message("-- External Project: ImGuiFileDialog")
include(FetchContent)

FetchContent_Declare(
    imgui_file_dialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG v0.4
)

FetchContent_GetProperties(imgui_file_dialog)
if(NOT imgui_file_dialog_POPULATED)
    FetchContent_Populate(imgui_file_dialog)

    set(target_name "imgui_file_dialog")

    set(src_imgui_file_dialog
        "${imgui_file_dialog_SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialogConfig.h"
        "${imgui_file_dialog_SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.h"
        "${imgui_file_dialog_SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.cpp")

    add_library(${target_name} STATIC ${src_imgui_file_dialog})

    target_include_directories(${target_name} PUBLIC ${imgui_file_dialog_SOURCE_DIR})

    set_target_properties(${target_name} PROPERTIES FOLDER "thirdparty")
endif()