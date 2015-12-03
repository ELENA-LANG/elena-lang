//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implementation of ELENA byte code routines.
//
//                                                 (C)2009-2015, by Alexei Rakov
//------------------------------------------------------------------------------

#include "elena.h"
// -----------------------------------------------------------------------------
#include "bytecode.h"

#define OPCODE_UNKNOWN      "unknown"

const char* _fnOpcodes[256] =
{
   "nop", "breakpoint", "pushb", "pop", "snop", "pushe", "dcopyverb", "throw",
   "dcopycount", "or", "pusha", "popa", "acopyb", "pope", "bsredirect", "dcopysubj",

   "not", "len", "bcopya", "dec", "popb", "close", "sub", "quit",
   "get", "set", "inc", "equit", "unknown", "unhook", "add", "create",

   "ecopyd", "dcopye", "pushd", "popd", "dreserve", "drestore", "exclude", "trylock",
   "freelock", "unknown", "unknown", "unknown", "eswap", "bswap", "copy", "xset",

   "xlen", "blen", "wlen", "flag", "nlen", "unknown", "class", "mindex",
   "call", "acallvd", "validate", "unknown", "unknown", "unknown", "unknown", "unknown",

   "nequal", "nless", "ncopy", "nadd", "nsub", "nmul", "ndiv", "nsave",
   "nload", "dcopyr", "nand", "nor", "nxor", "nshift", "nnot", "ncreate",

   "ncopyb", "lcopyb", "copyb", "unknown", "unknown", "unknown", "unknown", "unknown",
   "unknown", "wread", "wwrite", "nread", "nwrite", "unknown", "unknown", "wcreate",

   "breadw", "bread", "unknown", "unknown", "unknown", "breadb", "rsin", "rcos",
   "rarctan", "bwrite", "unknown", "unknown", "bwriteb", "bwritew", "unknown", "bcreate",

   "lcopy", "unknown", "lequal", "lless", "ladd", "lsub", "lmul", "ldiv",
   "land", "lor", "lxor", "lshift", "lnot", "unknown", "unknown", "unknown",

   "rcopy", "unknown", "rsave", "requal", "rless", "radd", "rsub", "rmul",
   "rdiv", "unknown", "rexp", "rln", "rabs", "rround", "rint", "rload",

   "dcopy", "ecopy", "restore", "aloadr", "aloadfi", "aloadsi", "ifheap", "bcopys",
   "open", "quitn", "bcopyr", "bcopyf", "acopyf", "acopys", "acopyr", "copym",

   "jump", "ajumpvi", "acallvi", "callr", "unknown", "callextr", "hook", "address",
   "unknown", "less", "notless", "ifb", "elseb", "if", "else", "next",

   "pushn", "eloadfi", "pushr", "unknown", "pushai", "esavefi", "pushfi", "dloadfi",
   "dloadsi", "dsavefi", "pushsi", "dsavesi", "eloadsi", "pushf", "esavesi", "reserve",

   "asavebi", "unknown", "aswapsi", "asavesi", "asavefi", "bswapsi", "eswapsi", "dswapsi",
   "bloadfi", "bloadsi", "nloadi", "nsavei", "asaver", "aloadai", "aloadbi", "axsavebi",

   "popi", "unknown", "scopyf", "setverb", "setsubj", "andn", "addn", "orn",
   "eaddn", "shiftn", "muln", "unknown", "unknown", "unknown", "unknown", "unknown",

   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
   "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",

   "new", "newn", "unknown", "xselectr", "xindexrm", "xjumprm", "selectr", "lessn",
   "ifm", "elsem", "ifr", "elser", "ifn", "elsen", "xcallrm", "unknown"
};

using namespace _ELENA_;

inline ref_t importRef(_Module* sour, size_t ref, _Module* dest)
{
   if (ref != 0) {
      int mask = ref & mskAnyRef;

      ident_t referenceName = sour->resolveReference(ref & ~mskAnyRef);

      return dest->mapReference(referenceName) | mask;
   }
   else return 0;
}

// --- CommandTape ---

bool CommandTape :: import(ByteCommand& command, _Module* sour, _Module* dest)
{
   if (ByteCodeCompiler::IsR2Code(command.code)) {
      command.additional = importRef(sour, (ref_t)command.additional, dest);
   }
   if (ByteCodeCompiler::IsRCode(command.code)) {
      command.argument = importRef(sour, (ref_t)command.argument, dest);
   }
   return true;
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

   Map<int, int> extLabels;

   MemoryReader reader(section);

   if (withHeader)
      reader.getDWord();

   while (!reader.Eof()) {
      argument = 0;
      additional = 0;

      code = (ByteCode)reader.getByte();
      if (code > MAX_SINGLE_ECODE) {
         argument = reader.getDWord();
      }
      if (code > MAX_DOUBLE_ECODE) {  
         additional = reader.getDWord();
      }
      if (ByteCodeCompiler::IsJump(code)) {
         int position = 0;
         int label = 0;
         if (code > MAX_DOUBLE_ECODE) {
            position = additional + reader.Position();
         }
         else position = argument + reader.Position();

         if (!extLabels.exist(position)) {
            label = ++labelSeed;
            extLabels.add(position, label);
         }
         else label = extLabels.get(position);

         if (code > MAX_DOUBLE_ECODE) {
            write(code, label, argument);
         }
         else write(code, label);
      }
      else if (code == bcNop) {
         int label;
         if (!extLabels.exist(reader.Position() - 1)) {
            label = ++labelSeed;
            extLabels.add(reader.Position() - 1, label);
         }
         else label = extLabels.get(reader.Position() - 1);

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

inline bool removeIdleJump(ByteCodeIterator it)
{
   while (true) {
      switch((ByteCode)*it) {
         case bcJump:
         case bcIfR:
         case bcElseR:
         case bcIfB:
         case bcElseB:
         case bcIf:
         case bcElse:
         case bcLess:
         case bcNotLess:
         case bcIfN:
         case bcElseN:
         case bcLessN:
         case bcIfM:
         case bcElseM:
         case bcNext:
         case bcIfHeap:
         case bcXJumpRM:
            *it = bcNop;
            return true;
      }
      it--;
   }
   return false;
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
   while (*it != blEnd || ((*it).argument != bsMethod && (*it).argument != bsSymbol)) {
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
            modified |= removeIdleJump(it);
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
            case bcXJumpRM:
               blocks.add(index + 1, 0);
               break;
            case bcJump:
               blocks.add(index + 1, 0);
            case bcIfR:
            case bcElseR:              
            case bcIfB:
            case bcElseB:              
            case bcIf:
            case bcElse:              
            case bcLess:
            case bcNotLess:
            case bcIfN:
            case bcElseN:              
            case bcLessN:
            case bcIfM:
            case bcElseM:              
            case bcNext:
            case bcHook:
            case bcAddress:
            case bcIfHeap:
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
      // HOTFIX : do not remove ending breakpoint coordinates
      if (*b_it != -1 && code != bcNone && code != bdBreakcoord && code != bdBreakpoint) {
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
      modified = true;

      i_it++;
   }

   return modified;
}

bool CommandTape :: optimizeIdleBreakpoints(CommandTape& tape)
{
   bool modified = false;
   bool idle = false;

   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      int code = (*it).code;

      if (code == bcJump)
         idle = true;
      else if (code == bcBreakpoint) {
         if (idle) {
            (*it).code = bcNone;
         }
      }
      // HOTFIX : if there is a label before breakpoint
      // it should not be removed
      else if (code == blLabel) {
         idle = false;
      }
      else if (code <= bcReserved && code >= bcNop) {
         idle = false;
      }

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

inline bool matchable(ByteCodeIterator& it)
{
   return test(*it, blLabelMask) || (*it < bcReserved && *it > bcBreakpoint);
}

void TransformTape :: transform(ByteCodeIterator& trans_it, Node replacement)
{
   ByteCodeIterator target_it = trans_it;

   ByteCodePattern pattern = replacement.Value();
   while (pattern.code != bcNone) {
      // skip meta commands (except label)
      while (!matchable(trans_it))
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
   map.add(verb, id);
}

void ByteCodeCompiler :: loadVerbs(MessageMap& verbs)
{
   // load verbs
   addVerb(verbs, NEW_MESSAGE,        NEW_MESSAGE_ID);
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
   addVerb(verbs, IF_MESSAGE,         IF_MESSAGE_ID);
   addVerb(verbs, FIND_MESSAGE,       FIND_MESSAGE_ID);
   addVerb(verbs, SEEK_MESSAGE,       SEEK_MESSAGE_ID);
   addVerb(verbs, REWIND_MESSAGE,     REWIND_MESSAGE_ID);
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
   addVerb(verbs, RETRIEVE_MESSAGE,   RETRIEVE_MESSAGE_ID);
   addVerb(verbs, CAST_MESSAGE,       CAST_MESSAGE_ID);
   addVerb(verbs, RESUME_MESSAGE,     RESUME_MESSAGE_ID);
   addVerb(verbs, OPEN_MESSAGE,       OPEN_MESSAGE_ID);
   addVerb(verbs, EXIT_MESSAGE,       EXIT_MESSAGE_ID);
   addVerb(verbs, SHOW_MESSAGE,       SHOW_MESSAGE_ID);
   addVerb(verbs, HIDE_MESSAGE,       HIDE_MESSAGE_ID);
   addVerb(verbs, CREATE_MESSAGE,     CREATE_MESSAGE_ID);
   addVerb(verbs, IS_MESSAGE,         IS_MESSAGE_ID);
   addVerb(verbs, ROLLBACK_MESSAGE,   ROLLBACK_MESSAGE_ID);
   addVerb(verbs, SELECT_MESSAGE,     SELECT_MESSAGE_ID);
   addVerb(verbs, REPLACE_MESSAGE,    REPLACE_MESSAGE_ID);
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

ByteCode ByteCodeCompiler :: code(ident_t s)
{
   for(int i = 0 ; i < 255 ; i++) {
      if (StringHelper::compare(s, _fnOpcodes[i])) {
         return (ByteCode)i;
      }
   }

   return bcNone;
}

ident_t ByteCodeCompiler :: decode(ByteCode code, ident_c* s)
{
   copystr(s, _fnOpcodes[(int)code]);

   return s;
}

//FunctionCode ByteCodeCompiler :: codeFunction(const wchar16_t* s)
//{
//   for(int i = 1 ; i < EXTENSION_COUNT ; i++) {
//      if (ConstantIdentifier::compare(s, _fnExtensions[i])) {
//         return (FunctionCode)i;
//      }
//   }
//
//   return fnUnknown;
//}

//const wchar16_t* ByteCodeCompiler :: decodeFunction(FunctionCode code, wchar16_t* s)
//{
//   size_t key = (size_t)code;
//
//   if (key < EXTENSION_COUNT) {
//      copystr(s, _fnExtensions[(int)code]);
//   }
//   else copystr(s, _fnExtensions[0]);
//
//   return s;
//}
