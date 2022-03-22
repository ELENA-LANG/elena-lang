//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDEVIEW_H
#define IDEVIEW_H

#include "view.h"
#include "editframe.h"

namespace elena_lang
{

class IDEModel
{
public:
   SourceViewModel _sourceViewModel;

   SourceViewModel* viewModel() { return &_sourceViewModel; }

   IDEModel() = default;
};

} // elena:lang

#endif // IDEVIEW_H
