project(elenasm15 CXX)

################################################################################
# Source groups
################################################################################
set(Headers
    "../../common/altstrings.h"
    "../../common/common.h"
    "../../common/dump.h"
    "../../common/files.h"
    "../../common/lists.h"
    "../../common/streams.h"
    "../../common/tools.h"
    "../../engine/bytecode.h"
    "../../engine/elena.h"
    "../../engine/elenaconst.h"
    "../../engine/parsertable.h"
    "../../engine/syntaxtree.h"
    "../../engine/textsource.h"
    "../cfparser.h"
    "../inlineparser.h"
    "../scriptengine.h"
    "../session.h"
    "../transformer.h"
    "../treeparser.h"
    "../vs/resource.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "../../common/altstrings.cpp"
    "../../common/dump.cpp"
    "../../common/files.cpp"
    "../../engine/bytecode.cpp"
    "../../engine/parsertable.cpp"
    "../../engine/syntaxtree.cpp"
    "../../engine/textsource.cpp"
    "../cfparser.cpp"
    "../inlineparser.cpp"
    "../session.cpp"
    "../transformer.cpp"
    "../treeparser.cpp"
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
set(ROOT_NAMESPACE escript)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "elenasm"
    TARGET_NAME_RELEASE "elenasm"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_NAME_DEBUG   "elenasm"
    ARCHIVE_OUTPUT_NAME_RELEASE "elenasm"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../common;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../engine;"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
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
    "_CRT_SECURE_NO_WARNINGS;"
    "_WIN32;"
    "_UNICODE"
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
            /W4;
            /ZI
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /MD;
            /Gy;
            /W3;
            /Zi
        >
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
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /MACHINE:X86;
        /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Dependencies
################################################################################
target_link_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
    ">"
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/"
    ">"
)

