//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//    WinAPI: Static dialog implementations
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windialogs.h"

using namespace elena_lang;

// --- FileDialog ---

const wchar_t* FileDialog::SourceFilter = _T("ELENA source file\0*.l\0All types\0*.*\0\0");

FileDialog :: FileDialog(HINSTANCE instance, WindowBase* owner, const wchar_t* filter, const wchar_t* caption,
   const wchar_t* initialDir)
{
   ZeroMemory(&_struct, sizeof(_struct));
   _struct.lStructSize = sizeof(_struct);
   _struct.hwndOwner = owner->handle();
   _struct.hInstance = instance;
   _struct.lpstrCustomFilter = (LPTSTR)nullptr;
   _struct.nMaxCustFilter = 0L;
   _struct.nFilterIndex = 1L;
   _struct.lpstrFilter = filter;
   _struct.lpstrFile = _fileName;
   _struct.nMaxFile = sizeof(_fileName);
   _struct.lpstrFileTitle = nullptr;
   _struct.nMaxFileTitle = 0;
   _struct.lpstrInitialDir = emptystr(initialDir) ? nullptr : initialDir;
   _struct.lpstrTitle = caption;
   _struct.nFileOffset = 0;
   _struct.nFileExtension = 0;
   _struct.lpstrDefExt = nullptr;
   _struct.lCustData = 0;
   _struct.lpfnHook = nullptr;
   _struct.lpTemplateName = nullptr;

   _defaultFlags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_LONGNAMES | DS_CENTER | OFN_HIDEREADONLY;

   _fileName[0] = 0;
}

bool FileDialog :: openFiles(List<path_t, freepath>& files)
{
   _struct.Flags = _defaultFlags | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
   if (::GetOpenFileName(&_struct)) {
      if (emptystr(_fileName + getlength(_fileName) + 1)) {
         files.add(StrUtil::lower(StrUtil::clone(_fileName)));
      }
      else {
         PathString path;
         const wchar_t* p = _fileName + getlength(_fileName) + 1;

         while (!emptystr(_fileName)) {
            path.copy(_fileName);
            path.combine(p);

            // !! always case insensitive?
            files.add(StrUtil::lower(StrUtil::clone(path.str())));

            p += getlength(p) + 1;
         }
      }

      return true;
   }

   return false;
}

bool FileDialog :: saveFile(path_t ext, PathString& path)
{
   _struct.Flags = _defaultFlags | OFN_PATHMUSTEXIST;
   _struct.lpstrDefExt = ext;

   if (::GetSaveFileName(&_struct)) {
      path.copy(_fileName);

      return true;
   }
   else return false;
}
