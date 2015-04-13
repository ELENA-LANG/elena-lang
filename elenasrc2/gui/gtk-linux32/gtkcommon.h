//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkcommonH
#define gtkcommonH

#include "../guicommon.h"
#include <gtkmm.h>

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

   void settext(const char* text);

   char* gettext();

   void freetext(char* text);
};

// --- DateTime ---
// !! temporal
struct DateTime
{
private:
   struct stat _time;

public:
   static DateTime getFileTime(const char* path);

   bool operator > (const DateTime dt) const
   {
      return _time.st_mtime > dt._time.st_mtime;
   }

   DateTime()
   {
      memset(&_time, 0, sizeof(_time));
   }
};


// --- misc functions ---

inline bool isPathRelative(const char* path)
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

inline void makePathRelative(_ELENA_::Path& path, const char* rootPath)
{
   int len = _ELENA_::getlength(rootPath);
   if (_ELENA_::StringHelper::compare(path, rootPath, len)) {
      _ELENA_::Path tempPath(path + len);

      path.copy(tempPath);
   }
}

} // _GUI_

#endif // gtkcommonH
