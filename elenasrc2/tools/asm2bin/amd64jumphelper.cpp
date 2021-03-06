//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "amd64jumphelper.h"
#include "assemblerException.h"

using namespace _ELENA_;

void AMD64JumpHelper::writeJxxForward(ident_t labelName, int prefix, bool shortJump)
{
   int label = _labels.get(labelName);
   if (label == 0) {
      label = _labels.Count() + 1;

      _labels.add(labelName, label);
   }

   if (shortJump) {
      _helper.writeShortJxxForward(label, (AMD64Helper::AMD64JumpType)prefix);
   }
   else _helper.writeJxxForward(label, (AMD64Helper::AMD64JumpType)prefix);
}

void AMD64JumpHelper::writeJxxBack(ident_t label, int prefix, bool shortJump)
{
   if (shortJump) {
      _helper.writeShortJxxBack((AMD64Helper::AMD64JumpType)prefix, _labels.get(label));
   }
   else _helper.writeNearJxxBack((AMD64Helper::AMD64JumpType)prefix, _labels.get(label));
}

void AMD64JumpHelper :: writeJmpForward(ident_t labelName, bool shortJump)
{
   int label = _labels.get(labelName);
   if (label == 0) {
      label = _labels.Count() + 1;

      _labels.add(labelName, label);
   }

   if (shortJump) {
      _helper.writeShortJmpForward(label);
   }
   else _helper.writeJmpForward(label);
}

void AMD64JumpHelper :: writeJmpBack(ident_t label, bool shortJump)
{
   if (shortJump) {
      _helper.writeShortJmpBack(_labels.get(label));
   }
   else _helper.writeNearJmpBack(_labels.get(label));
}

//void x86JumpHelper::writeLoopBack(ident_t label)
//{
//   _helper.writeLoopBack(_labels.get(label));
//}
//
//void x86JumpHelper::writeLoopForward(ident_t labelName)
//{
//   int label = _labels.get(labelName);
//   if (label == 0) {
//      label = _labels.Count() + 1;
//
//      _labels.add(labelName, label);
//   }
//
//   _helper.writeLoopForward(label);
//}
//
//void x86JumpHelper::writeCallBack(ident_t label)
//{
//   _helper.writeCallBack(_labels.get(label));
//}
//
//void x86JumpHelper::writeCallForward(ident_t labelName)
//{
//   int label = _labels.get(labelName);
//   if (label == 0) {
//      label = _labels.Count() + 1;
//
//      _labels.add(labelName, label);
//   }
//
//   _helper.writeCallForward(label);
//}

bool AMD64JumpHelper::addLabel(ident_t labelName)
{
   int label = _labels.get(labelName);
   if (label == 0) {
      label = _labels.Count() + 1;

      _labels.add(labelName, label);
   }

   _declaredLabels.add(labelName, 1, true);

   _helper.setLabel(label);

   return true;
}

//bool x86JumpHelper::checkAllUsedLabels(ident_t errorMessage, int procedureNumber)
//{
//	auto it = _labels.start();
//	while (!it.Eof())
//	{
//		ident_t label = it._item()->key;
//
//		// Check if label is declared
//		if (_declaredLabels.get(label) == NULL)
//			throw AssemblerException(errorMessage, label, procedureNumber);
//		it++;
//	}
//	return false;
//}
//
//bool x86JumpHelper::checkAllUsedLabels(ident_t errorMessage, ident_t procedureName)
//{
//   auto it = _labels.start();
//   while (!it.Eof())
//   {
//      ident_t label = it._item()->key;
//
//      // Check if label is declared
//      if (_declaredLabels.get(label) == NULL)
//         throw AssemblerException(errorMessage, label, procedureName);
//      it++;
//   }
//   return false;
//}