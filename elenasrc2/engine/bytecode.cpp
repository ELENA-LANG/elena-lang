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

#define OPCODE_AADD         "aadd"
#define OPCODE_ACALLVI      "acallvi"
#define OPCODE_ACOPYB       "acopyb"
#define OPCODE_ACOPYF       "acopyf"
#define OPCODE_ACOPYN       "acopyn"
#define OPCODE_ACOPYR       "acopyr"
#define OPCODE_ACOPYS       "acopys"
//#define OPCODE_ACCCREATE    "acccreate"
//#define OPCODE_ACCGETFI     "accgetfi"
//#define OPCODE_ACCGETSI     "accgetsi"
#define OPCODE_AELSER       "aelser"
#define OPCODE_AJUMPVI      "ajumpvi"
#define OPCODE_ALOADAI      "aloadai"
#define OPCODE_ALOADD       "aloadd"
#define OPCODE_ALOADFI      "aloadfi"
#define OPCODE_AMUL         "amul"
#define OPCODE_AELSE        "aelse"
#define OPCODE_AELSESI      "aelsesi"
#define OPCODE_ALOADBI      "aloadbi"
#define OPCODE_ALOADR       "aloadr"
#define OPCODE_ASAVER       "asaver"
#define OPCODE_ATHEN        "athen"
#define OPCODE_ATHENR       "athenr"
#define OPCODE_ATHENSI      "athensi"
#define OPCODE_ALOADSI      "aloadsi"
#define OPCODE_ASAVEBI      "asavebi"
#define OPCODE_ASAVEFI      "asavefi"
#define OPCODE_ASAVESI      "asavesi"
#define OPCODE_ASWAPSI      "aswapsi"
#define OPCODE_AXSETR       "axsetr"
#define OPCODE_BCOPYA       "bcopya"
#define OPCODE_BOX          "box"
#define OPCODE_BREAKPOINT   "breakpoint"
#define OPCODE_BSREDIRECT   "bsredirect"
#define OPCODE_BSTEST       "bstest"
#define OPCODE_CALLEXTR     "callextr"
#define OPCODE_CALLR        "callr"
#define OPCODE_CLOSE        "close"
#define OPCODE_CREATE       "create"
#define OPCODE_CREATEN      "createn"
#define OPCODE_DADDAI       "daddai"
#define OPCODE_DADDSI       "daddsi"
#define OPCODE_DLOADAI      "dloadai"
#define OPCODE_DLOADFI      "dloadfi"
#define OPCODE_DLOADSI      "dloadsi"
#define OPCODE_DCOPY        "dcopy"
#define OPCODE_DCREATE      "dcreate"
#define OPCODE_DCREATEN     "dcreaten"
#define OPCODE_DDEC         "ddec"
#define OPCODE_DELSE        "delse"
#define OPCODE_DELSEN       "delsen"
#define OPCODE_DINC         "dinc"
#define OPCODE_DSAVEAI      "dsaveai"
#define OPCODE_DSAVEFI      "dsavefi"
#define OPCODE_DSAVESI      "dsavesi"
#define OPCODE_DSUBAI       "dsubai"
#define OPCODE_DSUBSI       "dsubsi"
#define OPCODE_DTHEN        "dthen"
#define OPCODE_DTHENN       "dthenn"
#define OPCODE_ELSEFLAG     "elseflag"
#define OPCODE_EVALR        "evalr"
#define OPCODE_EXCLUDE      "exclude"
#define OPCODE_GET          "get"
#define OPCODE_GETLEN       "getlen"
#define OPCODE_HOOK         "hook"
#define OPCODE_IAXCOPYR     "iaxcopyr"
//#define OPCODE_IACCFILLR    "iaccfillr"
#define OPCODE_INCLUDE      "include"
//#define OPCODE_INCFI        "incfi"
//#define OPCODE_INCSI        "incsi"
#define OPCODE_JUMP         "jump"
//#define OPCODE_JUMPACC      "jumpacc"
#define OPCODE_MADDAI       "maddai"
#define OPCODE_MADD         "madd"
#define OPCODE_MCOPY        "mcopy"
#define OPCODE_MCOPYSUBJ    "mcopysubj"
#define OPCODE_MCOPYVERB    "mcopyverb"
#define OPCODE_MELSE        "melse"
#define OPCODE_MLOADAI      "mloadai"
#define OPCODE_MLOADFI      "mloadfi"
#define OPCODE_MLOADSI      "mloadsi"
//#define OPCODE_MCCELSEACC   "mccelseacc"
#define OPCODE_MELSEAI      "melseai"
//#define OPCODE_MCCREVERSE   "mccreverse"
#define OPCODE_MQUIT        "mquit"
#define OPCODE_MSAVEPARAMS  "msaveparams"
#define OPCODE_MTHEN        "mthen"
//#define OPCODE_MCCTHENACC   "mccthenacc"
//#define OPCODE_MCCTHENACCI  "mccthenacci"
#define OPCODE_NBOX         "nbox"
#define OPCODE_NOP          "nop"
#define OPCODE_NEXT         "next"
#define OPCODE_OPEN         "open"
#define OPCODE_POP          "pop"
#define OPCODE_POPA         "popa"
#define OPCODE_POPB         "popb"
#define OPCODE_POPBI        "popbi"
#define OPCODE_POPAI        "popai"
#define OPCODE_POPM          "popm"
#define OPCODE_POPFI        "popfi"
#define OPCODE_POPI         "popi"
#define OPCODE_POPSI        "popsi"
//#define OPCODE_POPSPI       "popspi"
#define OPCODE_PUSHA        "pusha"
#define OPCODE_PUSHAI       "pushai"
#define OPCODE_PUSHB        "pushb"
#define OPCODE_PUSHBI       "pushbi"
#define OPCODE_PUSHF        "pushf"
#define OPCODE_PUSHFI       "pushfi"
//#define OPCODE_PUSHI        "pushi"
#define OPCODE_PUSHM        "pushm"
#define OPCODE_PUSHN        "pushn"
#define OPCODE_PUSHR        "pushr"
#define OPCODE_PUSHSI       "pushsi"
//#define OPCODE_PUSHSPI      "pushspi"
#define OPCODE_QUIT         "quit"
#define OPCODE_QUITN        "quitn"
//#define OPCODE_RCALLN       "rcalln"
#define OPCODE_RESERVE      "reserve"
#define OPCODE_RESTORE      "restore"
//#define OPCODE_RETHROW      "rethrow"
#define OPCODE_SET          "set"
#define OPCODE_SCALLVI      "scallvi"
#define OPCODE_SCOPYF       "scopyf"
#define OPCODE_SWAPSI       "swapsi"
#define OPCODE_TEST         "test"
#define OPCODE_TESTFLAG     "testflag"
#define OPCODE_THROW        "throw"
#define OPCODE_UNHOOK       "unhook"
#define OPCODE_WSTEST       "wstest"
#define OPCODE_AXCOPYF      "axcopyf"
//#define OPCODE_XACCSAVEFI   "x_accsavefi"
//#define OPCODE_XMCCCOPYM    "x_mcccopym"
#define OPCODE_XCALLRM      "xcallrm"
#define OPCODE_XPOPAI       "xpopai"
#define OPCODE_XPUSHF       "xpushf"

#define OPCODE_UNKNOWN      "unknown"

#define FUNC_ABS            "abs"
#define FUNC_ADD            "add"
#define FUNC_ADDSTR         "addstr"
#define FUNC_AND            "and"
#define FUNC_COPY           "copy"
#define FUNC_COPYBUF        "copybuf"
#define FUNC_COPYINT        "copyint"
#define FUNC_COPYLONG       "copylong"
#define FUNC_COPYREAL       "copyreal"
#define FUNC_COPYSTR        "copystr"
#define FUNC_CREATE         "create"
#define FUNC_DELETESTR      "deletestr" 
#define FUNC_DIV            "div"
#define FUNC_EQUAL          "equal"
#define FUNC_EVAL           "eval"
#define FUNC_EXP            "exp"
#define FUNC_GETAT          "getat"
#define FUNC_GETBUF         "getbuf"
#define FUNC_GETINT         "getint"
#define FUNC_GETLEN         "getlen"
#define FUNC_GETLENZ        "getlenz"
#define FUNC_GETWORD        "getword"
#define FUNC_INC            "inc"
#define FUNC_INDEXOF        "indexof"
#define FUNC_INDEXOFSTR     "indexofstr"
#define FUNC_INDEXOFWORD    "indexofword"
#define FUNC_LESS           "less"
#define FUNC_LN             "ln"
#define FUNC_LOAD           "load"
#define FUNC_LOADNAME       "loadname"
#define FUNC_LOADSTR        "loadstr"
#define FUNC_MUL            "mul"
#define FUNC_NOT            "not"
#define FUNC_NOTGREATER     "notgreater"
#define FUNC_OR             "or"
#define FUNC_RESERVE        "reserve"
#define FUNC_RNDNEW         "rndnew"
#define FUNC_RNDNEXT        "rndnext"
#define FUNC_ROUND          "round"
#define FUNC_SAVE           "save"
#define FUNC_SETAT          "setat"
#define FUNC_SETBUF         "setbuf"
#define FUNC_SETINT         "setint"
#define FUNC_SETLEN         "setlen"
#define FUNC_SETWORD        "setword"
#define FUNC_SHIFT          "shift"
#define FUNC_SUB            "sub"
#define FUNC_XOR            "xor"

using namespace _ELENA_;

// --- help functions ---
inline bool IsJump(ByteCode code)
{
   switch(code) {
      case bcJump:
      case bcNext:
      case bcAElse:
      case bcAThen:
      case bcWSTest:
      case bcBSTest:
      case bcTest:
      case bcDElse:
      case bcDThen:
      case bcDElseN:
      case bcDThenN:
//      case bcMccElseAcc:
//      case bcMccThenAcc:
      case bcAElseR:
      case bcAThenR:
      case bcMElse:
      case bcMThen:
      case bcAElseSI:
      case bcAThenSI:
      case bcMElseAI:
//      case bcMccThenAccI:
      case bcElseFlag:
      case bcTestFlag:
         return true;
      default:
         return false;
      }
}

bool IsRCode(ByteCode code)
{
   switch(code) {
      case bcPushR:
      case bcEvalR:
      case bcCallR:
      case bcALoadR:
      case bcASaveR:
      case bcAXSetR:
      case bcACopyR:
      case bcCreate:
      case bcCreateN:
      case bcIAXCopyR:
      case bcNBox:
      case bcBox:
      case bcXCallRM:
         return true;
      default:
         return false;
   }
}

inline ref_t importRef(_Module* sour, size_t ref, _Module* dest)
{
   int mask = ref & mskAnyRef;

   const wchar16_t* referenceName = sour->resolveReference(ref & ~mskAnyRef);

   return dest->mapReference(referenceName) | mask;
}

// --- CommandTape ---

bool CommandTape :: import(ByteCommand& command, _Module* sour, _Module* dest)
{
   if (IsRCode(command.code)) {
      command.argument = importRef(sour, (ref_t)command.argument, dest);

      return true;
   }
   else return false;
}

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
         case bcAElse:
         case bcAThen:
         case bcWSTest:
         case bcBSTest:
         case bcTest:
         case bcDElse:
         case bcDThen:
         case bcDElseN:
         case bcDThenN:
//         case bcMccElseAcc:
//         case bcMccThenAcc:
         case bcAElseR:
         case bcAThenR:
         case bcNext:
         case bcMElse:
         case bcMThen:
         case bcAElseSI:
         case bcAThenSI:
         case bcElseFlag:
         case bcTestFlag:
         case bcMElseAI:
//         case bcMccThenAccI:
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
            //case bcJumpAcc:
            case bcQuit:
            case bcMQuit:
            case bcQuitN:
            case bcAJumpVI:
               blocks.add(index + 1, 0);
               break;
            case bcJump:
               blocks.add(index + 1, 0);
            case bcAElse:
            case bcAThen:              
            case bcWSTest:
            case bcBSTest:
            case bcTest:
            case bcDElse:
            case bcDThen:              
            case bcDElseN:
            case bcDThenN:              
            //case bcMccElseAcc:
            //case bcMccThenAcc:
            case bcAElseR:
            case bcAThenR:     
            case bcNext:
            case bcMElse:
            case bcMThen:
            case bcAElseSI:
            case bcAThenSI:
            case bcMElseAI:
            //case bcMccThenAccI:
            case bcElseFlag:
            case bcTestFlag:
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
   //if (ConstantIdentifier::compare(s, OPCODE_AADD)) {
   //   return bcAAdd;
   //}
   //else if (ConstantIdentifier::compare(s, OPCODE_ACOPYN)) {
   //   return bcACopyN;
   //}
   /*else */if (ConstantIdentifier::compare(s, OPCODE_ACALLVI)) {
      return bcACallVI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACOPYF)) {
      return bcACopyF;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACOPYR)) {
      return bcACopyR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACOPYS)) {
      return bcACopyS;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_ACCCREATE)) {
//      return bcAccCreate;
//   }
//   else if (ConstantIdentifier::compare(s, OPCODE_ACCGETFI)) {
//      return bcAccGetFI;
//   }
//   else if (ConstantIdentifier::compare(s, OPCODE_ACCGETSI)) {
//      return bcAccGetSI;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_AJUMPVI)) {
      return bcAJumpVI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ALOADAI)) {
      return bcALoadAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ALOADD)) {
      return bcALoadD;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ALOADFI)) {
      return bcALoadFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_AELSE)) {
      return bcAElse;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_AELSER)) {
      return bcAElseR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_AELSESI)) {
      return bcAElseSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ALOADBI)) {
      return bcALoadBI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ALOADR)) {
      return bcALoadR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ALOADSI)) {
      return bcALoadSI;
   }
   //else if (ConstantIdentifier::compare(s, OPCODE_AMUL)) {
   //   return bcAMul;
   //}
   else if (ConstantIdentifier::compare(s, OPCODE_ASAVER)) {
      return bcASaveR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ATHEN)) {
      return bcAThen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ATHENSI)) {
      return bcAThenSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_AXCOPYF)) {
      return bcAXCopyF;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_AXSETR)) {
      return bcAXSetR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ACOPYB)) {
      return bcACopyB;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ASAVEBI)) {
      return bcASaveBI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ASAVEFI)) {
      return bcASaveFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ASAVESI)) {
      return bcASaveSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ASWAPSI)) {
      return bcASwapSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ATHENR)) {
      return bcAThenR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BOX)) {
      return bcBox;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BREAKPOINT)) {
      return bcBreakpoint;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BCOPYA)) {
      return bcBCopyA;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BSREDIRECT)) {
      return bcBSRedirect;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_BSTEST)) {
      return bcBSTest;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CALLEXTR)) {
      return bcCallExtR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CALLR)) {
      return bcCallR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CLOSE)) {
      return bcClose;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CREATE)) {
      return bcCreate;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_CREATEN)) {
      return bcCreateN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DADDAI)) {
      return bcDAddAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DADDSI)) {
      return bcDAddSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DLOADAI)) {
      return bcDLoadAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DLOADFI)) {
      return bcDLoadFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DLOADSI)) {
      return bcDLoadSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DCOPY)) {
      return bcDCopy;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DDEC)) {
      return bcDDec;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DELSE)) {
      return bcDElse;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DELSEN)) {
      return bcDElseN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DINC)) {
      return bcDInc;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DSAVEAI)) {
      return bcDSaveAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DSAVEFI)) {
      return bcDSaveFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DSAVESI)) {
      return bcDSaveSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DSUBAI)) {
      return bcDSubAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DSUBSI)) {
      return bcDSubSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DTHEN)) {
      return bcDThen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_DTHENN)) {
      return bcDThenN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_ELSEFLAG)) {
      return bcElseFlag;
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
   else if (ConstantIdentifier::compare(s, OPCODE_IAXCOPYR)) {
      return bcIAXCopyR;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_IACCFILLR)) {
//      return bcIAccFillR;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_INCLUDE)) {
      return bcInclude;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_INCFI)) {
//      return bcIncFI;
//   }
//   else if (ConstantIdentifier::compare(s, OPCODE_INCSI)) {
//      return bcIncSI;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_JUMP)) {
      return bcJump;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_JUMPACC)) {
//      return bcJumpAcc;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_MADDAI)) {
      return bcMAddAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MADD)) {
      return bcMAdd;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCOPY)) {
      return bcMCopy;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCOPYSUBJ)) {
      return bcMCopySubj;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MCOPYVERB)) {
      return bcMCopyVerb;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MELSE)) {
      return bcMElse;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MLOADAI)) {
      return bcMLoadAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MLOADFI)) {
      return bcMLoadFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MLOADSI)) {
      return bcMLoadSI;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_MCCELSEACC)) {
//      return bcMccElseAcc;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_MELSEAI)) {
      return bcMElseAI;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_MCCREVERSE)) {
//      return bcMccReverse;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_MQUIT)) {
      return bcMQuit;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MSAVEPARAMS)) {
      return bcMSaveParams;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MTHEN)) {
      return bcMThen;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_MCCTHENACC)) {
//      return bcMccThenAcc;
//   }
//   else if (ConstantIdentifier::compare(s, OPCODE_MCCTHENACCI)) {
//      return bcMccThenAccI;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_NBOX)) {
      return bcNBox;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_NOP)) {
      return bcNop;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_NEXT)) {
      return bcNext;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_OPEN)) {
      return bcOpen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POP)) {
      return bcPop;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPA)) {
      return bcPopA;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPAI)) {
      return bcPopAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPFI)) {
      return bcPopFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPM)) {
      return bcPopM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPB)) {
      return bcPopB;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPBI)) {
      return bcPopBI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPI)) {
      return bcPopI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_POPSI)) {
      return bcPopSI;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_POPSI)) {
//      return bcPopSI;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHA)) {
      return bcPushA;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHAI)) {
      return bcPushAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHB)) {
      return bcPushB;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHBI)) {
      return bcPushBI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHF)) {
      return bcPushF;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHFI)) {
      return bcPushFI;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_PUSHI)) {
//      return bcPushI;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHN)) {
      return bcPushN;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHR)) {
      return bcPushR;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHM)) {
      return bcPushM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_PUSHSI)) {
      return bcPushSI;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_PUSHSPI)) {
//      return bcPushSPI;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_QUIT)) {
      return bcQuit;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_QUITN)) {
      return bcQuitN;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_RCALLN)) {
//      return bcRCallN;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_RESERVE)) {
      return bcReserve;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_RESTORE)) {
      return bcRestore;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_RETHROW)) {
//      return bcRethrow;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_SET)) {
      return bcSet;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_WSTEST)) {
      return bcWSTest;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_SCALLVI)) {
      return bcSCallVI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_SCOPYF)) {
      return bcSCopyF;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_SWAPSI)) {
      return bcSwapSI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_TEST)) {
      return bcTest;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_TESTFLAG)) {
      return bcTestFlag;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_THROW)) {
      return bcThrow;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_UNHOOK)) {
      return bcUnhook;
   }
//   else if (ConstantIdentifier::compare(s, OPCODE_XACCSAVEFI)) {
//      return bcXAccSaveFI;
//   }
//   else if (ConstantIdentifier::compare(s, OPCODE_XMCCCOPYM)) {
//      return bcXMccCopyM;
//   }
   else if (ConstantIdentifier::compare(s, OPCODE_XPOPAI)) {
      return bcXPopAI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XCALLRM)) {
      return bcXCallRM;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_XPUSHF)) {
      return bcXPushF;
   }
   else return bcNone;
}

const wchar16_t* ByteCodeCompiler :: decode(ByteCode code, wchar16_t* s)
{
   switch (code) {
      //case bcAAdd:
      //   StringHelper::copy(s, OPCODE_AADD, 1 + strlen(OPCODE_AADD));
      //   break;
      case bcACallVI:
         StringHelper::copy(s, OPCODE_ACALLVI, 1 + strlen(OPCODE_ACALLVI));
         break;
      case bcACopyB:
         StringHelper::copy(s, OPCODE_ACOPYB, 1 + strlen(OPCODE_ACOPYB));
         break;
      case bcACopyF:
         StringHelper::copy(s, OPCODE_ACOPYF, 1 + strlen(OPCODE_ACOPYF));
         break;
      //case bcACopyN:
      //   StringHelper::copy(s, OPCODE_ACOPYN, 1 + strlen(OPCODE_ACOPYN));
      //   break;
      case bcACopyR:
         StringHelper::copy(s, OPCODE_ACOPYR, 1 + strlen(OPCODE_ACOPYR));
         break;
      case bcACopyS:
         StringHelper::copy(s, OPCODE_ACOPYS, 1 + strlen(OPCODE_ACOPYS));
         break;
//      case bcAccCreate:
//         StringHelper::copy(s, OPCODE_ACCCREATE, 1 + strlen(OPCODE_ACCCREATE));
//         break;
//      case bcAccGetFI:
//         StringHelper::copy(s, OPCODE_ACCGETFI, 1 + strlen(OPCODE_ACCGETFI));
//         break;
//      case bcAccGetSI:
//         StringHelper::copy(s, OPCODE_ACCGETSI, 1 + strlen(OPCODE_ACCGETSI));
//         break;
      case bcALoadAI:
         StringHelper::copy(s, OPCODE_ALOADAI, 1 + strlen(OPCODE_ALOADAI));
         break;
      case bcALoadD:
         StringHelper::copy(s, OPCODE_ALOADD, 1 + strlen(OPCODE_ALOADD));
         break;
      case bcALoadBI:
         StringHelper::copy(s, OPCODE_ALOADBI, 1 + strlen(OPCODE_ALOADBI));
         break;
      case bcALoadFI:
         StringHelper::copy(s, OPCODE_ALOADFI, 1 + strlen(OPCODE_ALOADFI));
         break;
      case bcAElse:
         StringHelper::copy(s, OPCODE_AELSE, 1 + strlen(OPCODE_AELSE));
         break;
      case bcAElseR:
         StringHelper::copy(s, OPCODE_AELSER, 1 + strlen(OPCODE_AELSER));
         break;
      case bcAElseSI:
         StringHelper::copy(s, OPCODE_AELSESI, 1 + strlen(OPCODE_AELSESI));
         break;
      case bcAJumpVI:
         StringHelper::copy(s, OPCODE_AJUMPVI, 1 + strlen(OPCODE_AJUMPVI));
         break;
      case bcALoadR:
         StringHelper::copy(s, OPCODE_ALOADR, 1 + strlen(OPCODE_ALOADR));
         break;
      //case bcAMul:
      //   StringHelper::copy(s, OPCODE_AMUL, 1 + strlen(OPCODE_AMUL));
      //   break;
      case bcASaveR:
         StringHelper::copy(s, OPCODE_ASAVER, 1 + strlen(OPCODE_ASAVER));
         break;
      case bcAThen:
         StringHelper::copy(s, OPCODE_ATHEN, 1 + strlen(OPCODE_ATHEN));
         break;
      case bcAThenR:
         StringHelper::copy(s, OPCODE_ATHENR, 1 + strlen(OPCODE_ATHENR));
         break;
      case bcAThenSI:
         StringHelper::copy(s, OPCODE_ATHENSI, 1 + strlen(OPCODE_ATHENSI));
         break;
      case bcAXCopyF:
         StringHelper::copy(s, OPCODE_AXCOPYF, 1 + strlen(OPCODE_AXCOPYF));
         break;
      case bcAXSetR:
         StringHelper::copy(s, OPCODE_AXSETR, 1 + strlen(OPCODE_AXSETR));
         break;
      case bcALoadSI:
         StringHelper::copy(s, OPCODE_ALOADSI, 1 + strlen(OPCODE_ALOADSI));
         break;
      case bcASaveBI:
         StringHelper::copy(s, OPCODE_ASAVEBI, 1 + strlen(OPCODE_ASAVEBI));
         break;
      case bcASaveFI:
         StringHelper::copy(s, OPCODE_ASAVEFI, 1 + strlen(OPCODE_ASAVEFI));
         break;
      case bcASaveSI:
         StringHelper::copy(s, OPCODE_ASAVESI, 1 + strlen(OPCODE_ASAVESI));
         break;
      case bcASwapSI:
         StringHelper::copy(s, OPCODE_ASWAPSI, 1 + strlen(OPCODE_ASWAPSI));
         break;
      case bcBCopyA:
         StringHelper::copy(s, OPCODE_BCOPYA, 1 + strlen(OPCODE_BCOPYA));
         break;
      case bcBox:
         StringHelper::copy(s, OPCODE_BOX, 1 + strlen(OPCODE_BOX));
         break;
      case bcBreakpoint:
         StringHelper::copy(s, OPCODE_BREAKPOINT, 1 + strlen(OPCODE_BREAKPOINT));
         break;
      case bcBSRedirect:
         StringHelper::copy(s, OPCODE_BSREDIRECT, 1 + strlen(OPCODE_BSREDIRECT));
         break;
      case bcBSTest:
         StringHelper::copy(s, OPCODE_BSTEST, 1 + strlen(OPCODE_BSTEST));
         break;
      case bcCallExtR:
         StringHelper::copy(s, OPCODE_CALLEXTR, 1 + strlen(OPCODE_CALLEXTR));
         break;
      case bcCallR:
         StringHelper::copy(s, OPCODE_CALLR, 1 + strlen(OPCODE_CALLR));
         break;
      case bcSCallVI:
         StringHelper::copy(s, OPCODE_SCALLVI, 1 + strlen(OPCODE_SCALLVI));
         break;
      case bcClose:
         StringHelper::copy(s, OPCODE_CLOSE, 1 + strlen(OPCODE_CLOSE));
         break;
      case bcCreate:
         StringHelper::copy(s, OPCODE_CREATE, 1 + strlen(OPCODE_CREATE));
         break;
      case bcCreateN:
         StringHelper::copy(s, OPCODE_CREATEN, 1 + strlen(OPCODE_CREATEN));
         break;
      case bcDAddAI:
         StringHelper::copy(s, OPCODE_DADDAI, 1 + strlen(OPCODE_DADDAI));
         break;
      case bcDAddSI:
         StringHelper::copy(s, OPCODE_DADDSI, 1 + strlen(OPCODE_DADDSI));
         break;
      case bcDLoadAI:
         StringHelper::copy(s, OPCODE_DLOADAI, 1 + strlen(OPCODE_DLOADAI));
         break;
      case bcDLoadFI:
         StringHelper::copy(s, OPCODE_DLOADFI, 1 + strlen(OPCODE_DLOADFI));
         break;
      case bcDLoadSI:
         StringHelper::copy(s, OPCODE_DLOADSI, 1 + strlen(OPCODE_DLOADSI));
         break;
      case bcDCopy:
         StringHelper::copy(s, OPCODE_DCOPY, 1 + strlen(OPCODE_DCOPY));
         break;
      case bcDDec:
         StringHelper::copy(s, OPCODE_DDEC, 1 + strlen(OPCODE_DDEC));
         break;
      case bcDElse:
         StringHelper::copy(s, OPCODE_DELSE, 1 + strlen(OPCODE_DELSE));
         break;
      case bcDElseN:
         StringHelper::copy(s, OPCODE_DELSEN, 1 + strlen(OPCODE_DELSEN));
         break;
      case bcDInc:
         StringHelper::copy(s, OPCODE_DINC, 1 + strlen(OPCODE_DINC));
         break;
      case bcDSaveAI:
         StringHelper::copy(s, OPCODE_DSAVEAI, 1 + strlen(OPCODE_DSAVEAI));
         break;
      case bcDSaveFI:
         StringHelper::copy(s, OPCODE_DSAVEFI, 1 + strlen(OPCODE_DSAVEFI));
         break;
      case bcDSaveSI:
         StringHelper::copy(s, OPCODE_DSAVESI, 1 + strlen(OPCODE_DSAVESI));
         break;
      case bcDSubAI:
         StringHelper::copy(s, OPCODE_DSUBAI, 1 + strlen(OPCODE_DSUBAI));
         break;
      case bcDSubSI:
         StringHelper::copy(s, OPCODE_DSUBAI, 1 + strlen(OPCODE_DSUBSI));
         break;
      case bcDThen:
         StringHelper::copy(s, OPCODE_DTHEN, 1 + strlen(OPCODE_DTHEN));
         break;
      case bcDThenN:
         StringHelper::copy(s, OPCODE_DTHENN, 1 + strlen(OPCODE_DTHENN));
         break;
      case bcElseFlag:
         StringHelper::copy(s, OPCODE_ELSEFLAG, 1 + strlen(OPCODE_ELSEFLAG));
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
      case bcIAXCopyR:
         StringHelper::copy(s, OPCODE_IAXCOPYR, 1 + strlen(OPCODE_IAXCOPYR));
         break;
//      case bcIAccFillR:
//         StringHelper::copy(s, OPCODE_IACCFILLR, 1 + strlen(OPCODE_IACCFILLR));
//         break;
      case bcInclude:
         StringHelper::copy(s, OPCODE_INCLUDE, 1 + strlen(OPCODE_INCLUDE));
         break;
//      case bcIncFI:
//         StringHelper::copy(s, OPCODE_INCFI, 1 + strlen(OPCODE_INCFI));
//         break;
//      case bcIncSI:
//         StringHelper::copy(s, OPCODE_INCSI, 1 + strlen(OPCODE_INCSI));
//         break;
      case bcJump:
         StringHelper::copy(s, OPCODE_JUMP, 1 + strlen(OPCODE_JUMP));
         break;
//      case bcJumpAcc:
//         StringHelper::copy(s, OPCODE_JUMPACC, 1 + strlen(OPCODE_JUMPACC));
//         break;
      case bcMAdd:
         StringHelper::copy(s, OPCODE_MADD, 1 + strlen(OPCODE_MADD));
         break;
      case bcMAddAI:
         StringHelper::copy(s, OPCODE_MADDAI, 1 + strlen(OPCODE_MADDAI));
         break;
      case bcMCopy:
         StringHelper::copy(s, OPCODE_MCOPY, 1 + strlen(OPCODE_MCOPY));
         break;
      case bcMCopySubj:
         StringHelper::copy(s, OPCODE_MCOPYSUBJ, 1 + strlen(OPCODE_MCOPYSUBJ));
         break;
      case bcMCopyVerb:
         StringHelper::copy(s, OPCODE_MCOPYVERB, 1 + strlen(OPCODE_MCOPYVERB));
         break;
      case bcMElse:
         StringHelper::copy(s, OPCODE_MELSE, 1 + strlen(OPCODE_MELSE));
         break;
      case bcMLoadAI:
         StringHelper::copy(s, OPCODE_MLOADAI, 1 + strlen(OPCODE_MLOADAI));
         break;
      case bcMLoadFI:
         StringHelper::copy(s, OPCODE_MLOADFI, 1 + strlen(OPCODE_MLOADFI));
         break;
      case bcMLoadSI:
         StringHelper::copy(s, OPCODE_MLOADSI, 1 + strlen(OPCODE_MLOADSI));
         break;
//      case bcMccElseAcc:
//         StringHelper::copy(s, OPCODE_MCCELSEACC, 1 + strlen(OPCODE_MCCELSEACC));
//         break;
      case bcMElseAI:
         StringHelper::copy(s, OPCODE_MELSEAI, 1 + strlen(OPCODE_MELSEAI));
         break;
//      case bcMccReverse:
//         StringHelper::copy(s, OPCODE_MCCREVERSE, 1 + strlen(OPCODE_MCCREVERSE));
//         break;
      case bcMQuit:
         StringHelper::copy(s, OPCODE_MQUIT, 1 + strlen(OPCODE_MQUIT));
         break;
      case bcMSaveParams:
         StringHelper::copy(s, OPCODE_MSAVEPARAMS, 1 + strlen(OPCODE_MSAVEPARAMS));
         break;
      case bcMThen:
         StringHelper::copy(s, OPCODE_MTHEN, 1 + strlen(OPCODE_MTHEN));
         break;
//      case bcMccThenAcc:
//         StringHelper::copy(s, OPCODE_MCCTHENACC, 1 + strlen(OPCODE_MCCTHENACC));
//         break;
//      case bcMccThenAccI:
//         StringHelper::copy(s, OPCODE_MCCTHENACCI, 1 + strlen(OPCODE_MCCTHENACCI));
//         break;
      case bcNBox:
         StringHelper::copy(s, OPCODE_NBOX, 1 + strlen(OPCODE_NBOX));
         break;
      case bcNext:
         StringHelper::copy(s, OPCODE_NEXT, 1 + strlen(OPCODE_NEXT));
         break;
      case bcNop:
         StringHelper::copy(s, OPCODE_NOP, 1 + strlen(OPCODE_NOP));
         break;
      case bcOpen:
         StringHelper::copy(s, OPCODE_OPEN, 1 + strlen(OPCODE_OPEN));
         break;
      case bcPop:
         StringHelper::copy(s, OPCODE_POP, 1 + strlen(OPCODE_POP));
         break;
      case bcPopA:
         StringHelper::copy(s, OPCODE_POPA, 1 + strlen(OPCODE_POPA));
         break;
      case bcPopAI:
         StringHelper::copy(s, OPCODE_POPAI, 1 + strlen(OPCODE_POPAI));
         break;
      case bcPopFI:
         StringHelper::copy(s, OPCODE_POPFI, 1 + strlen(OPCODE_POPFI));
         break;
      case bcPopB:
         StringHelper::copy(s, OPCODE_POPB, 1 + strlen(OPCODE_POPB));
         break;
      case bcPopI:
         StringHelper::copy(s, OPCODE_POPI, 1 + strlen(OPCODE_POPI));
         break;
      case bcPopM:
         StringHelper::copy(s, OPCODE_POPM, 1 + strlen(OPCODE_POPM));
         break;
      case bcPopBI:
         StringHelper::copy(s, OPCODE_POPBI, 1 + strlen(OPCODE_POPBI));
         break;
      case bcPopSI:
         StringHelper::copy(s, OPCODE_POPSI, 1 + strlen(OPCODE_POPSI));
         break;
      case bcPushAI:
         StringHelper::copy(s, OPCODE_PUSHAI, 1 + strlen(OPCODE_PUSHAI));
         break;
      case bcPushA:
         StringHelper::copy(s, OPCODE_PUSHA, 1 + strlen(OPCODE_PUSHA));
         break;
      case bcPushB:
         StringHelper::copy(s, OPCODE_PUSHB, 1 + strlen(OPCODE_PUSHB));
         break;
      case bcPushBI:
         StringHelper::copy(s, OPCODE_PUSHBI, 1 + strlen(OPCODE_PUSHBI));
         break;
      case bcPushF:
         StringHelper::copy(s, OPCODE_PUSHF, 1 + strlen(OPCODE_PUSHF));
         break;
      case bcPushFI:
         StringHelper::copy(s, OPCODE_PUSHFI, 1 + strlen(OPCODE_PUSHFI));
         break;
//      case bcPushI:
//         StringHelper::copy(s, OPCODE_PUSHI, 1 + strlen(OPCODE_PUSHI));
//         break;
      case bcPushM:
         StringHelper::copy(s, OPCODE_PUSHM, 1 + strlen(OPCODE_PUSHM));
         break;
      case bcPushN:
         StringHelper::copy(s, OPCODE_PUSHN, 1 + strlen(OPCODE_PUSHN));
         break;
      case bcPushR:
         StringHelper::copy(s, OPCODE_PUSHR, 1 + strlen(OPCODE_PUSHR));
         break;
      case bcPushSI:
         StringHelper::copy(s, OPCODE_PUSHSI, 1 + strlen(OPCODE_PUSHSI));
         break;
//      case bcPushSPI:
//         StringHelper::copy(s, OPCODE_PUSHSPI, 1 + strlen(OPCODE_PUSHSPI));
//         break;
      case bcQuit:
         StringHelper::copy(s, OPCODE_QUIT, 1 + strlen(OPCODE_QUIT));
         break;
      case bcQuitN:
         StringHelper::copy(s, OPCODE_QUITN, 1 + strlen(OPCODE_QUITN));
         break;
//      case bcRCallN:
//         StringHelper::copy(s, OPCODE_RCALLM, 1 + strlen(OPCODE_RCALLN));
//         break;
      case bcReserve:
         StringHelper::copy(s, OPCODE_RESERVE, 1 + strlen(OPCODE_RESERVE));
         break;
      case bcRestore:
         StringHelper::copy(s, OPCODE_RESTORE, 1 + strlen(OPCODE_RESTORE));
         break;
      //case bcRethrow:
      //   StringHelper::copy(s, OPCODE_RETHROW, 1 + strlen(OPCODE_RETHROW));
      //   break;
      case bcSet:
         StringHelper::copy(s, OPCODE_SET, 1 + strlen(OPCODE_SET));
         break;
      case bcSCopyF:
         StringHelper::copy(s, OPCODE_SCOPYF, 1 + strlen(OPCODE_SCOPYF));
         break;
      case bcSwapSI:
         StringHelper::copy(s, OPCODE_SWAPSI, 1 + strlen(OPCODE_SWAPSI));
         break;
      case bcTest:
         StringHelper::copy(s, OPCODE_TEST, 1 + strlen(OPCODE_TEST));
         break;
      case bcTestFlag:
         StringHelper::copy(s, OPCODE_TESTFLAG, 1 + strlen(OPCODE_TESTFLAG));
         break;
      case bcThrow:
         StringHelper::copy(s, OPCODE_THROW, 1 + strlen(OPCODE_THROW));
         break;
      case bcUnhook:
         StringHelper::copy(s, OPCODE_UNHOOK, 1 + strlen(OPCODE_UNHOOK));
         break;
//      case bcXAccSaveFI:
//         StringHelper::copy(s, OPCODE_XACCSAVEFI, 1 + strlen(OPCODE_XACCSAVEFI));
//         break;
//      case bcXMccCopyM:
//         StringHelper::copy(s, OPCODE_MCCCOPYM, 1 + strlen(OPCODE_XMCCCOPYM));
//         break;
      case bcWSTest:
         StringHelper::copy(s, OPCODE_WSTEST, 1 + strlen(OPCODE_WSTEST));
         break;
      case bcXPopAI:
         StringHelper::copy(s, OPCODE_XPOPAI, 1 + strlen(OPCODE_XPOPAI));
         break;
      case bcXPushF:
         StringHelper::copy(s, OPCODE_XPUSHF, 1 + strlen(OPCODE_XPUSHF));
         break;
      case bcXCallRM:
         StringHelper::copy(s, OPCODE_XCALLRM, 1 + strlen(OPCODE_XCALLRM));
         break;
      case bcNFunc:
         StringHelper::copy(s, "n", 2);
         break;
      case bcLFunc:
         StringHelper::copy(s, "l", 2);
         break;
      case bcRFunc:
         StringHelper::copy(s, "r", 2);
         break;
      case bcFunc:
         StringHelper::copy(s, "rf", 3);
         break;
      case bcWSFunc:
         StringHelper::copy(s, "ws", 3);
         break;
      case bcBSFunc:
         StringHelper::copy(s, "bs", 3);
         break;
      default:
         StringHelper::copy(s, OPCODE_UNKNOWN, 1 + strlen(OPCODE_UNKNOWN));
   }

   return s;
}

FunctionCode ByteCodeCompiler :: codeFunction(const wchar16_t* s)
{
   if (ConstantIdentifier::compare(s, FUNC_ABS)) {
      return fnAbs;
   }
   else if (ConstantIdentifier::compare(s, FUNC_ADD)) {
      return fnAdd;
   }
   else if (ConstantIdentifier::compare(s, FUNC_ADDSTR)) {
      return fnAddStr;
   }
   else if (ConstantIdentifier::compare(s, FUNC_AND)) {
      return fnAnd;
   }
   else if (ConstantIdentifier::compare(s, FUNC_COPY)) {
      return fnCopy;
   }
   else if (ConstantIdentifier::compare(s, FUNC_COPYBUF)) {
      return fnCopyBuf;
   }
   else if (ConstantIdentifier::compare(s, FUNC_COPYINT)) {
      return fnCopyInt;
   }
   else if (ConstantIdentifier::compare(s, FUNC_COPYLONG)) {
      return fnCopyLong;
   }
   else if (ConstantIdentifier::compare(s, FUNC_COPYREAL)) {
      return fnCopyReal;
   }
   else if (ConstantIdentifier::compare(s, FUNC_COPYSTR)) {
      return fnCopyStr;
   }
   else if (ConstantIdentifier::compare(s, FUNC_CREATE)) {
      return fnCreate;
   }
   else if (ConstantIdentifier::compare(s, FUNC_DELETESTR)) {
      return fnDeleteStr;
   }
   else if (ConstantIdentifier::compare(s, FUNC_DIV)) {
      return fnDiv;
   }
   else if (ConstantIdentifier::compare(s, FUNC_EQUAL)) {
      return fnEqual;
   }
   else if (ConstantIdentifier::compare(s, FUNC_EVAL)) {
      return fnEval;
   }
   else if (ConstantIdentifier::compare(s, FUNC_EXP)) {
      return fnExp;
   }
   else if (ConstantIdentifier::compare(s, FUNC_GETAT)) {
      return fnGetAt;
   }
   else if (ConstantIdentifier::compare(s, FUNC_GETBUF)) {
      return fnGetBuf;
   }
   else if (ConstantIdentifier::compare(s, FUNC_GETINT)) {
      return fnGetInt;
   }
   else if (ConstantIdentifier::compare(s, FUNC_GETLEN)) {
      return fnGetLen;
   }
   else if (ConstantIdentifier::compare(s, FUNC_GETLENZ)) {
      return fnGetLenZ;
   }
   else if (ConstantIdentifier::compare(s, FUNC_GETWORD)) {
      return fnGetWord;
   }
   else if (ConstantIdentifier::compare(s, FUNC_INC)) {
      return fnInc;
   }
   else if (ConstantIdentifier::compare(s, FUNC_INDEXOF)) {
      return fnIndexOf;
   }
   else if (ConstantIdentifier::compare(s, FUNC_INDEXOFSTR)) {
      return fnIndexOfStr;
   }
   else if (ConstantIdentifier::compare(s, FUNC_INDEXOFWORD)) {
      return fnIndexOfWord;
   }
   else if (ConstantIdentifier::compare(s, FUNC_LESS)) {
      return fnLess;
   }
   else if (ConstantIdentifier::compare(s, FUNC_LN)) {
      return fnLn;
   }
   else if (ConstantIdentifier::compare(s, FUNC_LOAD)) {
      return fnLoad;
   }
   else if (ConstantIdentifier::compare(s, FUNC_LOADNAME)) {
      return fnLoadName;
   }
   else if (ConstantIdentifier::compare(s, FUNC_LOADSTR)) {
      return fnLoadStr;
   }
   else if (ConstantIdentifier::compare(s, FUNC_MUL)) {
      return fnMul;
   }
   else if (ConstantIdentifier::compare(s, FUNC_NOT)) {
      return fnNot;
   }
   else if (ConstantIdentifier::compare(s, FUNC_NOTGREATER)) {
      return fnNotGreater;
   }
   else if (ConstantIdentifier::compare(s, FUNC_OR)) {
      return fnOr;
   }
   else if (ConstantIdentifier::compare(s, FUNC_RESERVE)) {
      return fnReserve;
   }
   else if (ConstantIdentifier::compare(s, FUNC_ROUND)) {
      return fnRound;
   }
   else if (ConstantIdentifier::compare(s, FUNC_RNDNEW)) {
      return fnRndNew;
   }
   else if (ConstantIdentifier::compare(s, FUNC_RNDNEXT)) {
      return fnRndNext;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SAVE)) {
      return fnSave;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SETAT)) {
      return fnSetAt;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SETBUF)) {
      return fnSetBuf;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SETINT)) {
      return fnSetInt;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SETLEN)) {
      return fnSetLen;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SETWORD)) {
      return fnSetWord;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SHIFT)) {
      return fnShift;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SUB)) {
      return fnSub;
   }
   else if (ConstantIdentifier::compare(s, FUNC_XOR)) {
      return fnXor;
   }
   else return fnUnknown;
}

const wchar16_t* ByteCodeCompiler :: decodeFunction(FunctionCode code, wchar16_t* s)
{
   switch (code) {
      case fnAbs:
         StringHelper::copy(s, FUNC_ABS, 1 + strlen(FUNC_ABS));
         break;
      case fnAdd:
         StringHelper::copy(s, FUNC_ADD, 1 + strlen(FUNC_ADD));
         break;
      case fnAddStr:
         StringHelper::copy(s, FUNC_ADDSTR, 1 + strlen(FUNC_ADDSTR));
         break;
      case fnAnd:
         StringHelper::copy(s, FUNC_AND, 1 + strlen(FUNC_AND));
         break;
      case fnCopy:
         StringHelper::copy(s, FUNC_COPY, 1 + strlen(FUNC_COPY));
         break;
      case fnCopyBuf:
         StringHelper::copy(s, FUNC_COPYBUF, 1 + strlen(FUNC_COPYBUF));
         break;
      case fnCopyInt:
         StringHelper::copy(s, FUNC_COPYINT, 1 + strlen(FUNC_COPYINT));
         break;
      case fnCopyLong:
         StringHelper::copy(s, FUNC_COPYLONG, 1 + strlen(FUNC_COPYLONG));
         break;
      case fnCopyReal:
         StringHelper::copy(s, FUNC_COPYREAL, 1 + strlen(FUNC_COPYREAL));
         break;
      case fnCopyStr:
         StringHelper::copy(s, FUNC_COPYSTR, 1 + strlen(FUNC_COPYSTR));
         break;
      case fnCreate:
         StringHelper::copy(s, FUNC_CREATE, 1 + strlen(FUNC_CREATE));
         break;
      case fnDeleteStr:
         StringHelper::copy(s, FUNC_DELETESTR, 1 + strlen(FUNC_DELETESTR));
         break;
      case fnDiv:
         StringHelper::copy(s, FUNC_DIV, 1 + strlen(FUNC_DIV));
         break;
      case fnEqual:
         StringHelper::copy(s, FUNC_EQUAL, 1 + strlen(FUNC_EQUAL));
         break;
      case fnEval:
         StringHelper::copy(s, FUNC_EVAL, 1 + strlen(FUNC_EVAL));
         break;
      case fnExp:
         StringHelper::copy(s, FUNC_EXP, 1 + strlen(FUNC_EXP));
         break;
      case fnGetAt:
         StringHelper::copy(s, FUNC_GETAT, 1 + strlen(FUNC_GETAT));
         break;
      case fnGetBuf:
         StringHelper::copy(s, FUNC_GETBUF, 1 + strlen(FUNC_GETBUF));
         break;
      case fnGetInt:
         StringHelper::copy(s, FUNC_GETINT, 1 + strlen(FUNC_GETINT));
         break;
      case fnGetLen:
         StringHelper::copy(s, FUNC_GETLEN, 1 + strlen(FUNC_GETLEN));
         break;
      case fnGetLenZ:
         StringHelper::copy(s, FUNC_GETLENZ, 1 + strlen(FUNC_GETLENZ));
         break;
      case fnGetWord:
         StringHelper::copy(s, FUNC_GETWORD, 1 + strlen(FUNC_GETWORD));
         break;
      case fnInc:
         StringHelper::copy(s, FUNC_INC, 1 + strlen(FUNC_INC));
         break;
      case fnIndexOf:
         StringHelper::copy(s, FUNC_INDEXOF, 1 + strlen(FUNC_INDEXOF));
         break;
      case fnIndexOfStr:
         StringHelper::copy(s, FUNC_INDEXOFSTR, 1 + strlen(FUNC_INDEXOFSTR));
         break;
      case fnIndexOfWord:
         StringHelper::copy(s, FUNC_INDEXOFWORD, 1 + strlen(FUNC_INDEXOFWORD));
         break;
      case fnLess:
         StringHelper::copy(s, FUNC_LESS, 1 + strlen(FUNC_LESS));
         break;
      case fnLn:
         StringHelper::copy(s, FUNC_LN, 1 + strlen(FUNC_LN));
         break;
      case fnLoad:
         StringHelper::copy(s, FUNC_LOAD, 1 + strlen(FUNC_LOAD));
         break;
      case fnLoadName:
         StringHelper::copy(s, FUNC_LOADNAME, 1 + strlen(FUNC_LOADSTR));
         break;
      case fnLoadStr:
         StringHelper::copy(s, FUNC_LOADSTR, 1 + strlen(FUNC_LOADSTR));
         break;
      case fnMul:
         StringHelper::copy(s, FUNC_MUL, 1 + strlen(FUNC_MUL));
         break;
      case fnNot:
         StringHelper::copy(s, FUNC_NOT, 1 + strlen(FUNC_NOT));
         break;
      case fnNotGreater:
         StringHelper::copy(s, FUNC_NOTGREATER, 1 + strlen(FUNC_NOTGREATER));
         break;
      case fnOr:
         StringHelper::copy(s, FUNC_OR, 1 + strlen(FUNC_OR));
         break;
      case fnReserve:
         StringHelper::copy(s, FUNC_RESERVE, 1 + strlen(FUNC_RESERVE));
         break;
      case fnRndNew:
         StringHelper::copy(s, FUNC_RNDNEW, 1 + strlen(FUNC_RNDNEW));
         break;
      case fnRndNext:
         StringHelper::copy(s, FUNC_RNDNEXT, 1 + strlen(FUNC_RNDNEXT));
         break;
      case fnRound:
         StringHelper::copy(s, FUNC_ROUND, 1 + strlen(FUNC_ROUND));
         break;
      case fnSave:
         StringHelper::copy(s, FUNC_SAVE, 1 + strlen(FUNC_SAVE));
         break;
      case fnSetAt:
         StringHelper::copy(s, FUNC_SETAT, 1 + strlen(FUNC_SETAT));
         break;
      case fnSetBuf:
         StringHelper::copy(s, FUNC_SETBUF, 1 + strlen(FUNC_SETBUF));
         break;
      case fnSetInt:
         StringHelper::copy(s, FUNC_SETINT, 1 + strlen(FUNC_SETINT));
         break;
      case fnSetLen:
         StringHelper::copy(s, FUNC_SETLEN, 1 + strlen(FUNC_SETLEN));
         break;
      case fnSetWord:
         StringHelper::copy(s, FUNC_SETWORD, 1 + strlen(FUNC_SETWORD));
         break;
      case fnShift:
         StringHelper::copy(s, FUNC_SHIFT, 1 + strlen(FUNC_SHIFT));
         break;
      case fnSub:
         StringHelper::copy(s, FUNC_SUB, 1 + strlen(FUNC_SUB));
         break;
      case fnXor:
         StringHelper::copy(s, FUNC_XOR, 1 + strlen(FUNC_XOR));
         break;
      default:
         StringHelper::copy(s, OPCODE_UNKNOWN, 1 + strlen(OPCODE_UNKNOWN));
   }

   return s;
}