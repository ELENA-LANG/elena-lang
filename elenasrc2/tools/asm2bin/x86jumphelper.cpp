//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "x86jumphelper.h"

using namespace _ELENA_;

void x86JumpHelper :: writeJxxForward(const wchar16_t* labelName, int prefix, bool shortJump)
{
   int label = _labels.get(labelName);
   if (label == 0) {
      label = _labels.Count() + 1;

      _labels.add(labelName, label);
   }

   if (shortJump) {
      _helper.writeShortJxxForward(label, (x86Helper::x86JumpType)prefix);
   }
   else _helper.writeJxxForward(label, (x86Helper::x86JumpType)prefix);
}

void x86JumpHelper :: writeJxxBack(const wchar16_t* label, int prefix, bool shortJump)
{
   if (shortJump) {
      _helper.writeShortJxxBack((x86Helper::x86JumpType)prefix, _labels.get(label));
   }
   else _helper.writeNearJxxBack((x86Helper::x86JumpType)prefix, _labels.get(label));
}

void x86JumpHelper :: writeJmpForward(const wchar16_t* labelName, bool shortJump)
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

void x86JumpHelper :: writeJmpBack(const wchar16_t* label, bool shortJump)
{
   if (shortJump) {
      _helper.writeShortJmpBack(_labels.get(label));
   }
   else _helper.writeNearJmpBack(_labels.get(label));
}

void x86JumpHelper :: writeLoopBack(const wchar16_t* label)
{
   _helper.writeLoopBack(_labels.get(label));
}

void x86JumpHelper :: writeLoopForward(const wchar16_t* labelName)
{
   int label = _labels.get(labelName);
   if (label == 0) {
      label = _labels.Count() + 1;

      _labels.add(labelName, label);
   }

   _helper.writeLoopForward(label);
}

void x86JumpHelper :: writeCallBack(const wchar16_t* label)
{
   _helper.writeCallBack(_labels.get(label));
}

void x86JumpHelper :: writeCallForward(const wchar16_t* labelName)
{
   int label = _labels.get(labelName);
   if (label == 0) {
      label = _labels.Count() + 1;

      _labels.add(labelName, label);
   }

   _helper.writeCallForward(label);
}

bool x86JumpHelper :: addLabel(const wchar16_t* labelName)
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
