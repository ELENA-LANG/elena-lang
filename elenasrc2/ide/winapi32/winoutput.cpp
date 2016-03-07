//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Output class implementation
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winoutput.h"
#include "winideconst.h"
//#include "idesettings.h"

using namespace _GUI_;
using namespace _ELENA_;

// --- Output ---

Output :: Output(Control* owner, bool readOnly, const wchar_t* name)
   : Control(0, 0, 40, 40)
{
   _instance = owner->_getInstance();

   _handle = ::CreateWindowEx(
      0, _T("edit"), name,
      WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY,
      _left, _top, _width, _height, owner->getHandle(), NULL, _instance, (LPVOID)this);

   _redirector = new WindowRedirector(this, readOnly, 50);
}

Output :: ~Output()
{
   freeobj(_redirector);
}

void Output :: clear()
{
   ::SendMessage(_handle, EM_SETREADONLY, FALSE, 1);
   ::SendMessage(_handle, EM_SETSEL, 0, -1);
   ::SendMessage(_handle, WM_CLEAR, 0, 0);
   ::SendMessage(_handle, EM_SETREADONLY, TRUE, 1);
}

void Output :: onOutput(const char* text)
{
   wchar_t buffer[256];
   size_t buf_len = 255;

   while (!emptystr(text)) {
      size_t text_len = getlength(text);
      if (text_len > 128)
         text_len = 128;

      buf_len = MultiByteToWideChar(CP_OEMCP, 0, text, text_len, buffer, 256);
      buffer[buf_len] = 0;

      int eol = GetWindowTextLength(_handle);
      SendMessage(_handle, EM_SETSEL, (WPARAM)eol, (LPARAM)eol);
      SendMessage(_handle, EM_REPLACESEL, 0, (LPARAM)((LPWSTR)buffer));

      text += text_len;
   }
}

wchar_t* Output :: getOutput()
{
   int length = (int)SendMessage(_handle, WM_GETTEXTLENGTH, 0, 0);

   if (length > 0) {
      wchar_t* buffer = StringHelper::allocate(length + 1, (const wchar_t*)NULL);

      SendMessage(_handle, WM_GETTEXT, length + 1, (LPARAM)buffer);

      return buffer;
   }
   else return NULL;
}

// --- CompilerOutput ---

CompilerOutput :: CompilerOutput(Control* owner, Control* receptor)
   : Output(owner, true, L"output")
{
   _receptor = receptor->getHandle();
   _postponedAction = 0;
}

bool CompilerOutput :: execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir, int postponedAction)
{
   clear();

   _postponedAction = postponedAction;

   return _redirector->execute(path, cmdLine, curDir);
}

void CompilerOutput :: afterExecution(DWORD error)
{
   if (error == 0) {
      _notify(_receptor, IDM_COMPILER_SUCCESSFUL, _postponedAction);
   }
   else if ((int)error == -1) {
      _notify(_receptor, IDM_COMPILER_WITHWARNING);
   }
   else _notify(_receptor, IDM_COMPILER_UNSUCCESSFUL, _postponedAction);

   _postponedAction = 0;
}
