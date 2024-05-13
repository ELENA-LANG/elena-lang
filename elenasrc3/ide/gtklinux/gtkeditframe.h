//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container declaration
//                                              (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKEditFrame_H
#define GTKEditFrame_H

#include "gtklinux/gtktextframe.h"

namespace elena_lang
{

// --- EditFrame ---
class EditFrame : public TextViewFrame
{
public:
   EditFrame();
};

}

#endif // GTKEditFrame_H
