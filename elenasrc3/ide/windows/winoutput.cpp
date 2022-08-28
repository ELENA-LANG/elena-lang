//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Process Output Implementation File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winoutput.h"

using namespace elena_lang;

// --- ProcessOutput ---

LRESULT CALLBACK ProcessOutput::Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   ProcessOutput* window = (ProcessOutput*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
   return window->OutputProc(hWnd, Message, wParam, lParam);
}

ProcessOutput :: ProcessOutput(bool readOnly) : 
   ControlBase(nullptr, 0, 0, 50, 50),
   _editProc(nullptr),
   _readOnly(readOnly)
{

}

HWND ProcessOutput :: createControl(HINSTANCE instance, ControlBase* owner)
{
   int styles = WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
   if (_readOnly)
      styles |= ES_READONLY;

   _handle = ::CreateWindowEx(
      0, WC_EDIT, _title,
      styles,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, owner->handle(), nullptr, instance, (LPVOID)this);

   _editProc = (WNDPROC)SetWindowLongPtr(_handle, GWLP_WNDPROC, (LONG_PTR)Proc);
   ::SetWindowLongPtr(_handle, GWLP_USERDATA, (LONG_PTR)this);

   return _handle;
}

LRESULT ProcessOutput :: OutputProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
      case WM_CHAR:
      {
         int eol = GetWindowTextLength(_handle);
         SendMessage(_handle, EM_SETSEL, (WPARAM)eol, (LPARAM)eol);

         if ((wchar_t)wParam == 13) {
//            _redirector->write("\r\n", 2);
         }
         else if ((wchar_t)wParam == 0x8) {
//            _redirector->write(0x8);
         }
         else if ((wchar_t)wParam >= 0x20) {
//            _redirector->write((wchar_t)wParam);
         }

         break;
      }
   }
   return _editProc(hWnd, Message, wParam, lParam);
}

void ProcessOutput :: onOutput(const char* text)
{
   wchar_t buffer[256];
   size_t buf_len = 255;

   while (!emptystr(text)) {
      size_t text_len = getlength(text);
      if (text_len > 128)
         text_len = 128;

      buf_len = MultiByteToWideChar(CP_OEMCP, 0, text, (int)text_len, buffer, 256);
      buffer[buf_len] = 0;

      int eol = GetWindowTextLength(_handle);
      SendMessage(_handle, EM_SETSEL, (WPARAM)eol, (LPARAM)eol);
      SendMessage(_handle, EM_REPLACESEL, 0, (LPARAM)((LPWSTR)buffer));

      text += text_len;
   }
}

void ProcessOutput :: onErrorOutput(const char* s)
{
   onOutput(s);
}

// --- CompilerOutput ---

CompilerOutput :: CompilerOutput()
   : ProcessOutput(true)
{

}