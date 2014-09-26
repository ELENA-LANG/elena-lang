//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract Assembler declarations
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86jumphelperH
#define x86jumphelperH

#include "x86helper.h"

namespace _ELENA_
{

class x86JumpHelper
{
   x86LabelHelper                _helper;
   Map<const wchar16_t*, size_t> _labels;
   Map<const wchar16_t*, size_t> _declaredLabels;

public:
   bool checkDeclaredLabel(const wchar16_t* label)
   {
      return _declaredLabels.exist(label);
   }

   void writeJxxForward(const wchar16_t* label, int prefix, bool shortJump);
   void writeJxxBack(const wchar16_t* label, int prefix, bool shortJump);

   void writeJmpForward(const wchar16_t* label, bool shortJump);
   void writeJmpBack(const wchar16_t* label, bool shortJump);

   void writeLoopForward(const wchar16_t* label);
   void writeLoopBack(const wchar16_t* label);

   void writeCallForward(const wchar16_t* label);
   void writeCallBack(const wchar16_t* label);

   bool addLabel(const wchar16_t* label);

	x86JumpHelper(MemoryWriter* code)
      : _helper(code)
	{
	}
};

} // _ELENA_

#endif // x86jumphelperH

