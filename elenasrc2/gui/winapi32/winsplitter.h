//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Splitter class header 
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winsplitterH
#define winsplitterH

#include "wincommon.h"

namespace _GUI_
{

#define VSPLTR_WND_CLASS                        _T("ELENA VSPLTR CLASS")
#define HSPLTR_WND_CLASS                        _T("ELENA HSPLTR CLASS")

class Splitter : public Window
{
   HWND     _owner;

   Control* _client;
   bool     _vertical;
   int      _cursor;

   POINT    _srcPos;
   bool     _mouseCaptured;

   int      _notifyCode;

   virtual LRESULT _WindProc(HWND hWnd, size_t Message, WPARAM wParam, LPARAM lParam);

   virtual bool _onSetCursor();

   void shiftOn(int delta);

public:
   static void _registerClass(HINSTANCE instance, bool vertical);

   virtual bool isVisible(); 

   virtual size_t getWidth() const;
   virtual size_t getHeight() const;

   virtual void _setCoordinate(int x, int y);
   virtual void _setWidth(size_t width);
   virtual void _setHeight(size_t height);
   
   virtual void _resize();

   Splitter(Control* owner, Control* client, bool vertical, int notifyCode);
};

} // _GUI_

#endif // winsplitterH

