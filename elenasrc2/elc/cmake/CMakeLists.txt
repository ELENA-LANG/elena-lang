project(elc CXX)

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
    "../../engine/amd64jitcompiler.h"
    "../../engine/bytecode.h"
    "../../engine/compilercommon.h"
    "../../engine/compilerlogic.h"
    "../../engine/elena.h"
    "../../engine/elenaconst.h"
    "../../engine/jitcompiler.h"
    "../../engine/jitlinker.h"
    "../../engine/libman.h"
    "../../engine/loader.h"
    "../../engine/module.h"
    "../../engine/parsertable.h"
    "../../engine/section.h"
    "../../engine/syntaxtree.h"
    "../../engine/x86helper.h"
    "../../engine/x86jitcompiler.h"
    "../bcwriter.h"
    "../compiler.h"
    "../compilerscope.h"
    "../derivation.h"
    "../elc.h"
    "../errors.h"
    "../image.h"
    "../parser.h"
    "../project.h"
    "../separser.h"
    "../source.h"
    "../win32/linker.h"
    "../win32/syslibloader.h"
    "../vs/resource.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "../../common/altstrings.cpp"
    "../../common/config.cpp"
    "../../common/dump.cpp"
    "../../common/files.cpp"
    "../../common/xmlreader.cpp"
    "../../engine/amd64jitcompiler.cpp"
    "../../engine/bytecode.cpp"
    "../../engine/compilerlogic.cpp"
    "../../engine/jitcompiler.cpp"
    "../../engine/jitlinker.cpp"
    "../../engine/libman.cpp"
    "../../engine/loader.cpp"
    "../../engine/module.cpp"
    "../../engine/parsertable.cpp"
    "../../engine/section.cpp"
    "../../engine/syntaxtree.cpp"
    "../../engine/x86helper.cpp"
    "../../engine/x86jitcompiler.cpp"
    "../bcwriter.cpp"
    "../compiler.cpp"
    "../compilerscope.cpp"
    "../derivation.cpp"
    "../image.cpp"
    "../parser.cpp"
    "../project.cpp"
    "../separser.cpp"
    "../source.cpp"
    "../win32/elc.cpp"
    "../win32/linker.cpp"
    "../win32/syslibloader.cpp"
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
    TARGET_NAME_DEBUG   "elc"
    TARGET_NAME_RELEASE "elc"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PDB_OUTPUT_DIRECTORY_DEBUG   "${OUTPUT_DIRECTORY}"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/..;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../common;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../engine"
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
    "_CONSOLE;"
    "UNICODE;"
    "_UNICODE;"
    "_WIN32;"
    "_CRT_SECURE_NO_WARNINGS"
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
            /W4;
            /ZI
        >
        $<$<CONFIG:Release>:
            /MT;
            /W3;
            /Zi
        >
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /DEBUG
        >
        $<$<CONFIG:Release>:
            /MANIFEST:NO;
            /OPT:REF;
            /OPT:ICF
        >
        /MACHINE:X86;
        /SUBSYSTEM:CONSOLE;
        /DYNAMICBASE:NO;
        /INCREMENTAL:NO
    )
endif()

