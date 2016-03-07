//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Output class header
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winoutputH
#define winoutputH

#include "winapi32\winredirect.h"

namespace _GUI_
{

class Output : public Control, public RedirectorListener
{
protected:
   WindowRedirector* _redirector;
   
public:
   virtual void clear();
   virtual void onOutput(const char* text);
   virtual void afterExecution(DWORD exitCode)
   {

   }

   wchar_t* getOutput();
   
   Output(Control* owner, bool readOnly, const wchar_t* name);
   ~Output();
};

class CompilerOutput : public Output
{
   int  _postponedAction;
   HWND _receptor;    // notify receptor

public:
   bool execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir, int postponedAction);

   virtual void afterExecution(DWORD exitCode);

   CompilerOutput(Control* owner, Control* receptor);
};

} // _GUI_

#endif // winoutputH
