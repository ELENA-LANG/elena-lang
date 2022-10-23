//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINIDE_H
#define WINIDE_H

#include "windows/winsdi.h"
#include "windows/windialogs.h"
#include "idecontroller.h"
#include "ideview.h"

namespace elena_lang
{
   // --- Clipboard ---
   class Clipboard : public ClipboardBase
   {
      ControlBase* _owner;

      bool begin();
      void clear();
      void copy(HGLOBAL buffer);
      HGLOBAL get();
      void end();

      HGLOBAL createBuffer(size_t size);
      wchar_t* allocateBuffer(HGLOBAL buffer);
      void freeBuffer(HGLOBAL buffer);

   public:
      bool copyToClipboard(DocumentView* docView) override;
      void pasteFromClipboard(DocumentView* docView) override;

      Clipboard(ControlBase* owner);
   };

   // --- IDEWindow ---
   class IDEWindow : public SDIWindow
   {
      Dialog         dialog;
      Clipboard      clipboard;

      HINSTANCE      _instance;

      IDEModel*      _model;
      IDEController* _controller;

      void onModelChange(ExtNMHDR* hdr);
      void onNotifyMessage(ExtNMHDR* hdr);

      void onDoubleClick(NMHDR* hdr);

      void onTabSelChanged(HWND wnd);

      bool onCommand(int command) override;
      void onNotify(NMHDR* hdr) override;
      void onActivate() override;

      void onComilationStart();
      void onCompilationEnd(int exitCode);
      void onErrorHighlight(int index);

      void openResultTab(int controlIndex);
      void setChildFocus(int controlIndex);

      void newFile();
      void openFile();
      void saveFile();
      void closeFile();
      void exit();

      void undo();
      void redo();
      bool copyToClipboard();
      void pasteFromClipboard();
      void deleteText();

   public:
      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance);
   };

}

#endif