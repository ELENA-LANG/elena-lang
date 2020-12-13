//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE general header
//
//                                              (C)2005-2010, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef ideH
#define ideH

#include "gui.h"
#include "idecommon.h"

#ifdef _WIN32

#include "winapi\wineditframe.h"

#elif _LINUX32

#include "gtk-linux32/gtkeditframe.h"

#endif

#endif // ideH
