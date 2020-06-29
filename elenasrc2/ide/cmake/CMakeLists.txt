project(elide CXX)

################################################################################
# Source groups
################################################################################
set(Headers
    "../../common/altstrings.h"
    "../../common/common.h"
    "../../common/config.h"
    "../../common/dump.h"
    "../../common/files.h"
    "../../common/xmlreader.h"
    "../../engine/bytecode.h"
    "../../engine/module.h"
    "../../engine/rtman.h"
    "../../engine/section.h"
    "../../engine/win32/pehelper.h"
    "../../gui/document.h"
    "../../gui/text.h"
    "../../gui/winapi32/wincommon.h"
    "../../gui/winapi32/wingraphic.h"
    "../../gui/winapi32/winlistview.h"
    "../../gui/winapi32/winmenu.h"
    "../../gui/winapi32/winredirect.h"
    "../../gui/winapi32/winsdi.h"
    "../../gui/winapi32/winsplitter.h"
    "../../gui/winapi32/winstatusbar.h"
    "../../gui/winapi32/wintabbar.h"
    "../../gui/winapi32/wintextview.h"
    "../../gui/winapi32/wintoolbar.h"
    "../../gui/winapi32/wintreeview.h"
    "../appwindow.h"
    "../browser.h"
    "../debugcontroller.h"
    "../debugging.h"
    "../historylist.h"
    "../idecommon.h"
    "../menulist.h"
    "../settings.h"
    "../sourcedoc.h"
    "../winapi32/debugger.h"
    "../winapi32/windialogs.h"
    "../winapi32/wineditframe.h"
    "../winapi32/winide.h"
    "../winapi32/winideconst.h"
    "../winapi32/winoutput.h"
    "../winapi32/ide.rc"
    "../windowlist.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "../../common/altstrings.cpp"
    "../../common/config.cpp"
    "../../common/dump.cpp"
    "../../common/files.cpp"
    "../../common/xmlreader.cpp"
    "../../engine/bytecode.cpp"
    "../../engine/module.cpp"
    "../../engine/rtman.cpp"
    "../../engine/section.cpp"
    "../../engine/win32/pehelper.cpp"
    "../../gui/document.cpp"
    "../../gui/text.cpp"
    "../../gui/winapi32/wincommon.cpp"
    "../../gui/winapi32/wingraphic.cpp"
    "../../gui/winapi32/winlistview.cpp"
    "../../gui/winapi32/winmenu.cpp"
    "../../gui/winapi32/winredirect.cpp"
    "../../gui/winapi32/winsdi.cpp"
    "../../gui/winapi32/winsplitter.cpp"
    "../../gui/winapi32/winstatusbar.cpp"
    "../../gui/winapi32/wintabbar.cpp"
    "../../gui/winapi32/wintextview.cpp"
    "../../gui/winapi32/wintoolbar.cpp"
    "../../gui/winapi32/wintreeview.cpp"
    "../appwindow.cpp"
    "../browser.cpp"
    "../debugcontroller.cpp"
    "../historylist.cpp"
    "../menulist.cpp"
    "../settings.cpp"
    "../sourcedoc.cpp"
    "../winapi32/debugger.cpp"
    "../winapi32/main.cpp"
    "../winapi32/windialogs.cpp"
    "../winapi32/wineditframe.cpp"
    "../winapi32/winide.cpp"
    "../winapi32/winoutput.cpp"
    "../windowlist.cpp"
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
    TARGET_NAME_DEBUG   "elide"
    TARGET_NAME_RELEASE "elide"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/../../gui;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../common;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../engine"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Release>:"
        "WIN32;"
        "NDEBUG;"
        "_WINDOWS"
    ">"
    "_WIN32;"
    "UNICODE;"
    "_CRT_SECURE_NO_WARNINGS;"
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
        /SUBSYSTEM:WINDOWS;
        /DYNAMICBASE:NO;
        comctl32.lib;
        shlwapi.lib
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Release>:"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "advapi32;"
        "shell32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32"
    ">"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

