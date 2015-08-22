//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE Messages
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef messagesH
#define messagesH

// --- Common constants ---
#define APP_NAME                                _T("ELENA IDE 1.9.23")
#define EDITOR_NAME                             _T("ELENA IDE Editor")

// --- UI Messages ---

#define OUTPUT_TAB                              _T("Output")
#define MESSAGES_TAB                            _T("Messages")
#define WATCH_TAB                               _T("Auto Watch")
#define CALLSTACK_TAB                           _T("Call stack")

#define EDITOR_MODIFIED                         _T("Modified")
#define EDITOR_READY                            _T("Ready")

#define OPEN_FILE_CAPTION                       _T("Open File")
#define OPEN_PROJECT_CAPTION                    _T("Open Project")
#define SAVEAS_PROJECT_CAPTION                  _T("Save Project As")
#define SAVEAS_FILE_CAPTION                     _T("Save File As")
#define QUESTION_SAVE_CHANGES                   _T("Save changes to the project?")
#define QUESTION_INCLUDE_FILE1                  _T("Include the document ")
#define QUESTION_INCLUDE_FILE2                  _T(" into the project?")

// --- IDE message constants ---
#define IDE_MSG_INVALID_PROJECT                 _T("Invalid project file ")

#define ERROR_MOUDLE_NOT_FOUND                  _T("Could not locate the module ")
#define ERROR_COULD_NOT_START                   _T("Could not start the compilation process")
#define ERROR_CANNOT_OPEN_FILE                  _T("Cannot open file ")
#define ERROR_RUN_OUT_OF_DATE                   _T("The project modules are out of date\nPlease recompile the project")
#define ERROR_RUN_NEED_TARGET                   _T("A project with not specified target cannot be started")
#define ERROR_DEBUG_FILE_NOT_FOUND_SETTING      _T("A debugger cannot be started. Invalid or absent debug info\nPlease turn on debug info option and recompile the project")
#define ERROR_DEBUG_FILE_NOT_FOUND_COMPILE      _T("A debugger cannot be started. Invalid or absent debug info\nPlease compile the project")
#define ERROR_RUN_NEED_RECOMPILE                _T("A program cannot be started\nPlease re-compile the project")
#define ERROR_CANCELED                          _T("The operation was canceled")

#define QUESTION_SAVE_FILECHANGES               _T("Save changes to ")
#define ERROR_CANNOT_CLOSE_COMPILING            _T("The project is compiling. Close anyway?")

#define NOT_FOUND_TEXT                          _T("Search string not found")
#define REPLACE_TEXT                            _T("Replace this occurence?")

#define PROJECT_COMPILING                       _T("Compiling the project...")
#define SUCCESSFULLY_COMPILED                   _T("Successfully compiled")
#define COMPILED_WITH_ERRORS                    _T("Compiled with errors")
#define COMPILED_WITH_WARNINGS                  _T("Successfully compiled with warnings")

#define PROGRAM_BROKEN                          _T(" Program broken")
#define PROGRAM_STOPPED                         _T(" Program stopped")

// --- Context Menu ---

#define CONTEXT_MENU_CLOSE                      _T("Close\tCtrl+W")
#define CONTEXT_MENU_CUT                        _T("Cut\tCtrl+X")
#define CONTEXT_MENU_COPY                       _T("Copy\tCtrl+C")
#define CONTEXT_MENU_PASTE                      _T("Paste\tCtrl+V")
#define CONTEXT_MENU_TOGGLE                     _T("Toggle Breakpoint\tF5")
#define CONTEXT_MENU_RUNTO                      _T("Run to Cursor\tF4")
#define CONTEXT_MENU_INSPECT                    _T("Inspect\tCtrl+I")
#define CONTEXT_MENU_SHOWHEX                    _T("Show as hexadecimal")

// --- Hints ---

#define HINT_NEW_FILE                           _T("New File")
#define HINT_OPEN_FILE                          _T("Open File")
#define HINT_SAVE_FILE                          _T("Save File")
#define HINT_SAVEALL                            _T("Save All")
#define HINT_CLOSE_FILE                         _T("Close File")
#define HINT_CLOSE_PROJECT                      _T("Close Project")
#define HINT_CUT                                _T("Cut")
#define HINT_COPY                               _T("Copy")
#define HINT_PASTE                              _T("Paste")
#define HINT_UNDO                               _T("Undo")
#define HINT_REDO                               _T("Redo")
#define HINT_RUN                                _T("Run")
#define HINT_STOP                               _T("Stop")
#define HINT_STEPINTO                           _T("Step Into")
#define HINT_STEPOVER                           _T("Step Over")
#define HINT_GOTOSOURCE                         _T("Go to Source")

// --- Debugger exception texts ---

#define UNKNOWN_EXCEPTION_TEXT                  _T("Unknown exception at address ")
#define ACCESS_VIOLATION_EXCEPTION_TEXT         _T("Access violation at address ")
#define ARRAY_BOUNDS_EXCEEDED_EXCEPTION_TEXT    _T("Array bounds exceeded at address ")
#define DATATYPE_MISALIGNMENT_EXCEPTION_TEXT    _T("Datatype misalignment at address ")
#define FLT_DENORMAL_OPERAND_EXCEPTION_TEXT     _T("Floating-point denormal operand at address ")
#define FLT_DIVIDE_BY_ZERO_EXCEPTION_TEXT       _T("Floating-point divide by zero at address ")
#define FLT_INEXACT_RESULT_EXCEPTION_TEXT       _T("Floating-point inexact result at address ")
#define FLT_INVALID_OPERATION_EXCEPTION_TEXT    _T("Floating-point invalid operation at address ")
#define FLT_OVERFLOW_EXCEPTION_TEXT             _T("Floating-point overflow at address ")
#define FLT_STACK_CHECK_EXCEPTION_TEXT          _T("Floating-point stack check at address ")
#define FLT_UNDERFLOW_EXCEPTION_TEXT            _T("Floating-point underflow at address ")
#define ILLEGAL_INSTRUCTION_EXCEPTION_TEXT      _T("Illegal instruction at address ")
#define PAGE_ERROR_EXCEPTION_TEXT               _T("Page error at address ")
#define INT_DIVIDE_BY_ZERO_EXCEPTION_TEXT       _T("Division by zero at address ")
#define INT_OVERFLOW_EXCEPTION_TEXT             _T("Range overflow at address ")
#define INVALID_DISPOSITION_EXCEPTION_TEXT      _T("Invalid disposition at address ")
#define NONCONTINUABLE_EXCEPTION_EXCEPTION_TEXT _T("Non continuable exception at address ")
#define PRIV_INSTRUCTION_EXCEPTION_TEXT         _T("Private instruction at address ")
#define STACK_OVERFLOW_EXCEPTION_TEXT           _T("Stack overflow at address ")
#define GC_OUTOF_MEMORY_EXCEPTION_TEXT          _T("Out of memory at address ")

// --- About dialog ---
#define APACHE_LICENSE2                         _T("The Apache License V2.0\r\n\r\nCopyright (C) 2005-2015 Alex Rakov\r\n\r\nLicensed under the Apache License, Version 2.0 (the \"License\"); you may not use this file except in compliance with the License. You may obtain a copy of the License at:\r\n\r\nhttp://www.apache.org/licenses/LICENSE-2.0\r\n\r\nUnless required by applicable law or agreed to in writing, software distributed under the License is distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.\r\n\r\nSee the license details in the file doc\\license.txt")
#define ELENA_HOMEPAGE _T("http://elenalang.sourceforge.net/")
#define ELENA_BLOG     _T("http://elenalang.blogspot.com/")

#endif // messages
