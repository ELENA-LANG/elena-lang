//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Header File
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkcommonH
#define gtkcommonH

#include <gtkmm.h>

#include "../guicommon.h"

namespace _GUI_
{

typedef Gtk::Widget Control;
typedef Gtk::Window Window;

// --- Clipboard ---

class Clipboard
{
public:
   //static bool isAvailable();

   bool begin(Window* window)
   {
      //return open(window->getHandle());
      return true;
   }

   void end()
   {
      //close();
   }

   void settext(const _text_t* text);

   _text_t* gettext();

   void freetext(_text_t* text);
};

// --- misc functions ---

inline bool isPathRelative(const _text_t* path)
{
   return _ELENA_::Path::isRelative(path, _ELENA_::getlength(path));
}

inline void canonicalize(_ELENA_::Path& path)
{
   int index = path.find('\\');
   while (index != -1) {
      path[index] = '/';

      index = path.find('\\');
   }
//   TCHAR p[MAX_PATH];
//
//   ::PathCanonicalize(p, path);
//
//   path.copy(p);
}

inline void makePathRelative(_ELENA_::Path& path, const _path_t* rootPath)
{
   int len = _ELENA_::getlength(rootPath);
   if (_ELENA_::StringHelper::compare(path, rootPath, len)) {
      _ELENA_::Path tempPath(path + len);

      path.copy(tempPath);
   }
}

} // _GUI_

#endif // gtkcommonH
