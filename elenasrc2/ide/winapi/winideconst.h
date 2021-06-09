//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideconstH
#define winideconstH

#include "elenaconst.h"

#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

#define IDE_REVISION_NUMBER                     39

// --- Command line arguments ---

// --- Menu Command Base ---
#define IDM                                     40000             // Menu constants

// --- Resources ---
#define IDR_SEPARATOR                           0

#define	IDI_APP_ICON				               100

#define	IDR_FILENEW                            201
#define	IDR_FILEOPEN                           202
#define	IDR_FILESAVE                           203
#define	IDR_SAVEALL                            204
#define	IDR_CLOSEFILE                          205
#define	IDR_CLOSEALL                           206
#define	IDR_CUT                                207
#define	IDR_COPY                               208
#define	IDR_PASTE                              209
#define	IDR_UNDO                               210
#define	IDR_REDO                               211
#define	IDR_RUN                                212
#define	IDR_STOP                               213
#define	IDR_STEPINTO						         214
#define	IDR_STEPOVER                           215
#define	IDR_GOTO                               216
#define IDR_FILETREE						   217

#define	IDR_IDE_ACCELERATORS		               1400
#define	IDR_MAIN_MENU			                  1500

#define	IDM_FILE                               (IDM + 1000)
   #define  IDM_FILE_NEW                        (IDM_FILE + 1)
   #define	IDM_FILE_OPEN                       (IDM_FILE + 3)
   #define  IDM_PROJECT_NEW                     (IDM_FILE + 2)
   #define	IDM_PROJECT_OPEN                    (IDM_FILE + 4)
   #define	IDM_FILE_SAVE                       (IDM_FILE + 5)
   #define	IDM_FILE_SAVEAS                     (IDM_FILE + 6)
   #define	IDM_FILE_SAVEPROJECT                (IDM_FILE + 7)
   #define	IDM_FILE_SAVEALL                    (IDM_FILE + 8)
   #define	IDM_FILE_EXIT                       (IDM_FILE + 9)
   #define  IDM_FILE_CLOSE                      (IDM_FILE + 10)
   #define  IDM_FILE_CLOSEALL                   (IDM_FILE + 11)
   #define  IDM_FILE_CLOSEALLBUT                (IDM_FILE + 12)
   #define  IDM_PROJECT_CLOSE                   (IDM_FILE + 13)
   #define  IDM_FILE_FILES                      (IDM_FILE + 20)
   #define  IDM_FILE_FILES_1                    (IDM_FILE + 21)
   #define  IDM_FILE_FILES_2                    (IDM_FILE + 22)
   #define  IDM_FILE_FILES_3                    (IDM_FILE + 23)
   #define  IDM_FILE_FILES_4                    (IDM_FILE + 24)
   #define  IDM_FILE_FILES_5                    (IDM_FILE + 25)
   #define  IDM_FILE_FILES_6                    (IDM_FILE + 26)
   #define  IDM_FILE_FILES_7                    (IDM_FILE + 27)
   #define  IDM_FILE_FILES_8                    (IDM_FILE + 28)
   #define  IDM_FILE_FILES_9                    (IDM_FILE + 29)
   #define  IDM_FILE_FILES_10                   (IDM_FILE + 30)
   #define  IDM_FILE_FILES_CLEAR                (IDM_FILE + 31)
   #define  IDM_FILE_PROJECTS                   (IDM_FILE + 40)
   #define  IDM_FILE_PROJECTS_1                 (IDM_FILE + 41)
   #define  IDM_FILE_PROJECTS_2                 (IDM_FILE + 42)
   #define  IDM_FILE_PROJECTS_3                 (IDM_FILE + 43)
   #define  IDM_FILE_PROJECTS_4                 (IDM_FILE + 44)
   #define  IDM_FILE_PROJECTS_5                 (IDM_FILE + 45)
   #define  IDM_FILE_PROJECTS_6                 (IDM_FILE + 46)
   #define  IDM_FILE_PROJECTS_7                 (IDM_FILE + 47)
   #define  IDM_FILE_PROJECTS_8                 (IDM_FILE + 48)
   #define  IDM_FILE_PROJECTS_9                 (IDM_FILE + 49)
   #define  IDM_FILE_PROJECTS_10                (IDM_FILE + 50)
   #define  IDM_FILE_PROJECTS_CLEAR             (IDM_FILE + 51)

#define	IDM_EDIT                               (IDM + 2000)
   #define	IDM_EDIT_UNDO                       (IDM_EDIT + 1)
   #define	IDM_EDIT_REDO                       (IDM_EDIT + 2)
   #define	IDM_EDIT_COPY                       (IDM_EDIT + 3)
   #define	IDM_EDIT_PASTE                      (IDM_EDIT + 4)
   #define	IDM_EDIT_CUT                        (IDM_EDIT + 5)
   #define	IDM_EDIT_DELETE                     (IDM_EDIT + 6)
   #define  IDM_EDIT_SELECTALL				      (IDM_EDIT + 7)
   #define  IDM_EDIT_TRIM                       (IDM_EDIT + 8)
   #define  IDM_EDIT_ERASELINE                  (IDM_EDIT + 9)
   #define  IDM_EDIT_DUPLICATE                  (IDM_EDIT + 10)
   #define  IDM_EDIT_COMMENT                    (IDM_EDIT + 11)
   #define  IDM_EDIT_UNCOMMENT                  (IDM_EDIT + 12)
   #define  IDM_EDIT_INDENT                     (IDM_EDIT + 13)
   #define  IDM_EDIT_OUTDENT                    (IDM_EDIT + 14)
   #define  IDM_EDIT_SWAP                       (IDM_EDIT + 15)
   #define  IDM_EDIT_UPPERCASE                  (IDM_EDIT + 16)
   #define  IDM_EDIT_LOWERCASE                  (IDM_EDIT + 17)

#define IDM_VIEW                                (IDM + 3000)
   #define  IDM_VIEW_OUTPUT                     (IDM_VIEW + 1)
   #define  IDM_VIEW_WATCH                      (IDM_VIEW + 2)
   #define  IDM_VIEW_CALLSTACK                  (IDM_VIEW + 3)
   #define  IDM_VIEW_MESSAGES                   (IDM_VIEW + 4)   
   #define  IDM_VIEW_PROJECTVIEW                (IDM_VIEW + 5)
   #define  IDM_VIEW_VMCONSOLE                  (IDM_VIEW + 6)

#define	IDM_SEARCH                             (IDM + 4000)
   #define  IDM_SEARCH_FIND                     (IDM_SEARCH + 1)
   #define  IDM_SEARCH_FINDNEXT                 (IDM_SEARCH + 2)
   #define	IDM_SEARCH_GOTOLINE                 (IDM_SEARCH + 3)
   #define	IDM_SEARCH_REPLACE                  (IDM_SEARCH + 4)

#define IDM_PROJECT                             (IDM + 5000)
   #define IDM_PROJECT_INCLUDE                  (IDM_PROJECT + 1)
   #define IDM_PROJECT_EXCLUDE                  (IDM_PROJECT + 2)
   #define IDM_PROJECT_COMPILE                  (IDM_PROJECT + 3)
   #define IDM_PROJECT_FORWARDS                 (IDM_PROJECT + 4)
   #define IDM_PROJECT_OPTION                   (IDM_PROJECT + 5)
   #define IDM_PROJECT_CLEAN                    (IDM_PROJECT + 6)

#define IDM_DEBUG                               (IDM + 6000)
   #define IDM_DEBUG_RUN                        (IDM_DEBUG + 1)
   #define IDM_DEBUG_STEPOVER                   (IDM_DEBUG + 2)
   #define IDM_DEBUG_STEPINTO                   (IDM_DEBUG + 3)
   #define IDM_DEBUG_RUNTO                      (IDM_DEBUG + 4)
   #define IDM_DEBUG_BREAKPOINT                 (IDM_DEBUG + 5)
   #define IDM_DEBUG_CLEARBREAKPOINT            (IDM_DEBUG + 6)
   #define IDM_DEBUG_STOP                       (IDM_DEBUG + 7)
   #define IDM_DEBUG_INSPECT                    (IDM_DEBUG + 8)
   #define IDM_DEBUG_SWITCHHEXVIEW              (IDM_DEBUG + 9)
   #define IDM_DEBUG_GOTOSOURCE                 (IDM_DEBUG + 10)

#define IDM_TOOLS                               (IDM + 7000)
   #define IDM_EDITOR_OPTIONS                   (IDM_TOOLS + 1)
   #define IDM_DEBUGGER_OPTIONS                 (IDM_TOOLS + 2)

#define	IDM_WINDOW                             (IDM + 8000)
   #define IDM_WINDOW_NEXT                      (IDM_WINDOW + 1)
   #define IDM_WINDOW_PREVIOUS                  (IDM_WINDOW + 2)
   #define IDM_WINDOW_WINDOWS                   (IDM_WINDOW + 4)
   #define IDM_WINDOW_FIRST                     (IDM_WINDOW + 5)
   #define IDM_WINDOW_SECOND                    (IDM_WINDOW + 6)
   #define IDM_WINDOW_THIRD                     (IDM_WINDOW + 7)
   #define IDM_WINDOW_FOURTH                    (IDM_WINDOW + 8)
   #define IDM_WINDOW_FIFTH                     (IDM_WINDOW + 9)
   #define IDM_WINDOW_SIXTH                     (IDM_WINDOW + 10)
   #define IDM_WINDOW_SEVENTH                   (IDM_WINDOW + 11)
   #define IDM_WINDOW_EIGHTH                    (IDM_WINDOW + 12)
   #define IDM_WINDOW_NINTH                     (IDM_WINDOW + 13)
   #define IDM_WINDOW_TENTH                     (IDM_WINDOW + 14)

#define IDM_HELP                                (IDM + 9000)
   #define IDM_HELP_API                         (IDM_HELP + 1)
   #define IDM_HELP_ABOUT                       (IDM_HELP + 2)

#define IDM_WATCH                               (IDM + 10000)
   #define IDM_WATCH_OPEN                       (IDM_WATCH + 1)

// --- Notification messages ---
#define IDE_DEBUGGER_STEP                       200
#define IDE_DEBUGGER_START                      201
#define IDE_DEBUGGER_STOP                       202
#define IDE_DEBUGGER_LOADMODULE                 203
#define IDE_DEBUGGER_CHECKPOINT                 204
#define IDM_DEBUGGER_EXCEPTION                  205
#define IDE_DEBUGGER_BREAK					         206
#define IDE_DEBUGGER_HOOK                       207
#define IDE_DEBUGGER_LOADTEMPMODULE             208
#define IDM_COMPILER_SUCCESSFUL                 300
#define IDM_COMPILER_UNSUCCESSFUL               301
#define IDM_COMPILER_WITHWARNING			         302
#define IDM_LAYOUT_CHANGED					         303

// dialogs

#define IDD_SETTINGS                            500
#define IDC_SETTINGS_LABEL2                    (IDD_SETTINGS + 1)
#define IDC_SETTINGS_LABEL3                    (IDD_SETTINGS + 2)
#define IDC_SETTINGS_TARGET                    (IDD_SETTINGS + 3)
#define IDC_SETTINGS_PACKAGE                   (IDD_SETTINGS + 7)
#define IDC_SETTINGS_OUTPUT                    (IDD_SETTINGS + 8)
#define IDC_SETTINGS_DEBUG                     (IDD_SETTINGS + 9)
#define IDC_SETTINGS_TEPMPLATE                 (IDD_SETTINGS + 12)
#define IDC_SETTINGS_ARGUMENT                  (IDD_SETTINGS + 13)
#define IDC_SETTINGS_OPTIONS                   (IDD_SETTINGS + 14)

#define IDD_GOTOLINE                            600
#define	IDC_GOTOLINE_LINENUMBER                (IDD_GOTOLINE + 3)
#define	IDC_GOTOLINE_LABEL1                    (IDD_GOTOLINE + 4)

#define IDD_FORWARDS                            700
#define IDC_FORWARDS_LIST                       (IDD_FORWARDS + 1)
#define IDC_FORWARDS_EDIT                       (IDD_FORWARDS + 2)
#define IDC_FORWARDS_ADD                        (IDD_FORWARDS + 3)
#define IDC_FORWARDS_REPLACE                    (IDD_FORWARDS + 4)
#define IDC_FORWARDS_DELETE                     (IDD_FORWARDS + 5)
#define IDC_FORWARDS_LABEL1                     (IDD_FORWARDS + 8)
#define IDC_FORWARDS_SAVE                       (IDD_FORWARDS + 9)

#define IDD_WINDOWS                             800
#define IDC_WINDOWS_LIST                        (IDD_WINDOWS + 1)
#define IDC_WINDOWS_CLOSE                       (IDD_WINDOWS + 2)

#define IDD_EDITOR_SETTINGS                     900
#define IDC_EDITOR_LINENUMBERFLAG               (IDD_EDITOR_SETTINGS + 1)
#define IDC_EDITOR_COLORSCHEME                  (IDD_EDITOR_SETTINGS + 2)
#define IDC_EDITOR_FONTSIZE                     (IDD_EDITOR_SETTINGS + 3)
#define IDC_EDITOR_USETAB                       (IDD_EDITOR_SETTINGS + 4)
#define IDC_EDITOR_TABSIZE                      (IDD_EDITOR_SETTINGS + 5)
#define IDC_EDITOR_HIGHLIGHSYNTAXFLAG           (IDD_EDITOR_SETTINGS + 6)
#define IDC_EDITOR_UNICODEFILES                 (IDD_EDITOR_SETTINGS + 7)
#define IDC_EDITOR_REMEMBERPATH                 (IDD_EDITOR_SETTINGS + 8)
#define IDC_EDITOR_REMEMBERPROJECT              (IDD_EDITOR_SETTINGS + 9)
#define IDC_EDITOR_ENCODING                     (IDD_EDITOR_SETTINGS + 10)

#define IDD_EDITOR_FIND                         1000
#define IDC_FIND_TEXT                           (IDD_EDITOR_FIND + 4)
#define IDC_FIND_CASE                           (IDD_EDITOR_FIND + 5)
#define IDC_FIND_WHOLE                          (IDD_EDITOR_FIND + 6)
#define IDC_REPLACE_TEXT                        (IDD_EDITOR_FIND + 7)

#define IDD_EDITOR_REPLACE                      1100

#define IDD_ABOUT                               1200
#define IDC_ABOUT_HOME                          (IDD_ABOUT + 1)
#define IDC_ABOUT_LICENCE_TEXT                  (IDD_ABOUT + 2)
#define IDC_ABOUT_BLOG                          (IDD_ABOUT + 3)

#define IDD_DEBUGGER_SETTINGS                   1300
#define IDC_DEBUGGER_SRCPATH                    (IDD_DEBUGGER_SETTINGS + 1)
#define IDC_DEBUGGER_LIBPATH                    (IDD_DEBUGGER_SETTINGS + 2)

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
 
#define VER_FILE_VERSION_STR        STRINGIZE(ENGINE_MAJOR_VERSION)        \
                                    "." STRINGIZE(ENGINE_MINOR_VERSION)    \
                                    "." STRINGIZE(IDE_REVISION_NUMBER)

#define VER_PRODUCT_VERSION_STR     STRINGIZE(ENGINE_MAJOR_VERSION)        \
                                    "." STRINGIZE(ENGINE_MINOR_VERSION)    \
                                    ".0"

#endif // winideconstH
