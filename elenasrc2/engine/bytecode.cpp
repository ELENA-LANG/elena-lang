//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implementation of ELENA byte code routines.
//
//                                                 (C)2009-2012, by Alexei Rakov
//------------------------------------------------------------------------------

#include "elena.h"
// -----------------------------------------------------------------------------
#include "bytecode.h"

#define OPCODE_ACCADDN      "accaddn"
#define OPCODE_ACCBOXN      "accboxn"
#define OPCODE_ACCCOPYFPI   "acccopyfpi"
#define OPCODE_ACCCOPYN     "acccopyn"
#define OPCODE_ACCCOPYR     "acccopyr"
#define OPCODE_ACCCOPYSELF  "acccopyself"
#define OPCODE_ACCCREATE    "acccreate"
#define OPCODE_ACCCREATEN   "acccreaten"
#define OPCODE_ACCGETFI     "accgetfi"
#define OPCODE_ACCGETSI     "accgetsi"
#define OPCODE_ACCLOADACCI  "accloadacci"
#define OPCODE_ACCLOADFI    "accloadfi"
#define OPCODE_ACCLOADR     "accloadr"
#define OPCODE_ACCLOADSELFI "accloadselfi"
#define OPCODE_ACCLOADSI    "accloadsi"
#define OPCODE_ACCSAVEFI    "accsavefi"
#define OPCODE_ACCSAVER     "accsaver"
#define OPCODE_ACCSAVESELFI "accsaveselfi"
#define OPCODE_ACCSAVESI    "accsavesi"
#define OPCODE_ACCSWAPSI    "accswapsi"
#define OPCODE_BREAKPOINT   "breakpoint"
#define OPCODE_BSREDIRECT   "bsredirect"
#define OPCODE_CALLACC      "callacc"
#define OPCODE_CALLEXTR     "callextr"
#define OPCODE_CALLR        "callr"
#define OPCODE_CALLSI       "callsi"
#define OPCODE_CLOSE        "close"
#define OPCODE_COPYFPI      "copyfpi"
#define OPCODE_CREATE       "create"
#define OPCODE_CREATEN      "createn"
#define OPCODE_ELSE         "else"
#define OPCODE_ELSEFLAG     "elseflag"
#define OPCODE_ELSESI       "elsesi"
#define OPCODE_ELSER        "elser"
#define OPCODE_EVALR        "evalr"
#define OPCODE_EXCLUDE      "exclude"
#define OPCODE_GET          "get"
#define OPCODE_GETLEN       "getlen"
#define OPCODE_HOOK         "hook"
#define OPCODE_IACCCOPYR    "iacccopyr"
#define OPCODE_IACCFILLR    "iaccfillr"
#define OPCODE_INCLUDE      "include"
#define OPCODE_INIT         "init"
#define OPCODE_INCFI        "incfi"
#define OPCODE_INCSI        "incsi"
#define OPCODE_JUMP         "jump"
#define OPCODE_JUMPACC      "jumpacc"
#define OPCODE_JUMPACCN     "jumpaccn"
#define OPCODE_MCCADDACCI   "mccaddacci"
#define OPCODE_MCCADDM      "mccaddm"
#define OPCODE_MCCCOPYACCI  "mcccopyacci"
#define OPCODE_MCCCOPYFI    "mcccopyfi"
#define OPCODE_MCCCOPYM     "mcccopym"
#define OPCODE_MCCCOPYPRMFI "mcccopyprmfi"
#define OPCODE_MCCCOPYSI    "mcccopysi"
#define OPCODE_MCCCOPYSUBJ  "mcccopysubj"
#define OPCODE_MCCCOPYVERB  "mcccopyverb"
#define OPCODE_MCCELSE      "mccelse"
#define OPCODE_MCCELSEACC   "mccelseacc"
#define OPCODE_MCCELSEACCI  "mccelseacci"
#define OPCODE_MCCREVERSE   "mccreverse"
#define OPCODE_MCCTHEN      "mccthen"
#define OPCODE_MCCTHENACC   "mccthenacc"
#define OPCODE_MCCTHENACCI  "mccthenacci"
#define OPCODE_NOP          "nop"
#define OPCODE_NWRITE       "nwrite"
#define OPCODE_OPEN         "open"
#define OPCODE_POP          "pop"
#define OPCODE_POPACC       "popacc"
#define OPCODE_POPACCI      "popacci"
#define OPCODE_POPMCC       "popmcc"
#define OPCODE_POPFI        "popfi"
#define OPCODE_POPN         "popn"
#define OPCODE_POPSELF      "popself"
#define OPCODE_POPSELFI     "popselfi"
#define OPCODE_POPSI        "popsi"
#define OPCODE_POPSPI       "popspi"
#define OPCODE_PUSHACC      "pushacc"
#define OPCODE_PUSHACCI     "pushacci"
#define OPCODE_PUSHFI       "pushfi"
#define OPCODE_PUSHFPI      "pushfpi"
#define OPCODE_PUSHI        "pushi"
#define OPCODE_PUSHMCC      "pushmcc"
#define OPCODE_PUSHN        "pushn"
#define OPCODE_PUSHR        "pushr"
#define OPCODE_PUSHSELF     "pushself"
#define OPCODE_PUSHSELFI    "pushselfi"
#define OPCODE_PUSHSI       "pushsi"
#define OPCODE_PUSHSPI      "pushspi"
#define OPCODE_QUIT         "quit"
#define OPCODE_QUITN        "quitn"
#define OPCODE_QUITMCC      "quitmcc"
#define OPCODE_RCALLM       "rcallm"
#define OPCODE_RCALLN       "rcalln"
#define OPCODE_RESERVE      "reserve"
#define OPCODE_RESTORE      "restore"
#define OPCODE_RETHROW      "rethrow"
#define OPCODE_SET          "set"
#define OPCODE_SWAPSI       "swapsi"
#define OPCODE_THEN         "then"
#define OPCODE_THENFLAG     "thenflag"
#define OPCODE_THENR        "thenr"
#define OPCODE_THENSI       "thensi"
#define OPCODE_THROW        "throw"
#define OPCODE_UNHOOK       "unhook"
#define OPCODE_WRITEACC     "writeacc"
#define OPCODE_XACCCOPYFPI  "xacccopyfpi"
#define OPCODE_XACCSAVEFI   "x_accsavefi"
#define OPCODE_XMCCCOPYM    "x_mcccopym"
#define OPCODE_XPOPACCI     "x_popacci"
#define OPCODE_XPUSHFPI     "x_pushfpi"

#define OPCODE_UNKNOWN      "unknown"

using namespace _ELENA_;

// --- help functions ---
inline bool IsJump(ByteCode code)
{
   switch(code) {
      case bcJump:
      case bcElse:
      case bcThen:
      case bcMccElseAcc:
      case bcMccThenAcc:
      case bcElseR:
      case bcThenR:
      case bcMccElse:
      case bcMccThen:
      case bcElseSI:
      case bcThenSI:
      case bcMccElseAccI:
      case bcMccThenAccI:
      case bcElseFlag:
      case bcThenFlag:
            return true;
      default:
            return false;
      }
}

// --- CommandTape ---

void CommandTape :: write(ByteCode code)
{
   write(ByteCommand(code));
}

void CommandTape :: write(ByteCode code, int argument)
{
   write(ByteCommand(code, argument));
}

void CommandTape :: write(ByteCode code, int argument, int additional)
{
   write(ByteCommand(code, argument, additional));
}

void CommandTape :: write(ByteCode code, TapeStructure argument, int additional)
{
   write(ByteCommand(code, argument, additional));
}

int CommandTape :: resolvePseudoArg(PseudoArg argument)
{
   int realArg = 0;
   if (argument == baCurrentLabel) {
      realArg = labels.peek();
   }
   else if (argument == baFirstLabel) {
      realArg = *labels.end();
   }
   else if (argument == baPreviousLabel) {
      Stack<int>::Iterator it = labels.start();
      if (!it.Eof()) {
         it++;

         realArg = *it;
      }
   }
   else if (argument == baPrev2Label) {
      Stack<int>::Iterator it = labels.start();
      if (!it.Eof()) {
         it++;
         it++;

         realArg = *it;
      }
   }
   return realArg;
}

void CommandTape :: write(ByteCode code, PseudoArg argument)
{
   write(ByteCommand(code, resolvePseudoArg(argument)));
}

void CommandTape :: write(ByteCode code, PseudoArg argument, int additional)
{
   write(ByteCommand(code, resolvePseudoArg(argument), additional));
}

void CommandTape :: write(ByteCommand command)
{
   tape.add(command);
}

void CommandTape :: insert(ByteCodeIterator& it, ByteCommand command)
{
   tape.insertBefore(it, command);
}

ByteCodeIterator CommandTape :: find(ByteCode code, int argument)
{
   ByteCodeIterator it = tape.end();
   while (!it.First()) {
      if ((*it).code == code && (*it).argument == argument)
         return it;

      it--;
   }
   return tape.end()++;
}

ByteCodeIterator CommandTape :: find(ByteCode code)
{
   ByteCodeIterator it = tape.end();
   while (!it.First()) {
      if ((*it).code == code)
         return it;

      it--;
   }
   return tape.end()++;
}

void CommandTape :: import(_Memory* section)
{
   ByteCode code;
   int      argument = 0;
   int      additional = 0;

   Map<int, int> labels;

   MemoryReader reader(section);
   while (!reader.Eof()) {
      argument = 0;
      additional = 0;

      code = (ByteCode)reader.getByte();
      if (code >= 0x20) {
         argument = reader.getDWord();
      }
      if (code >= 0xE0) {  
         additional = reader.getDWord();
      }
      if (IsJump(code)) {
         int offset = 0;
         int label = 0;
         if (code >= 0xE0) {
            offset = additional;
         }
         else offset = argument;

         if (!labels.exist(offset)) {
            label = labels.Count() + 0x1000;
            labels.add(offset, label);
         }
         else label = labels.get(offset);

         if (code >= 0xE0) {
            write(code, label, argument);
         }
         else write(code, label);
      }
      else if (code == bcNop) {
         int label;
         if (!labels.exist(reader.Position() - 1)) {
            label = labels.Count() + 0x1000;
            labels.add(reader.Position() - 1, label);
         }
         else label = labels.get(reader.Position() - 1);

         write(blLabel, label);
      }
      else write(code, argument, additional);
   }   
}

inline void addJump(int label, int index, CachedMemoryMap<int, int, 20>& labels, 
                    CachedMemoryMap<int, int, 40>& jumps, 
                    CachedMemoryMap<int, int, 40>& fixes)
{
   int labelIndex = labels.get(label);
   if (labelIndex != -1) {
      jumps.add(index, labelIndex);
   }
   // if the label is not yet defined, the jump should be resolved later
   else fixes.add(label, index);
}

// return true if there is a idle jump
inline bool fixJumps(int label, int labelIndex, CachedMemoryMap<int, int, 40>& fixes, CachedMemoryMap<int, int, 40>& jumps)
{
   bool idleJump = false;

   CachedMemoryMap<int, int, 40>::Iterator it = fixes.start();
   while (!it.Eof()) {
      if (it.key() == label) {
         if (1 + *it == labelIndex) {
            idleJump = true;
         }
         else jumps.add(*it, labelIndex);
      }

      it++;
   }
   return idleJump;
}

inline bool markAsReached(int blockStart, int blockEnd, CachedMemoryMap<int, int, 20>& blocks, CachedMemoryMap<int, int, 40>& jumps)
{
   bool applied = false;

   CachedMemoryMap<int, int, 40>::Iterator it = jumps.start();
   while (!it.Eof()) {
      if (it.key() >= blockStart && it.key() < blockEnd) {
         CachedMemoryMap<int, int, 20>::Iterator b_it = blocks.getIt(*it);
         if (*b_it != -1) {
            applied = true;

            *b_it = -1;
         }
      }

      it++;
   }

   return applied;
}

inline int getBlockEnd(CachedMemoryMap<int, int, 20>::Iterator it, int length)
{
   it++;

   if (!it.Eof()) {
      return it.key();
   }
   else return length;
}

inline void removeIdleJump(ByteCodeIterator it)
{
   while (true) {
      switch((ByteCode)*it) {
         case bcJump:
         case bcElse:
         case bcThen:
         case bcMccElseAcc:
         case bcMccThenAcc:
         case bcElseR:
         case bcThenR:
         case bcMccElse:
         case bcMccThen:
         case bcElseSI:
         case bcThenSI:
         case bcElseFlag:
         case bcThenFlag:
         case bcMccElseAccI:
         case bcMccThenAccI:
            *it = bcNop;
            return;
      }
      it--;
   }
}

inline bool optimizeProcJumps(ByteCodeIterator& it)
{
   bool modified = false;

   ByteCodeIterator start_it = it;

   CachedMemoryMap<int, int, 20> blocks; // value: 0 - unreached, -1 - reached, 1 - partial (reached if the previous one can be reached)
   CachedMemoryMap<int, int, 20> labels(-1);
   CachedMemoryMap<int, int, 40> jumps;
   CachedMemoryMap<int, int, 40> fixes;

   blocks.add(0, -1);

   // populate blocks and jump lists
   int index = 0;
   while (*it != blEnd) {
      // skip pseudo commands (except labels)
      ByteCode code = *it;

      if (code == blLabel) {
         labels.add((*it).argument, index);

         // create partial block if it was not created
         if (!blocks.exist(index))
            blocks.add(index, 1);

         // fix forward jumps
         // if there is a idle jump it should be removed
         if (fixJumps((*it).argument, index, fixes, jumps)) {
            removeIdleJump(it);
            // mark its block as partial
            *blocks.getIt(index) = 1;
         }
      }
      else if (code <= bcReserved && code >= bcNop) {
         switch(code) {
            case bcThrow:
            case bcJumpAcc:
            case bcQuit:
            case bcQuitMcc:
            case bcQuitN:
            case bcJumpAccN:
               blocks.add(index + 1, 0);
               break;
            case bcJump:
               blocks.add(index + 1, 0);
            case bcElse:
            case bcThen:              
            case bcMccElseAcc:
            case bcMccThenAcc:
            case bcElseR:
            case bcThenR:     
            case bcMccElse:
            case bcMccThen:
            case bcElseSI:
            case bcThenSI:
            case bcMccElseAccI:
            case bcMccThenAccI:
            case bcElseFlag:
            case bcThenFlag:
            case bcHook:
               addJump((*it).argument, index, labels, jumps, fixes);
               break;
         }

         index++;
      }
      it++;
   }

   int length = index;

   // find out the blocks which can be reached
   bool marked = true;
   while (marked) {
      marked = false;
      
      CachedMemoryMap<int, int, 20>::Iterator b_it = blocks.start();
      bool prev = false;
      while (!b_it.Eof()) {
         if (*b_it == -1) {
            prev = true;

            marked |= markAsReached(b_it.key(), getBlockEnd(b_it, length), blocks, jumps);
         }
         else if (*b_it == 1 && prev) {
            *b_it = -1;
            marked |= markAsReached(b_it.key(), getBlockEnd(b_it, length), blocks, jumps);
         }
         else prev = false;

         b_it++;
      }
   }

   // remove unreached blocks
   it = start_it;
   index = 0;
   CachedMemoryMap<int, int, 20>::Iterator b_it = blocks.start();
   int blockEnd = getBlockEnd(b_it, length);
   while (*it != blEnd) {
      ByteCode code = *it;
      bool command = (code <= bcReserved && code >= bcNop);

      if (index == blockEnd) {
         b_it++;
         blockEnd = getBlockEnd(b_it, length);
      }

      // clear unreachable block
      if (*b_it != -1 && code != bcNone) {
         (*it).code = bcNone;
         modified = true;
      }

      if (command)
         index++;

      it++;
   }

   return modified;
}

bool CommandTape :: optimizeJumps(CommandTape& tape)
{
   bool modified = false;

   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      if (*it == blBegin && ((*it).argument == bsMethod || (*it).argument == bsSymbol))
         modified |= optimizeProcJumps(it);

      it++;
   }

   return modified;
}

// --- TransformTape ---

void TransformTape :: transform(ByteCodeIterator& trans_it, Node replacement)
{
   ByteCodeIterator target_it = trans_it;

   ByteCodePattern pattern = replacement.Value();
   while (pattern.code != bcNone) {
      // skip meta commands (except label)
      while (!test(*trans_it, blLabelMask) && ((*trans_it).code > bcReserved || (*trans_it).code == bcNop))
         trans_it--;

      (*trans_it).code = pattern.code;

      switch(pattern.argumentType) {
         case braAdd:
            (*trans_it).argument += pattern.argument;
            break;
         case braValue:
            (*trans_it).argument = pattern.argument;
            break;
         case braCopy:
            if (pattern.argument == 1) {
               (*target_it).argument = (*trans_it).argument;
            }
            else if (pattern.argument == 2) {
               (*target_it).additional = (*trans_it).argument;
            }
            break;
      }      

      trans_it--;
      replacement = replacement.FirstNode();
      pattern = replacement.Value();
   }
}

TransformTape::Node TransformTape :: makeStep(Node& step, ByteCommand& command)
{
   Node::ChildEnumerator child = step.Children();
   Node defaultNode(&trie);

   while (!child.Eof()) {
      Node current = child.Node();
      if (current.Value() == command) {
         return current;
      }
      else if (current.Value().code == bcMatch)
         return current;

      child++;
   }

   if (defaultNode != step) {
      return makeStep(defaultNode, command);
   }
   else return defaultNode;
}

bool TransformTape :: apply(CommandTape& commandTape)
{
   ByteCodeIterator it = commandTape.start();

   bool applied = false;
   Node current(&trie);
   while (!it.Eof()) {
      // skip meta commands (except labels)
      if (test(*it, blLabelMask) || (*it < bcReserved && *it > bcNop)) {
         current = makeStep(current, *it);

         // check if the end node is reached
         if (current.Value().code == bcMatch) {
            ByteCodeIterator trans_it = it;

            transform(--trans_it, current.FirstNode());

            applied = true;
            current = Node(&trie);
         }
         else it++;
      }
      else it++;
   } 

   return applied;
}

// --- ByteCodeCompiler ---

ByteCode ByteCodeCompiler :: code(const wchar16_t* s)
{
   if (ConstantIdentifier::compare(s, OPCODE_ACCADDN)) {
      return bcAccAddN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCBOXN)) {
      return bcAccBoxN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCCOPYFPI)) {
      return bcAccCopyFPI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCCOPYN)) {
      return bcAccCopyN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCCOPYR)) {
      return bcAccCopyR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCCREATE)) {
      return bcAccCreate;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCCREATEN)) {
      return bcAccCreateN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCGETFI)) {
      return bcAccGetFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCGETSI)) {
      return bcAccGetSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCLOADACCI)) {
      return bcAccLoadAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCLOADFI)) {
      return bcAccLoadFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCLOADR)) {
      return bcAccLoadR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCCOPYSELF)) {
      return bcAccCopySelf;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCLOADSELFI)) {
      return bcAccLoadSelfI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCLOADSI)) {
      return bcAccLoadSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCSAVEFI)) {
      return bcAccSaveFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCSAVER)) {
      return bcAccSaveR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCSAVESELFI)) {
      return bcAccSaveSelfI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCSAVESI)) {
      return bcAccSaveSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACCSWAPSI)) {
      return bcAccSwapSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BREAKPOINT)) {
      return bcBreakpoint;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BSREDIRECT)) {
      return bcBSRedirect;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CALLACC)) {
      return bcCallAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CALLEXTR)) {
      return bcCallExtR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CALLR)) {
      return bcCallR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CALLSI)) {
      return bcCallSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CLOSE)) {
      return bcClose;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_COPYFPI)) {
      return bcCopyFPI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CREATE)) {
      return bcCreate;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CREATEN)) {
      return bcCreateN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ELSE)) {
      return bcElse;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ELSEFLAG)) {
      return bcElseFlag;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ELSER)) {
      return bcElseR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ELSESI)) {
      return bcElseSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_EVALR)) {
      return bcEvalR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_EXCLUDE)) {
      return bcExclude;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_GET)) {
      return bcGet;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_GETLEN)) {
      return bcGetLen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_HOOK)) {
      return bcHook;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_IACCCOPYR)) {
      return bcIAccCopyR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_IACCFILLR)) {
      return bcIAccFillR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_INCLUDE)) {
      return bcInclude;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_INCFI)) {
      return bcIncFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_INCSI)) {
      return bcIncSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_INIT)) {
      return bcInit;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_JUMP)) {
      return bcJump;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_JUMPACC)) {
      return bcJumpAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_JUMPACCN)) {
      return bcJumpAccN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCADDACCI)) {
      return bcMccAddAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCADDM)) {
      return bcMccAddM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYACCI)) {
      return bcMccCopyAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYFI)) {
      return bcMccCopyFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYM)) {
      return bcMccCopyM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYPRMFI)) {
      return bcMccCopyPrmFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYSI)) {
      return bcMccCopySI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYSUBJ)) {
      return bcMccCopySubj;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCCOPYVERB)) {
      return bcMccCopyVerb;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCELSE)) {
      return bcMccElse;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCELSEACC)) {
      return bcMccElseAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCELSEACCI)) {
      return bcMccElseAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCREVERSE)) {
      return bcMccReverse;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCTHEN)) {
      return bcMccThen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCTHENACC)) {
      return bcMccThenAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCCTHENACCI)) {
      return bcMccThenAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_NOP)) {
      return bcNop;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_NWRITE)) {
      return bcNWrite;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_OPEN)) {
      return bcOpen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POP)) {
      return bcPop;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPACC)) {
      return bcPopAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPACCI)) {
      return bcPopAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPFI)) {
      return bcPopFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPMCC)) {
      return bcPopMcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPN)) {
      return bcPopN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPSELF)) {
      return bcPopSelf;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPSELFI)) {
      return bcPopSelfI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPSI)) {
      return bcPopSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPSI)) {
      return bcPopSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHACC)) {
      return bcPushAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHACCI)) {
      return bcPushAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHFI)) {
      return bcPushFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHFPI)) {
      return bcPushFPI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHI)) {
      return bcPushI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHN)) {
      return bcPushN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHR)) {
      return bcPushR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHMCC)) {
      return bcPushMcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHSELF)) {
      return bcPushSelf;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHSELFI)) {
      return bcPushSelfI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHSI)) {
      return bcPushSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHSPI)) {
      return bcPushSPI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_QUIT)) {
      return bcQuit;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_QUITMCC)) {
      return bcQuitMcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_QUITN)) {
      return bcQuitN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_RCALLM)) {
      return bcRCallM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_RCALLN)) {
      return bcRCallN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_RESERVE)) {
      return bcReserve;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_RESTORE)) {
      return bcRestore;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_RETHROW)) {
      return bcRethrow;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_SET)) {
      return bcSet;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_SWAPSI)) {
      return bcSwapSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_THEN)) {
      return bcThen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_THENFLAG)) {
      return bcThenFlag;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_THENR)) {
      return bcThenR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_THENSI)) {
      return bcThenSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_THROW)) {
      return bcThrow;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_UNHOOK)) {
      return bcUnhook;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_WRITEACC)) {
      return bcWriteAcc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XACCCOPYFPI)) {
      return bcXAccCopyFPI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XACCSAVEFI)) {
      return bcXAccSaveFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XMCCCOPYM)) {
      return bcXMccCopyM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XPOPACCI)) {
      return bcXPopAccI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XPUSHFPI)) {
      return bcXPushFPI;
   }
   else return bcNone;
}

const wchar16_t* ByteCodeCompiler :: decode(ByteCode code, wchar16_t* s)
{
   switch (code) {
      case bcAccAddN:
         StringHelper::copy(s, OPCODE_ACCADDN, 1 + strlen(OPCODE_ACCADDN));
         break;
      case bcAccBoxN:
         StringHelper::copy(s, OPCODE_ACCBOXN, 1 + strlen(OPCODE_ACCBOXN));
         break;
      case bcAccCopyFPI:
         StringHelper::copy(s, OPCODE_ACCCOPYFPI, 1 + strlen(OPCODE_ACCCOPYFPI));
         break;
      case bcAccCopyN:
         StringHelper::copy(s, OPCODE_ACCCOPYN, 1 + strlen(OPCODE_ACCCOPYN));
         break;
      case bcAccCopySelf:
         StringHelper::copy(s, OPCODE_ACCCOPYSELF, 1 + strlen(OPCODE_ACCCOPYSELF));
         break;
      case bcAccCopyR:
         StringHelper::copy(s, OPCODE_ACCCOPYR, 1 + strlen(OPCODE_ACCCOPYR));
         break;
      case bcAccCreate:
         StringHelper::copy(s, OPCODE_ACCCREATE, 1 + strlen(OPCODE_ACCCREATE));
         break;
      case bcAccCreateN:
         StringHelper::copy(s, OPCODE_ACCCREATEN, 1 + strlen(OPCODE_ACCCREATEN));
         break;
      case bcAccGetFI:
         StringHelper::copy(s, OPCODE_ACCGETFI, 1 + strlen(OPCODE_ACCGETFI));
         break;
      case bcAccGetSI:
         StringHelper::copy(s, OPCODE_ACCGETSI, 1 + strlen(OPCODE_ACCGETSI));
         break;
      case bcAccLoadAccI:
         StringHelper::copy(s, OPCODE_ACCLOADACCI, 1 + strlen(OPCODE_ACCLOADACCI));
         break;
      case bcAccLoadFI:
         StringHelper::copy(s, OPCODE_ACCLOADFI, 1 + strlen(OPCODE_ACCLOADFI));
         break;
      case bcAccLoadR:
         StringHelper::copy(s, OPCODE_ACCLOADR, 1 + strlen(OPCODE_ACCLOADR));
         break;
      case bcAccLoadSelfI:
         StringHelper::copy(s, OPCODE_ACCLOADSELFI, 1 + strlen(OPCODE_ACCLOADSELFI));
         break;
      case bcAccLoadSI:
         StringHelper::copy(s, OPCODE_ACCLOADSI, 1 + strlen(OPCODE_ACCLOADSI));
         break;
      case bcAccSaveFI:
         StringHelper::copy(s, OPCODE_ACCSAVEFI, 1 + strlen(OPCODE_ACCSAVEFI));
         break;
      case bcAccSaveR:
         StringHelper::copy(s, OPCODE_ACCSAVER, 1 + strlen(OPCODE_ACCSAVER));
         break;
      case bcAccSaveSelfI:
         StringHelper::copy(s, OPCODE_ACCSAVESELFI, 1 + strlen(OPCODE_ACCSAVESELFI));
         break;
      case bcAccSaveSI:
         StringHelper::copy(s, OPCODE_ACCSAVESI, 1 + strlen(OPCODE_ACCSAVESI));
         break;
      case bcAccSwapSI:
         StringHelper::copy(s, OPCODE_ACCSWAPSI, 1 + strlen(OPCODE_ACCSWAPSI));
         break;
      case bcBreakpoint:
         StringHelper::copy(s, OPCODE_BREAKPOINT, 1 + strlen(OPCODE_BREAKPOINT));
         break;
      case bcBSRedirect:
         StringHelper::copy(s, OPCODE_BSREDIRECT, 1 + strlen(OPCODE_BSREDIRECT));
         break;
      case bcCallAcc:
         StringHelper::copy(s, OPCODE_CALLACC, 1 + strlen(OPCODE_CALLACC));
         break;
      case bcCallExtR:
         StringHelper::copy(s, OPCODE_CALLEXTR, 1 + strlen(OPCODE_CALLEXTR));
         break;
      case bcCallR:
         StringHelper::copy(s, OPCODE_CALLR, 1 + strlen(OPCODE_CALLR));
         break;
      case bcCallSI:
         StringHelper::copy(s, OPCODE_CALLSI, 1 + strlen(OPCODE_CALLSI));
         break;
      case bcClose:
         StringHelper::copy(s, OPCODE_CLOSE, 1 + strlen(OPCODE_CLOSE));
         break;
      case bcCopyFPI:
         StringHelper::copy(s, OPCODE_COPYFPI, 1 + strlen(OPCODE_COPYFPI));
         break;
      case bcCreate:
         StringHelper::copy(s, OPCODE_CREATE, 1 + strlen(OPCODE_CREATE));
         break;
      case bcCreateN:
         StringHelper::copy(s, OPCODE_CREATEN, 1 + strlen(OPCODE_CREATEN));
         break;
      case bcElse:
         StringHelper::copy(s, OPCODE_ELSE, 1 + strlen(OPCODE_ELSE));
         break;
      case bcElseFlag:
         StringHelper::copy(s, OPCODE_ELSEFLAG, 1 + strlen(OPCODE_ELSEFLAG));
         break;
      case bcElseR:
         StringHelper::copy(s, OPCODE_ELSER, 1 + strlen(OPCODE_ELSER));
         break;
      case bcElseSI:
         StringHelper::copy(s, OPCODE_ELSESI, 1 + strlen(OPCODE_ELSESI));
         break;
      case bcEvalR:
         StringHelper::copy(s, OPCODE_EVALR, 1 + strlen(OPCODE_EVALR));
         break;
      case bcExclude:
         StringHelper::copy(s, OPCODE_EXCLUDE, 1 + strlen(OPCODE_EXCLUDE));
         break;
      case bcGet:
         StringHelper::copy(s, OPCODE_GET, 1 + strlen(OPCODE_GET));
         break;
      case bcGetLen:
         StringHelper::copy(s, OPCODE_GETLEN, 1 + strlen(OPCODE_GETLEN));
         break;
      case bcHook:
         StringHelper::copy(s, OPCODE_HOOK, 1 + strlen(OPCODE_HOOK));
         break;
      case bcIAccCopyR:
         StringHelper::copy(s, OPCODE_IACCCOPYR, 1 + strlen(OPCODE_IACCCOPYR));
         break;
      case bcIAccFillR:
         StringHelper::copy(s, OPCODE_IACCFILLR, 1 + strlen(OPCODE_IACCFILLR));
         break;
      case bcInclude:
         StringHelper::copy(s, OPCODE_INCLUDE, 1 + strlen(OPCODE_INCLUDE));
         break;
      case bcIncFI:
         StringHelper::copy(s, OPCODE_INCFI, 1 + strlen(OPCODE_INCFI));
         break;
      case bcIncSI:
         StringHelper::copy(s, OPCODE_INCSI, 1 + strlen(OPCODE_INCSI));
         break;
      case bcInit:
         StringHelper::copy(s, OPCODE_INIT, 1 + strlen(OPCODE_INIT));
         break;
      case bcJump:
         StringHelper::copy(s, OPCODE_JUMP, 1 + strlen(OPCODE_JUMP));
         break;
      case bcJumpAcc:
         StringHelper::copy(s, OPCODE_JUMPACC, 1 + strlen(OPCODE_JUMPACC));
         break;
      case bcJumpAccN:
         StringHelper::copy(s, OPCODE_JUMPACCN, 1 + strlen(OPCODE_JUMPACCN));
         break;
      case bcMccAddAccI:
         StringHelper::copy(s, OPCODE_MCCADDACCI, 1 + strlen(OPCODE_MCCADDACCI));
         break;
      case bcMccAddM:
         StringHelper::copy(s, OPCODE_MCCADDM, 1 + strlen(OPCODE_MCCADDM));
         break;
      case bcMccCopyAccI:
         StringHelper::copy(s, OPCODE_MCCCOPYACCI, 1 + strlen(OPCODE_MCCCOPYACCI));
         break;
      case bcMccCopyFI:
         StringHelper::copy(s, OPCODE_MCCCOPYFI, 1 + strlen(OPCODE_MCCCOPYFI));
         break;
      case bcMccCopyM:
         StringHelper::copy(s, OPCODE_MCCCOPYM, 1 + strlen(OPCODE_MCCCOPYM));
         break;
      case bcMccCopyPrmFI:
         StringHelper::copy(s, OPCODE_MCCCOPYPRMFI, 1 + strlen(OPCODE_MCCCOPYPRMFI));
         break;
      case bcMccCopySI:
         StringHelper::copy(s, OPCODE_MCCCOPYSI, 1 + strlen(OPCODE_MCCCOPYSI));
         break;
      case bcMccCopySubj:
         StringHelper::copy(s, OPCODE_MCCCOPYSUBJ, 1 + strlen(OPCODE_MCCCOPYSUBJ));
         break;
      case bcMccCopyVerb:
         StringHelper::copy(s, OPCODE_MCCCOPYVERB, 1 + strlen(OPCODE_MCCCOPYVERB));
         break;
      case bcMccElse:
         StringHelper::copy(s, OPCODE_MCCELSE, 1 + strlen(OPCODE_MCCELSE));
         break;
      case bcMccElseAcc:
         StringHelper::copy(s, OPCODE_MCCELSEACC, 1 + strlen(OPCODE_MCCELSEACC));
         break;
      case bcMccElseAccI:
         StringHelper::copy(s, OPCODE_MCCELSEACCI, 1 + strlen(OPCODE_MCCELSEACCI));
         break;
      case bcMccReverse:
         StringHelper::copy(s, OPCODE_MCCREVERSE, 1 + strlen(OPCODE_MCCREVERSE));
         break;
      case bcMccThen:
         StringHelper::copy(s, OPCODE_MCCTHEN, 1 + strlen(OPCODE_MCCTHEN));
         break;
      case bcMccThenAcc:
         StringHelper::copy(s, OPCODE_MCCTHENACC, 1 + strlen(OPCODE_MCCTHENACC));
         break;
      case bcMccThenAccI:
         StringHelper::copy(s, OPCODE_MCCTHENACCI, 1 + strlen(OPCODE_MCCTHENACCI));
         break;
      case bcNop:
         StringHelper::copy(s, OPCODE_NOP, 1 + strlen(OPCODE_NOP));
         break;
      case bcNWrite:
         StringHelper::copy(s, OPCODE_NWRITE, 1 + strlen(OPCODE_NWRITE));
         break;
      case bcOpen:
         StringHelper::copy(s, OPCODE_OPEN, 1 + strlen(OPCODE_OPEN));
         break;
      case bcPop:
         StringHelper::copy(s, OPCODE_POP, 1 + strlen(OPCODE_POP));
         break;
      case bcPopAcc:
         StringHelper::copy(s, OPCODE_POPACC, 1 + strlen(OPCODE_POPACC));
         break;
      case bcPopAccI:
         StringHelper::copy(s, OPCODE_POPACCI, 1 + strlen(OPCODE_POPACCI));
         break;
      case bcPopFI:
         StringHelper::copy(s, OPCODE_POPFI, 1 + strlen(OPCODE_POPFI));
         break;
      case bcPopN:
         StringHelper::copy(s, OPCODE_POPN, 1 + strlen(OPCODE_POPN));
         break;
      case bcPopMcc:
         StringHelper::copy(s, OPCODE_POPMCC, 1 + strlen(OPCODE_POPMCC));
         break;
      case bcPopSelf:
         StringHelper::copy(s, OPCODE_POPSELF, 1 + strlen(OPCODE_POPSELF));
         break;
      case bcPopSelfI:
         StringHelper::copy(s, OPCODE_POPSELFI, 1 + strlen(OPCODE_POPSELFI));
         break;
      case bcPopSI:
         StringHelper::copy(s, OPCODE_POPSI, 1 + strlen(OPCODE_POPSI));
         break;
      case bcPushAccI:
         StringHelper::copy(s, OPCODE_PUSHACCI, 1 + strlen(OPCODE_PUSHACCI));
         break;
      case bcPushAcc:
         StringHelper::copy(s, OPCODE_PUSHACC, 1 + strlen(OPCODE_PUSHACC));
         break;
      case bcPushFI:
         StringHelper::copy(s, OPCODE_PUSHFI, 1 + strlen(OPCODE_PUSHFI));
         break;
      case bcPushFPI:
         StringHelper::copy(s, OPCODE_PUSHFPI, 1 + strlen(OPCODE_PUSHFPI));
         break;
      case bcPushI:
         StringHelper::copy(s, OPCODE_PUSHI, 1 + strlen(OPCODE_PUSHI));
         break;
      case bcPushMcc:
         StringHelper::copy(s, OPCODE_PUSHMCC, 1 + strlen(OPCODE_PUSHMCC));
         break;
      case bcPushN:
         StringHelper::copy(s, OPCODE_PUSHN, 1 + strlen(OPCODE_PUSHN));
         break;
      case bcPushR:
         StringHelper::copy(s, OPCODE_PUSHR, 1 + strlen(OPCODE_PUSHR));
         break;
      case bcPushSelf:
         StringHelper::copy(s, OPCODE_PUSHSELF, 1 + strlen(OPCODE_PUSHSELF));
         break;
      case bcPushSelfI:
         StringHelper::copy(s, OPCODE_PUSHSELFI, 1 + strlen(OPCODE_PUSHSELFI));
         break;
      case bcPushSI:
         StringHelper::copy(s, OPCODE_PUSHSI, 1 + strlen(OPCODE_PUSHSI));
         break;
      case bcPushSPI:
         StringHelper::copy(s, OPCODE_PUSHSPI, 1 + strlen(OPCODE_PUSHSPI));
         break;
      case bcQuit:
         StringHelper::copy(s, OPCODE_QUIT, 1 + strlen(OPCODE_QUIT));
         break;
      case bcQuitN:
         StringHelper::copy(s, OPCODE_QUITN, 1 + strlen(OPCODE_QUITN));
         break;
      case bcQuitMcc:
         StringHelper::copy(s, OPCODE_QUITMCC, 1 + strlen(OPCODE_QUITMCC));
         break;
      case bcRCallM:
         StringHelper::copy(s, OPCODE_RCALLM, 1 + strlen(OPCODE_RCALLM));
         break;
      case bcRCallN:
         StringHelper::copy(s, OPCODE_RCALLM, 1 + strlen(OPCODE_RCALLN));
         break;
      case bcReserve:
         StringHelper::copy(s, OPCODE_RESERVE, 1 + strlen(OPCODE_RESERVE));
         break;
      case bcRestore:
         StringHelper::copy(s, OPCODE_RESTORE, 1 + strlen(OPCODE_RESTORE));
         break;
      case bcRethrow:
         StringHelper::copy(s, OPCODE_RETHROW, 1 + strlen(OPCODE_RETHROW));
         break;
      case bcSet:
         StringHelper::copy(s, OPCODE_SET, 1 + strlen(OPCODE_SET));
         break;
      case bcSwapSI:
         StringHelper::copy(s, OPCODE_SWAPSI, 1 + strlen(OPCODE_SWAPSI));
         break;
      case bcThen:
         StringHelper::copy(s, OPCODE_THEN, 1 + strlen(OPCODE_THEN));
         break;
      case bcThenFlag:
         StringHelper::copy(s, OPCODE_THENFLAG, 1 + strlen(OPCODE_THENFLAG));
         break;
      case bcThenR:
         StringHelper::copy(s, OPCODE_THENR, 1 + strlen(OPCODE_THENR));
         break;
      case bcThenSI:
         StringHelper::copy(s, OPCODE_THENSI, 1 + strlen(OPCODE_THENSI));
         break;
      case bcThrow:
         StringHelper::copy(s, OPCODE_THROW, 1 + strlen(OPCODE_THROW));
         break;
      case bcUnhook:
         StringHelper::copy(s, OPCODE_UNHOOK, 1 + strlen(OPCODE_UNHOOK));
         break;
      case bcWriteAcc:
         StringHelper::copy(s, OPCODE_WRITEACC, 1 + strlen(OPCODE_WRITEACC));
         break;
      case bcXAccCopyFPI:
         StringHelper::copy(s, OPCODE_XACCCOPYFPI, 1 + strlen(OPCODE_XACCCOPYFPI));
         break;
      case bcXAccSaveFI:
         StringHelper::copy(s, OPCODE_XACCSAVEFI, 1 + strlen(OPCODE_XACCSAVEFI));
         break;
      case bcXMccCopyM:
         StringHelper::copy(s, OPCODE_MCCCOPYM, 1 + strlen(OPCODE_XMCCCOPYM));
         break;
      case bcXPopAccI:
         StringHelper::copy(s, OPCODE_XPOPACCI, 1 + strlen(OPCODE_XPOPACCI));
         break;
      case bcXPushFPI:
         StringHelper::copy(s, OPCODE_XPUSHFPI, 1 + strlen(OPCODE_XPUSHFPI));
         break;
      default:
         StringHelper::copy(s, OPCODE_UNKNOWN, 1 + strlen(OPCODE_UNKNOWN));
   }

   return s;
}
