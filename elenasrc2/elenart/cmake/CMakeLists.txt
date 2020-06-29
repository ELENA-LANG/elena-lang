project(elenart15 CXX)

################################################################################
# Source groups
################################################################################
set(Headers
    "../../common/altstrings.h"
    "../../common/common.h"
    "../../common/config.h"
    "../../common/dump.h"
    "../../common/files.h"
    "../../common/lists.h"
    "../../common/streams.h"
    "../../common/tools.h"
    "../../common/xmlreader.h"
    "../../engine/bytecode.h"
    "../../engine/libman.h"
    "../../engine/module.h"
    "../../engine/rtman.h"
    "../../engine/section.h"
    "../../engine/win32/pehelper.h"
    "../elenartmachine.h"
    "../vs/resource.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "../../common/altstrings.cpp"
    "../../common/config.cpp"
    "../../common/dump.cpp"
    "../../common/files.cpp"
    "../../common/xmlreader.cpp"
    "../../engine/bytecode.cpp"
    "../../engine/elenamachine.cpp"
    "../../engine/libman.cpp"
    "../../engine/module.cpp"
    "../../engine/rtman.cpp"
    "../../engine/section.cpp"
    "../../engine/win32/pehelper.cpp"
    "../../engine/win32/win32routines.cpp"
    "../../engine/x86/x86routines.cpp"
    "../elenartmachine.cpp"
    "../win32/dllmain.cpp"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${Headers}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE elenart)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "elenart"
    TARGET_NAME_RELEASE "elenart"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/..;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../win32;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../common;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../engine;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../engine/win32"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "WIN32;"
    "_WINDOWS;"
    "_USRDLL;"
    "EVM_EXPORTS;"
    "UNICODE;"
    "_WIN32;"
    "_UNICODE;"
    "_CRT_SECURE_NO_WARNINGS"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /RTCu;
            /MDd;
            /ZI
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /MD;
            /Gy;
            /Zi
        >
        /W3;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
    source_file_compile_options(../win32/dllmain.cpp ${FILE_CL_OPTIONS})
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /DEBUG;
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /MANIFEST:NO;
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /MACHINE:X86;
        /SUBSYSTEM:WINDOWS
    )
endif()

