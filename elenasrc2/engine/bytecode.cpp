//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implementation of ELENA byte code routines.
//
//                                                 (C)2009-2014, by Alexei Rakov
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
#define OPCODE_BSGREDIRECT  "bsgredirect"
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
#define OPCODE_IAXLOADBI    "iaxloadbi"
#define OPCODE_IAXLOADFI    "iaxloadfi"
#define OPCODE_IAXLOADSI    "iaxloadsi"
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
#define OPCODE_MELSEVERB    "melseverb"
#define OPCODE_MLOADAI      "mloadai"
#define OPCODE_MLOADFI      "mloadfi"
#define OPCODE_MLOADSI      "mloadsi"
//#define OPCODE_MCCELSEACC   "mccelseacc"
#define OPCODE_MELSEAI      "melseai"
//#define OPCODE_MCCREVERSE   "mccreverse"
#define OPCODE_MQUIT        "mquit"
#define OPCODE_MRESET       "mreset"
#define OPCODE_MSAVEPARAMS  "msaveparams"
#define OPCODE_MTHEN        "mthen"
#define OPCODE_MTHENVERB    "mthenverb"
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
#define OPCODE_UNBOX        "unbox"
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
#define FUNC_ARCTAN         "arctan"
#define FUNC_COPY           "copy"
#define FUNC_COPYBUF        "copybuf"
#define FUNC_COPYINT        "copyint"
#define FUNC_COPYLONG       "copylong"
#define FUNC_COPYREAL       "copyreal"
#define FUNC_COPYSTR        "copystr"
#define FUNC_COS            "cos"
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
#define FUNC_INT            "int"
#define FUNC_LESS           "less"
#define FUNC_LN             "ln"
#define FUNC_LOAD           "load"
#define FUNC_LOADNAME       "loadname"
#define FUNC_LOADSTR        "loadstr"
#define FUNC_MUL            "mul"
#define FUNC_NOT            "not"
#define FUNC_NOTGREATER     "notgreater"
#define FUNC_OR             "or"
#define FUNC_PI             "pi"
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
#define FUNC_SIN            "sin"
#define FUNC_SHIFT          "shift"
#define FUNC_SQRT           "sqrt"
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
      case bcMElseVerb:
      case bcMThenVerb:
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
         case bcMElseVerb:
         case bcMThenVerb:
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
            case bcMElseVerb:
            case bcMThenVerb:
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

bool TransformTape :: makeStep(Node& step, ByteCommand& command)
{
   Node::ChildEnumerator child = step.Children();
   Node defaultNode(&trie);

   while (!child.Eof()) {
      Node current = child.Node();
      if (current.Value() == command) {
         step = current;

         return true;
      }
      else if (current.Value().code == bcMatch) {
         step = current;

         return true;
      }

      child++;
   }

   if (defaultNode != step) {
      step = defaultNode;

      return makeStep(step, command);
   }

   return false;
}

inline bool matchable(ByteCodeIterator& it)
{
   return test(*it, blLabelMask) || (*it < bcReserved && *it > bcNop);
}

bool TransformTape :: apply(CommandTape& commandTape)
{
   ByteCodeIterator it = commandTape.start();

   bool applied = false;
   Node current(&trie);
   while (!it.Eof()) {
      // skip meta commands (except labels)
      if (matchable(it)) {
         // make first step
         if (makeStep(current, *it)) {
            it++;

            ByteCodeIterator word_it = it;
            while (!word_it.Eof() && (!matchable(word_it) || makeStep(current, *word_it))) {
               // check if the end node is reached
               if (current.Value().code == bcMatch) {
                  it = word_it;

                  transform(--word_it, current.FirstNode());

                  applied = true;
                  current = Node(&trie);

                  break;
               }
               else word_it++;
            }
         }
         else it++;
      }
      else it++;
   } 

   return applied;
}

// --- ByteCodeCompiler ---

// --- load verb dictionary ---

inline void addVerb(MessageMap& map, const char* verb, int id)
{
   String<wchar16_t, 20> verbName(verb);

   map.add(verbName, id);
}

void ByteCodeCompiler :: loadVerbs(MessageMap& verbs)
{
   // load verbs
   addVerb(verbs, NEW_MESSAGE, NEW_MESSAGE_ID);
   addVerb(verbs, GET_MESSAGE,        GET_MESSAGE_ID);
   addVerb(verbs, EVAL_MESSAGE,       EVAL_MESSAGE_ID);
   addVerb(verbs, EVALUATE_MESSAGE,   EVAL_MESSAGE_ID);
   addVerb(verbs, EQUAL_MESSAGE,      EQUAL_MESSAGE_ID);
   addVerb(verbs, LESS_MESSAGE,       LESS_MESSAGE_ID);
   addVerb(verbs, AND_MESSAGE,        AND_MESSAGE_ID);
   addVerb(verbs, OR_MESSAGE,         OR_MESSAGE_ID);
   addVerb(verbs, XOR_MESSAGE,        XOR_MESSAGE_ID);
   addVerb(verbs, DO_MESSAGE,         DO_MESSAGE_ID);
   addVerb(verbs, STOP_MESSAGE,       STOP_MESSAGE_ID);
   addVerb(verbs, NOTEQUAL_MESSAGE,   NOTEQUAL_MESSAGE_ID);
   addVerb(verbs, NOTLESS_MESSAGE,    NOTLESS_MESSAGE_ID);
   addVerb(verbs, NOTGREATER_MESSAGE, NOTGREATER_MESSAGE_ID);
   addVerb(verbs, GREATER_MESSAGE,    GREATER_MESSAGE_ID);
   addVerb(verbs, ADD_MESSAGE,        ADD_MESSAGE_ID);
   addVerb(verbs, SUB_MESSAGE,        SUB_MESSAGE_ID);
   addVerb(verbs, MUL_MESSAGE,        MUL_MESSAGE_ID);
   addVerb(verbs, DIV_MESSAGE,        DIV_MESSAGE_ID);
   addVerb(verbs, REFER_MESSAGE,      REFER_MESSAGE_ID);
   addVerb(verbs, APPEND_MESSAGE,     APPEND_MESSAGE_ID);
   addVerb(verbs, REDUCE_MESSAGE,     REDUCE_MESSAGE_ID);
   addVerb(verbs, INCREASE_MESSAGE,   INCREASE_MESSAGE_ID);
   addVerb(verbs, SEPARATE_MESSAGE,   SEPARATE_MESSAGE_ID);
   addVerb(verbs, SET_REFER_MESSAGE,  SET_REFER_MESSAGE_ID);
   addVerb(verbs, SET_MESSAGE,        SET_MESSAGE_ID);
   addVerb(verbs, READ_MESSAGE,       READ_MESSAGE_ID);
   addVerb(verbs, WRITE_MESSAGE,      WRITE_MESSAGE_ID);
   addVerb(verbs, RAISE_MESSAGE,      RAISE_MESSAGE_ID);
   addVerb(verbs, IFFAILED_MESSAGE,   IFFAILED_MESSAGE_ID);
   addVerb(verbs, FIND_MESSAGE,       FIND_MESSAGE_ID);
   addVerb(verbs, SEEK_MESSAGE,       SEEK_MESSAGE_ID);
   addVerb(verbs, REVERSE_MESSAGE,    REVERSE_MESSAGE_ID);
   addVerb(verbs, EXCHANGE_MESSAGE,   EXCHANGE_MESSAGE_ID);
   addVerb(verbs, INDEXOF_MESSAGE,    INDEXOF_MESSAGE_ID);
   addVerb(verbs, CLOSE_MESSAGE,      CLOSE_MESSAGE_ID);
   addVerb(verbs, CLEAR_MESSAGE,      CLEAR_MESSAGE_ID);
   addVerb(verbs, DELETE_MESSAGE,     DELETE_MESSAGE_ID);
   addVerb(verbs, RUN_MESSAGE,        RUN_MESSAGE_ID);
   addVerb(verbs, INSERT_MESSAGE,     INSERT_MESSAGE_ID);
   addVerb(verbs, SAVE_MESSAGE,       SAVE_MESSAGE_ID);
   addVerb(verbs, RESET_MESSAGE,      RESET_MESSAGE_ID);
   addVerb(verbs, CONVERT_MESSAGE,    CONVERT_MESSAGE_ID);
   addVerb(verbs, FILL_MESSAGE,       FILL_MESSAGE_ID);
   addVerb(verbs, LOAD_MESSAGE,       LOAD_MESSAGE_ID);
   addVerb(verbs, SHIFT_MESSAGE,      SHIFT_MESSAGE_ID);
   addVerb(verbs, NOT_MESSAGE,        NOT_MESSAGE_ID);
   addVerb(verbs, VALIDATE_MESSAGE,   VALIDATE_MESSAGE_ID);
   addVerb(verbs, INC_MESSAGE,        INC_MESSAGE_ID);
   addVerb(verbs, START_MESSAGE,      START_MESSAGE_ID);
}

void ByteCodeCompiler :: loadOperators(MessageMap& operators)
{
   addVerb(operators, ADD_OPERATOR, ADD_MESSAGE_ID);
   addVerb(operators, SUB_OPERATOR, SUB_MESSAGE_ID);
   addVerb(operators, MUL_OPERATOR, MUL_MESSAGE_ID);
   addVerb(operators, DIV_OPERATOR, DIV_MESSAGE_ID);
   addVerb(operators, IF_OPERATOR, IF_MESSAGE_ID);
   addVerb(operators, IFNOT_OPERATOR, IFNOT_MESSAGE_ID);
   addVerb(operators, EQUAL_OPERATOR, EQUAL_MESSAGE_ID);
   addVerb(operators, NOTEQUAL_OPERATOR, NOTEQUAL_MESSAGE_ID);
   addVerb(operators, LESS_OPERATOR, LESS_MESSAGE_ID);
   addVerb(operators, GREATER_OPERATOR, GREATER_MESSAGE_ID);
   addVerb(operators, NOTLESS_OPERATOR, NOTLESS_MESSAGE_ID);
   addVerb(operators, NOTGREATER_OPERATOR, NOTGREATER_MESSAGE_ID);
   addVerb(operators, AND_OPERATOR, AND_MESSAGE_ID);
   addVerb(operators, OR_OPERATOR, OR_MESSAGE_ID);
   addVerb(operators, XOR_OPERATOR, XOR_MESSAGE_ID);
   addVerb(operators, REFER_OPERATOR, REFER_MESSAGE_ID);
   addVerb(operators, APPEND_OPERATOR, APPEND_MESSAGE_ID);
   addVerb(operators, REDUCE_OPERATOR, REDUCE_MESSAGE_ID);
   addVerb(operators, INCREASE_OPERATOR, INCREASE_MESSAGE_ID);
   addVerb(operators, SEPARATE_OPERATOR, SEPARATE_MESSAGE_ID);
   addVerb(operators, WRITE_OPERATOR, WRITE_MESSAGE_ID);
   addVerb(operators, READ_OPERATOR, READ_MESSAGE_ID);
}

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
   else if (ConstantIdentifier::compare(s, OPCODE_BSGREDIRECT)) {
      return bcBSGRedirect;
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
   else if (ConstantIdentifier::compare(s, OPCODE_IAXLOADBI)) {
      return bcIAXLoadBI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_IAXLOADFI)) {
      return bcIAXLoadFI;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_IAXLOADSI)) {
      return bcIAXLoadSI;
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
   else if (ConstantIdentifier::compare(s, OPCODE_MELSEVERB)) {
      return bcMElseVerb;
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
   else if (ConstantIdentifier::compare(s, OPCODE_MRESET)) {
      return bcMReset;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MSAVEPARAMS)) {
      return bcMSaveParams;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MTHEN)) {
      return bcMThen;
   }
   else if (ConstantIdentifier::compare(s, OPCODE_MTHENVERB)) {
      return bcMThenVerb;
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
   else if (ConstantIdentifier::compare(s, OPCODE_UNBOX)) {
      return bcUnbox;
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
         copystr(s, OPCODE_ACALLVI);
         break;
      case bcACopyB:
         copystr(s, OPCODE_ACOPYB);
         break;
      case bcACopyF:
         copystr(s, OPCODE_ACOPYF);
         break;
      //case bcACopyN:
      //   StringHelper::copy(s, OPCODE_ACOPYN, 1 + strlen(OPCODE_ACOPYN));
      //   break;
      case bcACopyR:
         copystr(s, OPCODE_ACOPYR);
         break;
      case bcACopyS:
         copystr(s, OPCODE_ACOPYS);
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
         copystr(s, OPCODE_ALOADAI);
         break;
      case bcALoadD:
         copystr(s, OPCODE_ALOADD);
         break;
      case bcALoadBI:
         copystr(s, OPCODE_ALOADBI);
         break;
      case bcALoadFI:
         copystr(s, OPCODE_ALOADFI);
         break;
      case bcAElse:
         copystr(s, OPCODE_AELSE);
         break;
      case bcAElseR:
         copystr(s, OPCODE_AELSER);
         break;
      case bcAElseSI:
         copystr(s, OPCODE_AELSESI);
         break;
      case bcAJumpVI:
         copystr(s, OPCODE_AJUMPVI);
         break;
      case bcALoadR:
         copystr(s, OPCODE_ALOADR);
         break;
      //case bcAMul:
      //   StringHelper::copy(s, OPCODE_AMUL, 1 + strlen(OPCODE_AMUL));
      //   break;
      case bcASaveR:
         copystr(s, OPCODE_ASAVER);
         break;
      case bcAThen:
         copystr(s, OPCODE_ATHEN);
         break;
      case bcAThenR:
         copystr(s, OPCODE_ATHENR);
         break;
      case bcAThenSI:
         copystr(s, OPCODE_ATHENSI);
         break;
      case bcAXCopyF:
         copystr(s, OPCODE_AXCOPYF);
         break;
      case bcAXSetR:
         copystr(s, OPCODE_AXSETR);
         break;
      case bcALoadSI:
         copystr(s, OPCODE_ALOADSI);
         break;
      case bcASaveBI:
         copystr(s, OPCODE_ASAVEBI);
         break;
      case bcASaveFI:
         copystr(s, OPCODE_ASAVEFI);
         break;
      case bcASaveSI:
         copystr(s, OPCODE_ASAVESI);
         break;
      case bcASwapSI:
         copystr(s, OPCODE_ASWAPSI);
         break;
      case bcBCopyA:
         copystr(s, OPCODE_BCOPYA);
         break;
      case bcBox:
         copystr(s, OPCODE_BOX);
         break;
      case bcBreakpoint:
         copystr(s, OPCODE_BREAKPOINT);
         break;
      case bcBSGRedirect:
         copystr(s, OPCODE_BSGREDIRECT);
         break;
      case bcBSRedirect:
         copystr(s, OPCODE_BSREDIRECT);
         break;
      case bcBSTest:
         copystr(s, OPCODE_BSTEST);
         break;
      case bcCallExtR:
         copystr(s, OPCODE_CALLEXTR);
         break;
      case bcCallR:
         copystr(s, OPCODE_CALLR);
         break;
      case bcSCallVI:
         copystr(s, OPCODE_SCALLVI);
         break;
      case bcClose:
         copystr(s, OPCODE_CLOSE);
         break;
      case bcCreate:
         copystr(s, OPCODE_CREATE);
         break;
      case bcCreateN:
         copystr(s, OPCODE_CREATEN);
         break;
      case bcDAddAI:
         copystr(s, OPCODE_DADDAI);
         break;
      case bcDAddSI:
         copystr(s, OPCODE_DADDSI);
         break;
      case bcDLoadAI:
         copystr(s, OPCODE_DLOADAI);
         break;
      case bcDLoadFI:
         copystr(s, OPCODE_DLOADFI);
         break;
      case bcDLoadSI:
         copystr(s, OPCODE_DLOADSI);
         break;
      case bcDCopy:
         copystr(s, OPCODE_DCOPY);
         break;
      case bcDDec:
         copystr(s, OPCODE_DDEC);
         break;
      case bcDElse:
         copystr(s, OPCODE_DELSE);
         break;
      case bcDElseN:
         copystr(s, OPCODE_DELSEN);
         break;
      case bcDInc:
         copystr(s, OPCODE_DINC);
         break;
      case bcDSaveAI:
         copystr(s, OPCODE_DSAVEAI);
         break;
      case bcDSaveFI:
         copystr(s, OPCODE_DSAVEFI);
         break;
      case bcDSaveSI:
         copystr(s, OPCODE_DSAVESI);
         break;
      case bcDSubAI:
         copystr(s, OPCODE_DSUBAI);
         break;
      case bcDSubSI:
         copystr(s, OPCODE_DSUBAI);
         break;
      case bcDThen:
         copystr(s, OPCODE_DTHEN);
         break;
      case bcDThenN:
         copystr(s, OPCODE_DTHENN);
         break;
      case bcElseFlag:
         copystr(s, OPCODE_ELSEFLAG);
         break;
      case bcEvalR:
         copystr(s, OPCODE_EVALR);
         break;
      case bcExclude:
         copystr(s, OPCODE_EXCLUDE);
         break;
      case bcGet:
         copystr(s, OPCODE_GET);
         break;
      case bcGetLen:
         copystr(s, OPCODE_GETLEN);
         break;
      case bcHook:
         copystr(s, OPCODE_HOOK);
         break;
      case bcIAXCopyR:
         copystr(s, OPCODE_IAXCOPYR);
         break;
      case bcIAXLoadBI:
         copystr(s, OPCODE_IAXLOADBI);
         break;
      case bcIAXLoadFI:
         copystr(s, OPCODE_IAXLOADFI);
         break;
      case bcIAXLoadSI:
         copystr(s, OPCODE_IAXLOADSI);
         break;
      case bcInclude:
         copystr(s, OPCODE_INCLUDE);
         break;
      case bcJump:
         copystr(s, OPCODE_JUMP);
         break;
//      case bcJumpAcc:
//         copystr(s, OPCODE_JUMPACC, 1 + strlen(OPCODE_JUMPACC));
//         break;
      case bcMAdd:
         copystr(s, OPCODE_MADD);
         break;
      case bcMAddAI:
         copystr(s, OPCODE_MADDAI);
         break;
      case bcMCopy:
         copystr(s, OPCODE_MCOPY);
         break;
      case bcMCopySubj:
         copystr(s, OPCODE_MCOPYSUBJ);
         break;
      case bcMCopyVerb:
         copystr(s, OPCODE_MCOPYVERB);
         break;
      case bcMElse:
         copystr(s, OPCODE_MELSE);
         break;
      case bcMElseVerb:
         copystr(s, OPCODE_MELSEVERB);
         break;
      case bcMLoadAI:
         copystr(s, OPCODE_MLOADAI);
         break;
      case bcMLoadFI:
         copystr(s, OPCODE_MLOADFI);
         break;
      case bcMLoadSI:
         copystr(s, OPCODE_MLOADSI);
         break;
//      case bcMccElseAcc:
//         copystr(s, OPCODE_MCCELSEACC, 1 + strlen(OPCODE_MCCELSEACC));
//         break;
      case bcMElseAI:
         copystr(s, OPCODE_MELSEAI);
         break;
//      case bcMccReverse:
//         copystr(s, OPCODE_MCCREVERSE, 1 + strlen(OPCODE_MCCREVERSE));
//         break;
      case bcMQuit:
         copystr(s, OPCODE_MQUIT);
         break;
      case bcMReset:
         copystr(s, OPCODE_MRESET);
         break;
      case bcMSaveParams:
         copystr(s, OPCODE_MSAVEPARAMS);
         break;
      case bcMThen:
         copystr(s, OPCODE_MTHEN);
         break;
      case bcMThenVerb:
         copystr(s, OPCODE_MTHENVERB);
         break;
//      case bcMccThenAcc:
//         copystr(s, OPCODE_MCCTHENACC, 1 + strlen(OPCODE_MCCTHENACC));
//         break;
//      case bcMccThenAccI:
//         copystr(s, OPCODE_MCCTHENACCI, 1 + strlen(OPCODE_MCCTHENACCI));
//         break;
      case bcNBox:
         copystr(s, OPCODE_NBOX);
         break;
      case bcNext:
         copystr(s, OPCODE_NEXT);
         break;
      case bcNop:
         copystr(s, OPCODE_NOP);
         break;
      case bcOpen:
         copystr(s, OPCODE_OPEN);
         break;
      case bcPop:
         copystr(s, OPCODE_POP);
         break;
      case bcPopA:
         copystr(s, OPCODE_POPA);
         break;
      case bcPopAI:
         copystr(s, OPCODE_POPAI);
         break;
      case bcPopFI:
         copystr(s, OPCODE_POPFI);
         break;
      case bcPopB:
         copystr(s, OPCODE_POPB);
         break;
      case bcPopI:
         copystr(s, OPCODE_POPI);
         break;
      case bcPopM:
         copystr(s, OPCODE_POPM);
         break;
      case bcPopBI:
         copystr(s, OPCODE_POPBI);
         break;
      case bcPopSI:
         copystr(s, OPCODE_POPSI);
         break;
      case bcPushAI:
         copystr(s, OPCODE_PUSHAI);
         break;
      case bcPushA:
         copystr(s, OPCODE_PUSHA);
         break;
      case bcPushB:
         copystr(s, OPCODE_PUSHB);
         break;
      case bcPushBI:
         copystr(s, OPCODE_PUSHBI);
         break;
      case bcPushF:
         copystr(s, OPCODE_PUSHF);
         break;
      case bcPushFI:
         copystr(s, OPCODE_PUSHFI);
         break;
//      case bcPushI:
//         copystr(s, OPCODE_PUSHI, 1 + strlen(OPCODE_PUSHI));
//         break;
      case bcPushM:
         copystr(s, OPCODE_PUSHM);
         break;
      case bcPushN:
         copystr(s, OPCODE_PUSHN);
         break;
      case bcPushR:
         copystr(s, OPCODE_PUSHR);
         break;
      case bcPushSI:
         copystr(s, OPCODE_PUSHSI);
         break;
//      case bcPushSPI:
//         copystr(s, OPCODE_PUSHSPI, 1 + strlen(OPCODE_PUSHSPI));
//         break;
      case bcQuit:
         copystr(s, OPCODE_QUIT);
         break;
      case bcQuitN:
         copystr(s, OPCODE_QUITN);
         break;
//      case bcRCallN:
//         copystr(s, OPCODE_RCALLM, 1 + strlen(OPCODE_RCALLN));
//         break;
      case bcReserve:
         copystr(s, OPCODE_RESERVE);
         break;
      case bcRestore:
         copystr(s, OPCODE_RESTORE);
         break;
      //case bcRethrow:
      //   copystr(s, OPCODE_RETHROW, 1 + strlen(OPCODE_RETHROW));
      //   break;
      case bcSet:
         copystr(s, OPCODE_SET);
         break;
      case bcSCopyF:
         copystr(s, OPCODE_SCOPYF);
         break;
      case bcSwapSI:
         copystr(s, OPCODE_SWAPSI);
         break;
      case bcTest:
         copystr(s, OPCODE_TEST);
         break;
      case bcTestFlag:
         copystr(s, OPCODE_TESTFLAG);
         break;
      case bcThrow:
         copystr(s, OPCODE_THROW);
         break;
      case bcUnbox:
         copystr(s, OPCODE_UNBOX);
         break;
      case bcUnhook:
         copystr(s, OPCODE_UNHOOK);
         break;
//      case bcXAccSaveFI:
//         copystr(s, OPCODE_XACCSAVEFI, 1 + strlen(OPCODE_XACCSAVEFI));
//         break;
//      case bcXMccCopyM:
//         copystr(s, OPCODE_MCCCOPYM, 1 + strlen(OPCODE_XMCCCOPYM));
//         break;
      case bcWSTest:
         copystr(s, OPCODE_WSTEST);
         break;
      case bcXPopAI:
         copystr(s, OPCODE_XPOPAI);
         break;
      case bcXPushF:
         copystr(s, OPCODE_XPUSHF);
         break;
      case bcXCallRM:
         copystr(s, OPCODE_XCALLRM);
         break;
      case bcNFunc:
         copystr(s, "n");
         break;
      case bcLFunc:
         copystr(s, "l");
         break;
      case bcRFunc:
         copystr(s, "r");
         break;
      case bcFunc:
         copystr(s, "rf");
         break;
      case bcWSFunc:
         copystr(s, "ws");
         break;
      case bcBSFunc:
         copystr(s, "bs");
         break;
      default:
         copystr(s, OPCODE_UNKNOWN);
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
   else if (ConstantIdentifier::compare(s, FUNC_ARCTAN)) {
      return fnArcTan;
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
   else if (ConstantIdentifier::compare(s, FUNC_COS)) {
      return fnCos;
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
   else if (ConstantIdentifier::compare(s, FUNC_INT)) {
      return fnInt;
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
   else if (ConstantIdentifier::compare(s, FUNC_PI)) {
      return fnPi;
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
   else if (ConstantIdentifier::compare(s, FUNC_SIN)) {
      return fnSin;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SHIFT)) {
      return fnShift;
   }
   else if (ConstantIdentifier::compare(s, FUNC_SQRT)) {
      return fnSqrt;
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
         copystr(s, FUNC_ABS);
         break;
      case fnAdd:
         copystr(s, FUNC_ADD);
         break;
      case fnAddStr:
         copystr(s, FUNC_ADDSTR);
         break;
      case fnAnd:
         copystr(s, FUNC_AND);
         break;
      case fnArcTan:
         copystr(s, FUNC_ARCTAN);
         break;
      case fnCopy:
         copystr(s, FUNC_COPY);
         break;
      case fnCopyBuf:
         copystr(s, FUNC_COPYBUF);
         break;
      case fnCopyInt:
         copystr(s, FUNC_COPYINT);
         break;
      case fnCopyLong:
         copystr(s, FUNC_COPYLONG);
         break;
      case fnCopyReal:
         copystr(s, FUNC_COPYREAL);
         break;
      case fnCopyStr:
         copystr(s, FUNC_COPYSTR);
         break;
      case fnCos:
         copystr(s, FUNC_COS);
         break;
      case fnCreate:
         copystr(s, FUNC_CREATE);
         break;
      case fnDeleteStr:
         copystr(s, FUNC_DELETESTR);
         break;
      case fnDiv:
         copystr(s, FUNC_DIV);
         break;
      case fnEqual:
         copystr(s, FUNC_EQUAL);
         break;
      case fnEval:
         copystr(s, FUNC_EVAL);
         break;
      case fnExp:
         copystr(s, FUNC_EXP);
         break;
      case fnGetAt:
         copystr(s, FUNC_GETAT);
         break;
      case fnGetBuf:
         copystr(s, FUNC_GETBUF);
         break;
      case fnGetInt:
         copystr(s, FUNC_GETINT);
         break;
      case fnGetLen:
         copystr(s, FUNC_GETLEN);
         break;
      case fnGetLenZ:
         copystr(s, FUNC_GETLENZ);
         break;
      case fnGetWord:
         copystr(s, FUNC_GETWORD);
         break;
      case fnInc:
         copystr(s, FUNC_INC);
         break;
      case fnIndexOf:
         copystr(s, FUNC_INDEXOF);
         break;
      case fnIndexOfStr:
         copystr(s, FUNC_INDEXOFSTR);
         break;
      case fnIndexOfWord:
         copystr(s, FUNC_INDEXOFWORD);
         break;
      case fnInt:
         copystr(s, FUNC_INT);
         break;
      case fnLess:
         copystr(s, FUNC_LESS);
         break;
      case fnLn:
         copystr(s, FUNC_LN);
         break;
      case fnLoad:
         copystr(s, FUNC_LOAD);
         break;
      case fnLoadName:
         copystr(s, FUNC_LOADNAME);
         break;
      case fnLoadStr:
         copystr(s, FUNC_LOADSTR);
         break;
      case fnMul:
         copystr(s, FUNC_MUL);
         break;
      case fnNot:
         copystr(s, FUNC_NOT);
         break;
      case fnNotGreater:
         copystr(s, FUNC_NOTGREATER);
         break;
      case fnOr:
         copystr(s, FUNC_OR);
         break;
      case fnPi:
         copystr(s, FUNC_PI);
         break;
      case fnReserve:
         copystr(s, FUNC_RESERVE);
         break;
      case fnRndNew:
         copystr(s, FUNC_RNDNEW);
         break;
      case fnRndNext:
         copystr(s, FUNC_RNDNEXT);
         break;
      case fnRound:
         copystr(s, FUNC_ROUND);
         break;
      case fnSave:
         copystr(s, FUNC_SAVE);
         break;
      case fnSetAt:
         copystr(s, FUNC_SETAT);
         break;
      case fnSetBuf:
         copystr(s, FUNC_SETBUF);
         break;
      case fnSetInt:
         copystr(s, FUNC_SETINT);
         break;
      case fnSetLen:
         copystr(s, FUNC_SETLEN);
         break;
      case fnSetWord:
         copystr(s, FUNC_SETWORD);
         break;
      case fnSin:
         copystr(s, FUNC_SIN);
         break;
      case fnShift:
         copystr(s, FUNC_SHIFT);
         break;
      case fnSqrt:
         copystr(s, FUNC_SQRT);
         break;
      case fnSub:
         copystr(s, FUNC_SUB);
         break;
      case fnXor:
         copystr(s, FUNC_XOR);
         break;
      default:
         copystr(s, OPCODE_UNKNOWN);
   }

   return s;
}