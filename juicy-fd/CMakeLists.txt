set(target_name "juicy-fd")

set(src_files
    "src/juicy-fd/FileDialog.h"
    "src/juicy-fd/FileDialog.cpp")

add_library(${target_name} STATIC ${src_files})

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(${target_name} PROPERTIES FOLDER ${PROJECT_NAME})