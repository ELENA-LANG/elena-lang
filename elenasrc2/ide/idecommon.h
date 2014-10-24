//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE common
//
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef idecommonH
#define idecommonH

#include "common.h"

namespace _GUI_
{

// --- Project settings ---
#define IDE_FILES_SECTION                       "files"
#define IDE_FORWARDS_SECTION                    "forwards"
#define IDE_PROJECT_SECTION                     "project"
#define IDE_PACKAGE_SETTING                     "namespace"
#define IDE_TEMPLATE_SETTING                    "template"
#define IDE_COMPILER_OPTIONS                    "options"
#define IDE_EXECUTABLE_SETTING                  "executable"
#define IDE_OUTPUT_SETTING                      "output"
#define IDE_VMPATH_SETTING                      "vmpath"
#define IDE_ARGUMENT_SETTING                    "arguments"
#define IDE_DEBUGINFO_SETTING                   "debuginfo"

// --- ELENA IDE Styles ---
#define SCHEME_COUNT                            2

#define STYLE_KEYWORD                           3
#define STYLE_COMMENT                           4
#define STYLE_OPERATOR                          5
#define STYLE_MESSAGE                           6
#define STYLE_NUMBER                            7
#define STYLE_STRING                            8
#define STYLE_HINT                              9  // !! not used
#define STYLE_ERROR_LINE                        10
#define STYLE_TRACE_LINE                        11
#define STYLE_TRACE                             12
#define STYLE_BREAKPOINT                        13
#define STYLE_HIGHLIGHTED_BRACKET               14
#define STYLE_MAX                               14

// --- UIState ---

enum IDEState
{
   uiEmpty         = 0x00,
   uiFrameShown    = 0x01,
   uiProjectActive = 0x02,
   uiIDEBusy       = 0x04,
   uiDebugging     = 0x08,
   uiAutoRecompile = 0x10,
   uiHighlight     = 0x20,
   uiBracketBold   = 0x40
};

// --- FrameState ---

enum FrameState
{
   editEmpty           = 0x00,
   editHasDocument     = 0x01,
   editHasSelection    = 0x02,
   editCanUndo         = 0x04,
   editCanRedo         = 0x08,
   editModifiedMode    = 0x10,
   editModeChanged     = 0x20,
   editOverwriteMode   = 0x40
};

#ifdef _WIN32

#include "eng\messages.h"
#include "winapi32\winideconst.h"

#elif _LINUX32

#include "eng/messages.h"
#include "gtk-linux32/gtkideconst.h"

#endif

typedef _ELENA_::String<tchar_t, 255> SearchText;

struct SearchOption
{
   SearchText text;
   SearchText newText;
   bool       matchCase;
   bool       wholeWord;

   SearchOption()
   {
      matchCase = wholeWord = false;
   }
};

// --- MessageBookmark ---

struct MessageBookmark
{
   const wchar16_t* module;
   tchar_t*         file;
   size_t           col, row;

   MessageBookmark(const tchar_t* file, const tchar_t* col, const tchar_t* row)
   {
      this->module = NULL;
      this->file = _ELENA_::StringHelper::clone(file);
      this->col = _ELENA_::StringHelper::strToInt(col);
      this->row = _ELENA_::StringHelper::strToInt(row);
   }

   MessageBookmark(const wchar16_t* module, const tchar_t* file, size_t col, size_t row)
   {
      this->module = module;
      this->file = _ELENA_::StringHelper::clone(file);
      this->col = col;
      this->row = row;
   }

   ~MessageBookmark()
   {
      _ELENA_::freestr(file);
   }
};

} // _GUI_

#endif // idecommonH
