//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract Assembler declarations
//
//                                              (C)2005-2006, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86jumphelperH
#define x86jumphelperH

#include "x86helper.h"

namespace _ELENA_
{

class x86JumpHelper
{
   x86LabelHelper              _helper;
   Map<const wchar_t*, size_t> _labels;
   Map<const wchar_t*, size_t> _declaredLabels;

public:
   bool checkDeclaredLabel(const wchar_t* label)
   {
      return _declaredLabels.exist(label);
   }

   void writeJxxForward(const wchar_t* label, int prefix, bool shortJump);
   void writeJxxBack(const wchar_t* label, int prefix, bool shortJump);

   void writeJmpForward(const wchar_t* label, bool shortJump);
   void writeJmpBack(const wchar_t* label, bool shortJump);

   void writeLoopForward(const wchar_t* label);
   void writeLoopBack(const wchar_t* label);

   void writeCallForward(const wchar_t* label);
   void writeCallBack(const wchar_t* label);

   bool addLabel(const wchar_t* label);

	x86JumpHelper(MemoryWriter* code)
      : _helper(code)
	{
	}
};

} // _ELENA_

#endif // x86jumphelperH

