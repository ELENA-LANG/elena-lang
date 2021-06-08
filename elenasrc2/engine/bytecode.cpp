//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implementation of ELENA byte code routines.
//
//                                                 (C)2009-2021, by Alexei Rakov
//------------------------------------------------------------------------------

#include "elena.h"
// -----------------------------------------------------------------------------
#include "bytecode.h"

constexpr auto OPCODE_UNKNOWN = "unknown";

const char* _fnOpcodes[256] =
{
   "nop", "breakpoint", "coalesce", "peek", "snop", "pushverb", "loadverb", "throw",
   "mcount", "push", "pusha", "popa", "xnew", "storev", "bsredirect", "setv",

   "not", "open", "pop", "sub", "swapd", "close", "rexp", "quit",
   "get", "set", "swap", "mquit", "count", "unhook", "rsin", "allocd",

   "rcos", "rarctan", "pushd", "popd", "xtrans", "include", "exclude", "trylock",
   "freelock", "freed", "loadenv", "store", "rln", "read", "clone", "xset",

   "rabs", "len", "rload", "flag", OPCODE_UNKNOWN, "parent", "class", "mindex",
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, "rround", "equal", OPCODE_UNKNOWN,

   "nequal", "nless", OPCODE_UNKNOWN, "lequal", "lless", "rset", "rsave", "save",
   "load", "rsaven", "rsavel", "lsave", "lload", OPCODE_UNKNOWN, OPCODE_UNKNOWN, "rint",

   "addf", "subf", "nxorf", "norf", "nandf", "movfipd", OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, "xsave", "div", "xwrite", "copyto", "nshlf", "nshrf",

   "mul", "checksi", "xredirect", "xvredirect", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, "laddf", "lsubf", "lmulf", "ldivf",
   "landf", "lorf", "lxorf", "lshlf", OPCODE_UNKNOWN, "lshrf", OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "raddnf", "rsubnf", "rmulnf", "requal", "rless", "raddf", "rsubf", "rmulf",
   "rdivf", "rdivnf", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, "rintf", OPCODE_UNKNOWN,

   "dec", "geti", "restore", "peekr", "peekfi", "peeksi", "ifheap", "xseti",
   OPCODE_UNKNOWN, "quitn", "create", "fillr", "movf", "movsip", "movr", "movm",

   "jump", "jumpvi", "callvi", "callr", "jumpi", "setframe", "hook", "address",
   "calli", OPCODE_UNKNOWN, "notless", "notgreater", "elsed", "if", "else", "ifcount",

   "pushn", "movn", "pushr", "equalfi", "pushai", "loadf", "pushfi", "loadfi",
   "loadsi", "savef", "pushsi", "savesi", "savefi", "pushf", "pushsip", "reserve",

   "seti", "movfip", "pushfip", "storesi", "storefi", "naddf", "nmulf", "xsetr",
   "nsubf", "ndivf", "loadi", "savei", "storer", "xor", "clonef", "xload",

   "freei", "alloci", "xcreate", "movv", "shl", "and", "inc", "or",
   "coalescer", "shr", "xsavelenf", "vjumprm", "xsaveai", "copyai", "move", "moveto",

   "readtof", "createn", "xsetfi", "copytoai", "copytofi", "copytof", "copyfi", "copyf",
   "mtredirect", "xmtredirect", "greatern", "notgreatern", "notlessn", "xrsavef", "xaddf", "xsavef",

   "new", "newn", "fillri", "xselectr", "vcallrm", "jumprm", "select", "lessn",
   "allocn", "xsavesi", "ifr", "elser", "ifn", "elsen", "callrm", "callextr",
};

using namespace _ELENA_;

inline ref_t importRef(_Module* sour, ref_t ref, _Module* dest)
{
   if (ref != 0 && ref != -1) {
      int mask = ref & mskAnyRef;
      if (mask == mskInt32Ref) {
         return importConstant(sour, ref & ~mskAnyRef, dest) | mask;
      }
      else return importReference(sour, ref & ~mskAnyRef, dest) | mask;
   }
   else return ref;
}

// --- CommandTape ---

bool CommandTape :: importReference(ByteCommand& command, _Module* sour, _Module* dest)
{
   if (ByteCodeCompiler::IsR2Code(command.code)) {
      command.additional = importRef(sour, (ref_t)command.additional, dest);
   }
   if (ByteCodeCompiler::IsM2Code(command.code)) {
      command.additional = importMessage(sour, (ref_t)command.additional, dest);
   }
   if (ByteCodeCompiler::IsRCode(command.code)) {
      command.argument = importRef(sour, (ref_t)command.argument, dest);
   }
   if (ByteCodeCompiler::IsMCode(command.code)) {
      command.argument = importMessage(sour, (ref_t)command.argument, dest);
   }
   if (ByteCodeCompiler::IsMNCode(command.code)) {
      command.argument = getAction(importMessage(sour, encodeAction((ref_t)command.argument), dest));
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

void CommandTape :: import(_Memory* section, bool withHeader, bool withBreakpoints)
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
      else if (code == bcBreakpoint) {
         if (withBreakpoints) {
            write(code);
            write(bdBreakpoint, dsAssemblyStep);
         }
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
         //case bcIfB:
         case bcElseD:
         case bcIf:
         case bcIfCount:
         case bcElse:
         //case bcLess:
         case bcNotLess:
         case bcNotGreater:
         case bcIfN:
         case bcElseN:
         case bcLessN:
         case bcNotLessN:
         case bcGreaterN:
         case bcNotGreaterN:
         //case bcIfM:
         //case bcElseM:
         //case bcNext:
         case bcIfHeap:
         case bcJumpRM:
         case bcVJumpRM:
         case bcJumpI:
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
      else if (code <= bcCallExtR && code >= bcNop) {
         switch(code) {
            case bcThrow:
            //case bcJumpAcc:
            case bcQuit:
////            case bcMQuit:
            case bcQuitN:
            case bcJumpVI:
            case bcJumpRM:
            case bcVJumpRM:
            case bcJumpI:
               blocks.add(index + 1, 0);
               break;
            case bcJump:
               blocks.add(index + 1, 0);
            case bcIfR:
            case bcElseR:              
//            case bcIfB:
            case bcElseD:              
            case bcIf:
            case bcIfCount:
            case bcElse:              
//            case bcLess:
            case bcNotLess:
            case bcNotGreater:
            case bcIfN:
            case bcElseN:              
            case bcLessN:
            case bcNotLessN:
            case bcGreaterN:
            case bcNotGreaterN:
//            case bcIfM:
//            case bcElseM:              
//            case bcNext:
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
   while (*it != blEnd || ((*it).argument != bsMethod && (*it).argument != bsSymbol)) {
      ByteCode code = *it;
      bool command = (code <= bcCallExtR && code >= bcNop);

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
      else if (code <= bcCallExtR && code >= bcNop) {
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
   return test(*it, blLabelMask) || (*it <= bcCallExtR && *it > bcBreakpoint);
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
         case braAditionalValue:
            (*trans_it).additional = pattern.argument;
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

bool TransformTape :: makeStep(Node& step, ByteCommand& command, int previousArg)
{
   Node::ChildEnumerator child = step.Children();
   Node defaultNode(&trie);

   while (!child.Eof()) {
      Node current = child.Node();
      ByteCodePattern pattern = current.Value();
      if (pattern.argumentType == braSame) {
         pattern.argument ^= (previousArg == command.argument) ? 1 : 0;
      }
      else if (pattern.argumentType == braAdditionalSame) {
         pattern.argument ^= (previousArg == command.additional) ? 1 : 0;
      }
      if (pattern == command) {
         step = current;

         return true;
      }
      else if (pattern.code == bcMatch) {
         step = current;

         return true;
      }

      child++;
   }

   if (defaultNode != step) {
      step = defaultNode;

      return makeStep(step, command, previousArg);
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
         if (makeStep(current, *it, 0)) {
            int previousArg = (*it).argument;
            it++;

            ByteCodeIterator word_it = it;
            while (!word_it.Eof() && (!matchable(word_it) || makeStep(current, *word_it, previousArg))) {
               if(matchable(word_it))
                  previousArg = (*word_it).argument;

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

void ByteCodeCompiler :: loadOperators(MessageMap& operators, MessageMap& unaryOperators)
{
   addVerb(operators, ADD_OPERATOR, ADD_OPERATOR_ID);
   addVerb(operators, SUB_OPERATOR, SUB_OPERATOR_ID);
   addVerb(operators, MUL_OPERATOR, MUL_OPERATOR_ID);
   addVerb(operators, DIV_OPERATOR, DIV_OPERATOR_ID);
   addVerb(operators, IF_OPERATOR, IF_OPERATOR_ID);
   addVerb(operators, IFNOT_OPERATOR, IFNOT_OPERATOR_ID);
   addVerb(operators, EQUAL_OPERATOR, EQUAL_OPERATOR_ID);
   addVerb(operators, NOTEQUAL_OPERATOR, NOTEQUAL_OPERATOR_ID);
   addVerb(operators, LESS_OPERATOR, LESS_OPERATOR_ID);
   addVerb(operators, GREATER_OPERATOR, GREATER_OPERATOR_ID);
   addVerb(operators, NOTLESS_OPERATOR, NOTLESS_OPERATOR_ID);
   addVerb(operators, NOTGREATER_OPERATOR, NOTGREATER_OPERATOR_ID);
   addVerb(operators, AND_OPERATOR, AND_OPERATOR_ID);
   addVerb(operators, OR_OPERATOR, OR_OPERATOR_ID);
   addVerb(operators, XOR_OPERATOR, XOR_OPERATOR_ID);
   addVerb(operators, BAND_OPERATOR, BAND_OPERATOR_ID);
   addVerb(operators, BOR_OPERATOR, BOR_OPERATOR_ID);
   addVerb(operators, BXOR_OPERATOR, BXOR_OPERATOR_ID);
   addVerb(operators, APPEND_OPERATOR, APPEND_OPERATOR_ID);
   addVerb(operators, BAPPEND_OPERATOR, BAPPEND_OPERATOR_ID);
   addVerb(operators, REDUCE_OPERATOR, REDUCE_OPERATOR_ID);
   addVerb(operators, INCREASE_OPERATOR, INCREASE_OPERATOR_ID);
   addVerb(operators, BINCREASE_OPERATOR, BINCREASE_OPERATOR_ID);
   addVerb(operators, SEPARATE_OPERATOR, SEPARATE_OPERATOR_ID);
   addVerb(operators, SHIFTR_OPERATOR, SHIFTR_OPERATOR_ID);
   addVerb(operators, SHIFTL_OPERATOR, SHIFTL_OPERATOR_ID);
   addVerb(operators, LEN_OPERATOR, LEN_OPERATOR_ID);
   addVerb(operators, ISNIL_OPERATOR, ISNIL_OPERATOR_ID);
   addVerb(operators, CATCH_OPERATOR, CATCH_OPERATOR_ID);
   addVerb(operators, ALT_OPERATOR, ALT_OPERATOR_ID);
   addVerb(operators, FINALLY_OPERATOR, FINALLY_OPERATOR_ID);

   addVerb(unaryOperators, INVERTED_OPERATOR, INVERTED_OPERATOR_ID);
   addVerb(unaryOperators, BINVERTED_OPERATOR, BINVERTED_OPERATOR_ID);
   addVerb(unaryOperators, NEGATIVE_OPERATOR, NEGATIVE_OPERATOR_ID);
   addVerb(unaryOperators, VALUE_OPERATOR, VALUE_OPERATOR_ID);
}

ByteCode ByteCodeCompiler :: code(ident_t s)
{
   for(int i = 0 ; i <= 255 ; i++) {
      if (s.compare(_fnOpcodes[i])) {
         return (ByteCode)i;
      }
   }

   return bcNone;
}

ident_t ByteCodeCompiler :: decode(ByteCode code, char* s)
{
   copystr(s, _fnOpcodes[(int)code]);

   return s;
}


bool ByteCodeCompiler :: resolveMessageName(IdentifierString& messageName, _Module* module, mssg_t messageRef)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(messageRef, actionRef, argCount, flags);

   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      messageName.append("params#");
   }
   if ((flags & PREFIX_MESSAGE_MASK) == PROPERTY_MESSAGE) {
      messageName.append("prop#");
   }

   if (test(messageRef, STATIC_MESSAGE)) {
      messageName.append("#static&");
   }

   //   if (test(reference, SPECIAL_MESSAGE)) {         
   //   }
   //   else {
   //      if (test(reference, SEALED_MESSAGE)) {
   //         command.append("#static&");
   //      }
   //      if (test(reference, PROPSET_MESSAGE)) {
   //         command.append("set&");
   //      }
   //   }
   ref_t signature = 0;
   ident_t actionName = module->resolveAction(actionRef, signature);
   if (emptystr(actionName))
      return false;

   if (actionName.compare(GENERIC_PREFIX) && (flags & PREFIX_MESSAGE_MASK) == CONVERSION_MESSAGE)
      messageName.append("#cast&");

   messageName.append(actionName);
   if (signature) {
      ref_t references[ARG_COUNT];

      messageName.append('<');
      size_t len = module->resolveSignature(signature, references);
      for (size_t i = 0; i < len; i++) {
         if (i != 0)
            messageName.append(',');

         messageName.append(module->resolveReference(references[i]));
      }
      messageName.append('>');
   }

   if (argCount > 0) {
      messageName.append('[');
      messageName.appendInt(argCount);
      messageName.append(']');
   }

   return true;
}
