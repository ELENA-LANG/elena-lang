project(asm2binx CXX)

################################################################################
# Source groups
################################################################################
set(Headers
    "../../../common/altstrings.h"
    "../../../common/common.h"
    "../../../common/config.h"
    "../../../common/dump.h"
    "../../../common/files.h"
    "../../../common/lists.h"
    "../../../common/streams.h"
    "../../../common/tools.h"
    "../../../common/xmlreader.h"
    "../../../elc/source.h"
    "../../../engine/amd64helper.h"
    "../../../engine/bytecode.h"
    "../../../engine/elena.h"
    "../../../engine/elenaconst.h"
    "../../../engine/module.h"
    "../../../engine/section.h"
    "../../../engine/x86helper.h"
    "../amd64assembler.h"
    "../assembler.h"
    "../assemblerException.h"
    "../ecassembler.h"
    "../preprocessor.h"
    "../preProcessorException.h"
    "../x86assembler.h"
    "../x86jumphelper.h"
    "../vs/resource.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "../../../common/altstrings.cpp"
    "../../../common/config.cpp"
    "../../../common/dump.cpp"
    "../../../common/files.cpp"
    "../../../common/xmlreader.cpp"
    "../../../elc/source.cpp"
    "../../../engine/amd64helper.cpp"
    "../../../engine/bytecode.cpp"
    "../../../engine/module.cpp"
    "../../../engine/section.cpp"
    "../../../engine/x86helper.cpp"
    "../amd64assembler.cpp"
    "../asm2binx.cpp"
    "../ecassembler.cpp"
    "../preprocessor.cpp"
    "../x86assembler.cpp"
    "../x86jumphelper.cpp"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${Headers}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "asm2binx"
    TARGET_NAME_RELEASE "asm2binx"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PDB_OUTPUT_DIRECTORY_DEBUG   "${OUTPUT_DIRECTORY}"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../common;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../elc;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../engine;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../common/win32"
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
    "_CONSOLE;"
    "_CRT_SECURE_NO_WARNINGS;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /RTC1;
            /MTd;
            /ZI
        >
        $<$<CONFIG:Release>:
            /MT;
            /Zi
        >
        /W3;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
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
        /SUBSYSTEM:CONSOLE;
        /DYNAMICBASE:NO
    )
endif()

