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

#define OPCODE_UNKNOWN      "unknown"

const char* _fnOpcodes[256] =
{
   "nop", "breakpoint", "pushb", "pop", "unknown", "pushm", "mcopyverb", "throw",
   "unknown", "mcopysubj", "pusha", "popa", "acopyb", "popm", "bsredirect", "unbox",

   "bsgredirect", "getlen", "bcopya", "ddec", "popb", "close", "unknown", "quit",
   "get", "set", "dinc", "mquit", "aloadd", "unhook", "exclude", "include",

   "reserve", "pushn", "pushr", "pushbi", "pushai", "unknown", "pushfi", "unknown",
   "unknown", "msaveparams", "pushsi", "unknown", "unknown", "pushf", "unknown", "unknown",

   "popi", "popbi", "popfi", "xpopai", "popsi", "popai", "unknown", "unknown",
   "unknown", "quitn", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",

   "callextr", "evalr", "acallvi", "callr", "unknown", "unknown", "unknown", "mloadai",
   "mloadsi", "mloadfi", "msaveai", "msetverb", "mreset", "maddai", "mcopy", "madd",

   "dloadsi", "dsavesi", "dloadfi", "aloadr", "aloadfi", "aloadsi", "dcopy", "dloadai",
   "unknown", "daddai", "dsubai", "daddsi", "dsubsi", "dsavefi", "unknown", "unknown",

   "unknown", "asavebi", "unknown", "asavesi", "asavefi", "asaver", "unknown", "dsaveai",
   "unknown", "unknown", "unknown", "unknown", "swapsi", "aswapsi", "axcopyr", "iaxloadb",

   "unknown", "unknown", "scopyf", "unknown", "unknown", "acopys", "unknown", "unknown",
   "acopyr", "unknown", "aloadai", "unknown", "acopyf", "unknown", "unknown", "unknown",

   "unknown", "unknown", "reserved", "unknown", "unknown", "unknown", "unknown", "unknown",
   "open", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",

   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",

   "jump", "ajumpvi", "unknown", "unknown", "unknown", "unknown", "hook", "delse",
   "dthen", "aelse", "athen", "unknown", "unknown", "unknown", "unknown", "unknown",

   "nbox", "box", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",

   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "aloadbi", "unknown",

   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
   "unknown", "unknown", "unknown", "unknown", "unknown", "bstest", "test", "wstest",

   "aelser", "athenr", "melse", "mthen", "delsen", "dthenn", "aelsesi", "athensi",
   "melseverb", "mthenverb", "melseai", "mthenai", "elseflag", "testflag", "unknown", "next",

   "create", "createn", "iaxcopyr", "iaxloadfi", "iaxloadsi", "iaxloadbi", "unknown", "unknown",
   "unknown", "unknown", "unknown", "unknown", "scallvi", "unknown", "xcallrm", "unknown",
};

const char* _fnExtensions[EXTENSION_COUNT] =
{
   "unknown", "ncopy", "ncopystr", "nload", "nequal", "nless", "nnotgreater", "nadd", "nsub", "nmul",
   "ndiv", "nand", "nor", "nxor", "nshift", "nnot", "ninc", 
   "lcopy", "lcopyint", "lcopystr", "lload", "lequal", "lless", "lnotgreater", "ladd", "lsub", "lmul",
   "ldiv", "land", "lor", "lxor", "lnot", "lshift", 
   "rcopy", "rcopyint", "rcopylong", "rcopystr", "requal", "rless", "rnotgreater", "radd", "rsub", "rmul",
   "rdiv", "raddint", "rsubint", "rmulint", "rdivint", "raddlong", "rsublong", "rmullong", "rdivlong",
   "wsgetlen", "wssetlen", "wscreate", "wscopy", "wscopyint", "wscopylong", "wscopyreal", "wscopybuf",
   "wsequal", "wsless", "wsnotgreater", "wsadd", "wsgetat", "wssetat", "wsindexofstr", "wscopystr", 
   "wsaddstr", "wsloadname", "bssetbuf", "bsgetbuf", "bscopystr", "bssetword", "bsgetword", "bsindexof",
   "bsindexofword", "bseval", "lrndnew", "lrndnext", "rabs", "rround", "rexp", "rln", "rint", "rcos",
   "rsin", "rarctan", "rsqrt", "rpi", "refgetlenz", "refcreate", "bscreate", "nsave", "lsave", "wssave",
   "wsreserve", "bssave", "bsreserve", "bsgetlen", "bssetlen", "wsload", "bsload", "bsgetint", "ncopyword",
   "loadclass", "indexofmsg", "wseval", "ncall"
};

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
      case bcAElseR:
      case bcAThenR:
      case bcMElse:
      case bcMThen:
      case bcMElseVerb:
      case bcMThenVerb:
      case bcAElseSI:
      case bcAThenSI:
      case bcMElseAI:
      case bcMThenAI:
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
      case bcAXCopyR:
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

void CommandTape :: write(ByteCode code, int argument, Predicate predicate)
{
   write(ByteCommand(code, argument, 0, predicate));
}

void CommandTape :: write(ByteCode code, int argument, int additional, Predicate predicate)
{
   write(ByteCommand(code, argument, additional, predicate));
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

void CommandTape :: import(_Memory* section, bool withHeader)
{
   ByteCode code;
   int      argument = 0;
   int      additional = 0;

   Map<int, int> labels;

   MemoryReader reader(section);

   if (withHeader)
      reader.getDWord();

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
         case bcMThenAI:
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
   CachedMemoryMap<int, ByteCodeIterator, 20> idleLabels; // used to remove unused labels

   blocks.add(0, -1);

   // populate blocks and jump lists
   int index = 0;
   while (*it != blEnd) {
      // skip pseudo commands (except labels)
      ByteCode code = *it;

      if (code == blLabel) {
         labels.add((*it).argument, index);

         // add to idleLabels only if there are no forward jumps to it
         if (!fixes.exist((*it).argument)) {
            idleLabels.add((*it).argument, it);
         }

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
//            case bcMQuit:
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
            case bcMThenAI:
            //case bcMccThenAccI:
            case bcElseFlag:
            case bcTestFlag:
            case bcHook:
               // remove the label from idle list
               idleLabels.exclude((*it).argument);

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

   // remove idle labels
   CachedMemoryMap<int, ByteCodeIterator, 20>::Iterator i_it = idleLabels.start();
   while (!i_it.Eof()) {
      *(*i_it) = bcNop;

      i_it++;
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
   for(int i = 0 ; i < 255 ; i++) {
      if (ConstantIdentifier::compare(s, _fnOpcodes[i])) {
         return (ByteCode)i;
      }
   }

   return bcNone;
}

const wchar16_t* ByteCodeCompiler :: decode(ByteCode code, wchar16_t* s)
{
   copystr(s, _fnOpcodes[(int)code]);

   return s;
}

FunctionCode ByteCodeCompiler :: codeFunction(const wchar16_t* s)
{
   for(int i = 1 ; i < EXTENSION_COUNT ; i++) {
      if (ConstantIdentifier::compare(s, _fnExtensions[i])) {
         return (FunctionCode)i;
      }
   }

   return fnUnknown;
}

const wchar16_t* ByteCodeCompiler :: decodeFunction(FunctionCode code, wchar16_t* s)
{
   size_t key = (size_t)code;

   if (key < EXTENSION_COUNT) {
      copystr(s, _fnExtensions[(int)code]);
   }
   else copystr(s, _fnExtensions[0]);

   return s;
}
