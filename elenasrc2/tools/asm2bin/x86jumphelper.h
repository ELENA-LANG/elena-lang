//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract Assembler declarations
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef x86jumphelperH
#define x86jumphelperH

#include "x86helper.h"

namespace _ELENA_
{

class x86JumpHelper
{
   x86LabelHelper       _helper;
   Map<ident_t, size_t> _labels;
   Map<ident_t, size_t> _declaredLabels;

public:
   bool checkDeclaredLabel(ident_t label)
   {
      return _declaredLabels.exist(label);
   }

   void writeJxxForward(ident_t label, int prefix, bool shortJump);
   void writeJxxBack(ident_t label, int prefix, bool shortJump);

   void writeJmpForward(ident_t label, bool shortJump);
   void writeJmpBack(ident_t label, bool shortJump);

   void writeLoopForward(ident_t label);
   void writeLoopBack(ident_t label);

   void writeCallForward(ident_t label);
   void writeCallBack(ident_t label);

   bool addLabel(ident_t label);

   bool checkAllUsedLabels(ident_t errorMessage, int procedureNumber);
   bool checkAllUsedLabels(ident_t errorMessage, ident_t procedureName); // duplication??

	x86JumpHelper(MemoryWriter* code)
      : _helper(code)
	{
	}
};

} // _ELENA_

#endif // x86jumphelperH

