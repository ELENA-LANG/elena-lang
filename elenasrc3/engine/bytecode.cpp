//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                                (C)2021-2022, by Aleksey Rakov
//------------------------------------------------------------------------------

#include "bytecode.h"

using namespace elena_lang;

constexpr auto OPCODE_UNKNOWN = "unknown";

const char* _fnOpcodes[256] =
{
   "nop", "breakpoint", OPCODE_UNKNOWN, "redirect", "quit", "mov env", "load", "len",
   "class", "save", "throw", "unhook", "loadv", "xcmp", "bload", "wload",

   "incude", "exclude", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "set", "set dp", "nlen", "xassign", "peek", "store", "xswap sp", "swap sp",
   "mov mssg", "mov n", "load dp", "xcmp dp", "sub n", "add n", "set fp", OPCODE_UNKNOWN,

   "copy", "close", "alloc", "free", "and n", "read", "write", "cmp n",
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "save dp", "store fp", "save sp", "store sp", "xflush sp", "get", "assign", OPCODE_UNKNOWN,
   "peek fp", "peek sp", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "call", "call vt", "jump", "jeq", "jne", "jump vt", OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "cmp", OPCODE_UNKNOWN, "icmp", "tst flag", "tstn", "tst mssg", OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   "cmp fp", "cmp sp", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "copy dp", "iadd dp", "isub dp", "imul dp", "idiv dp", "nsave dp", "xhook dp", "xnewn",
   "nadd dp", OPCODE_UNKNOWN, "xwrite offs", "xcopy offs", "vjump mssg", "jump mssg", "seleq", "sellt",

   "open", "xstore sp", "open header", "mov sp", "new", "newn", "xmov sp", "createn",
   "create", "xstore fp", "xdispatch mssg", "dispatch mssg", "vcall mssg", "call mssg", "call extern", OPCODE_UNKNOWN
};

// --- Auxiliary  ---

void fixJumps(MemoryBase* code, int labelPosition, Map<int, int>& jumps, int label)
{
   Map<int, int>::Iterator it = jumps.start();
   while (!it.eof()) {
      if (it.key() == label) {
         int position = labelPosition - *it - 4;
         code->write(*it, &position, 4);
      }
      else if (it.key() == (int)(label | mskLabelRef)) {
         int position = labelPosition | mskLabelRef;
         code->write(*it, &position, 4);
      }
      ++it;
   }
}

// return true if there is a idle jump
inline bool fixIdleJumps(int label, int labelIndex, CachedMemoryMap<int, int, 40>& fixes, CachedMemoryMap<int, int, 40>& jumps)
{
   bool idleJump = false;

   CachedMemoryMap<int, int, 40>::Iterator it = fixes.start();
   while (!it.eof()) {
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

// --- ByteCodeUtil ---

ByteCode ByteCodeUtil :: code(ustr_t command)
{
   for (size_t i = 0; i <= 255; i++) {
      if (command.compare(_fnOpcodes[i]))
         return (ByteCode)i;
   }

   return ByteCode::None;
}

void ByteCodeUtil :: decode(ByteCode code, IdentifierString& target)
{
   target.append(_fnOpcodes[(int)code]);
}

void ByteCodeUtil :: formatMessageName(IdentifierString& messageName, ModuleBase* module, ustr_t actionName,
   ref_t* references, size_t len, pos_t argCount, ref_t flags)
{
   if (test(flags, STATIC_MESSAGE))
      messageName.append("static:");

   if (test(flags, FUNCTION_MESSAGE))
      messageName.append("function:");

   if (test(flags, PROPERTY_MESSAGE))
      messageName.append("prop:");

   if (test(flags, VARIADIC_MESSAGE))
      messageName.append("params:");

   messageName.append(actionName);
   if (len > 0) {
      messageName.append('<');
      
      for (size_t i = 0; i < len; i++) {
         if (i != 0)
            messageName.append(',');

         messageName.append(module->resolveReference(references[i]));
      }
      messageName.append('>');
   }

   if (argCount != 0) {
      messageName.append('[');
      messageName.appendInt(argCount);
      messageName.append(']');
   }
}

bool ByteCodeUtil :: resolveMessageName(IdentifierString& messageName, ModuleBase* module, mssg_t message)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   ref_t signature = 0;
   ustr_t actionName = module->resolveAction(actionRef, signature);
   if (emptystr(actionName))
      return false;

   ref_t references[ARG_COUNT];
   size_t len = signature ? module->resolveSignature(signature, references) : 0;

   formatMessageName(messageName, module, actionName, references, len, argCount, flags);

   return true;
}

mssg_t ByteCodeUtil :: resolveMessage(ustr_t messageName, ModuleBase* module, bool readOnlyMode)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0;

   if (messageName.startsWith("static:")) {
      flags |= STATIC_MESSAGE;
      messageName += getlength("static:");
   }
   if (messageName.startsWith("function:")) {
      flags |= FUNCTION_MESSAGE;
      messageName += getlength("function:");
   }
   if (messageName.startsWith("prop:")) {
      flags |= PROPERTY_MESSAGE;
      messageName += getlength("prop:");
   }
   if (messageName.startsWith("params:")) {
      flags |= VARIADIC_MESSAGE;
      messageName += getlength("params:");
   }

   IdentifierString actionName;
   size_t paramIndex = messageName.find('[');
   if (paramIndex != NOTFOUND_POS) {
      actionName.copy(messageName, paramIndex);

      String<char, 12> counterStr(messageName + paramIndex + 1, messageName.length() - paramIndex - 2);
      argCount = StrConvertor::toInt(counterStr.str(), 10);
   }
   else actionName.copy(messageName);

   if (actionName.compare(INVOKE_MESSAGE)) {
      flags |= FUNCTION_MESSAGE;
   }

   ref_t signature = 0;
   size_t index = (*actionName).find('<');
   if (index != NOTFOUND_POS) {
      ref_t references[ARG_COUNT];
      size_t end = (*actionName).find('>');
      size_t len = 0;
      size_t i = index + 1;
      while (i < end) {
         size_t j = (*actionName).findSub(i, ',', end);

         IdentifierString temp(actionName.str() + i, j - i);
         references[len++] = module->mapReference(*temp, true);

         i = j + 1;
      }

      signature = module->mapSignature(references, len, true);

      actionName.truncate(index);
   }

   if ((*actionName).compare(CAST_MESSAGE))
      flags |= CONVERSION_MESSAGE;

   actionRef = module->mapAction(*actionName, signature, readOnlyMode);
   if (actionRef == 0) {
      return 0;
   }

   return encodeMessage(actionRef, argCount, flags);
}

void ByteCodeUtil :: importCommand(ByteCommand& command, SectionScopeBase* target, ModuleBase* importer)
{
   if (isRCommand(command.code)) {
      ref_t mask = command.arg1 & mskAnyRef;
      switch (mask) {
         case mskMssgLiteralRef:
            command.arg1 = target->importMessageConstant(importer, command.arg1 & ~mskAnyRef) | mask;
            break;
         default:
            command.arg1 = target->importReference(importer, command.arg1 & ~mskAnyRef) | mask;
            break;
      }      
   }
   else if (isMCommand(command.code)) {
      command.arg1 = target->importMessage(importer, command.arg1);
   }

   if (isR2Command(command.code)) {
      ref_t mask = command.arg2 & mskAnyRef;
      command.arg2 = target->importReference(importer, command.arg2 & ~mskAnyRef) | mask;
   }
}

// --- CommandTape ---

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

inline bool removeIdleJump(ByteCodeIterator it)
{
   while (true) {
      ByteCommand command = *it;

      switch (command.code) {
         case ByteCode::Jump:
         case ByteCode::Jne:
         case ByteCode::Jeq:
         //case bcIfR:
         //case bcElseR:
         //case bcElseD:
         //case bcIf:
         //case bcIfCount:
         //case bcElse:
         //case bcNotLess:
         //case bcNotGreater:
         //case bcIfN:
         //case bcElseN:
         //case bcLessN:
         //case bcNotLessN:
         //case bcGreaterN:
         //case bcNotGreaterN:
         //   //case bcIfM:
         //   //case bcElseM:
         //   //case bcNext:
         //case bcIfHeap:
         case ByteCode::JumpMR:
         case ByteCode::VJumpMR:
         case ByteCode::JumpVI:
         //case bcJumpI:
            *it = ByteCode::Nop;
            it.flush();
            return true;
         default:
            break;
         }
         --it;
   }
   return false;
}

inline bool markAsReached(int blockStart, int blockEnd, CachedMemoryMap<int, int, 20>& blocks, CachedMemoryMap<int, int, 40>& jumps)
{
   bool applied = false;

   CachedMemoryMap<int, int, 40>::Iterator it = jumps.start();
   while (!it.eof()) {
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
   ++it;

   if (!it.eof()) {
      return it.key();
   }
   else return length;
}

inline bool optimizeProcJumps(ByteCodeIterator it)
{
   bool modified = false;

   auto start_it = it;

   CachedMemoryMap<int, int, 20> blocks(0); // value: 0 - unreached, -1 - reached, 1 - partial (reached if the previous one can be reached)
   CachedMemoryMap<int, int, 20> labels(-1);
   CachedMemoryMap<int, int, 40> jumps(0);
   CachedMemoryMap<int, int, 40> fixes(0);
   CachedMemoryMap<int, ByteCodeIterator, 20> idleLabels({}); // used to remove unused labels

   blocks.add(0, -1);

   // populate blocks and jump lists
   int index = 0;
   bool importMode = false;
   while (!it.eof()) {
      // skip pseudo commands (except labels)
      ByteCommand command = *it;

      if (command.code == ByteCode::ImportOn) {
         importMode = true;
      }
      else if (command.code == ByteCode::ImportOff) {
         importMode = false;
      }
      else if (importMode) {
         // ignore commands in the import mode
      }
      else if (command.code == ByteCode::Label) {
         labels.add(command.arg1, index);

         // add to idleLabels only if there are no forward jumps to it
         if (!fixes.exist(command.arg1)) {
            idleLabels.add(command.arg1, it);
         }

         // create partial block if it was not created
         if (!blocks.exist(index))
            blocks.add(index, 1);

         // fix forward jumps
         // if there is a idle jump it should be removed
         if (fixIdleJumps(command.arg1, index, fixes, jumps)) {
            modified |= removeIdleJump(it);
            // mark its block as partial
            *blocks.getIt(index) = 1;
         }
      }
      else if (command.code <= ByteCode::CallExtR && command.code >= ByteCode::Nop) {
         switch (command.code) {
            case ByteCode::Throw:
            case ByteCode::Quit:
            //case bcQuitN:
            case ByteCode::JumpMR:
            case ByteCode::VJumpMR:
            case ByteCode::JumpVI:
            //case ByteCode::JumpI:
               blocks.add(index + 1, 0);
               break;
            case ByteCode::Jump:
               blocks.add(index + 1, 0);
            case ByteCode::Jeq:
            case ByteCode::Jne:
            //case bcIfR:
            //case bcElseR:
            //case bcElseD:
            //case bcIf:
            //case bcIfCount:
            //case bcElse:
            //case bcNotLess:
            //case bcNotGreater:
            //case bcIfN:
            //case bcElseN:
            //case bcLessN:
            //case bcNotLessN:
            //case bcGreaterN:
            //case bcNotGreaterN:
               //            case bcIfM:
               //            case bcElseM:
               //            case bcNext:
            //case bcAddress:
            //case bcIfHeap:
               // remove the label from idle list
               idleLabels.exclude(command.arg1);

               addJump(command.arg1, index, labels, jumps, fixes);
               break;
            case ByteCode::XHookDPR:
               idleLabels.exclude(command.arg2 & ~mskLabelRef);

               addJump(command.arg2 & ~mskLabelRef, index, labels, jumps, fixes);
               break;
            default:
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
      while (!b_it.eof()) {
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
   while (!it.eof()) {
      ByteCommand command = *it;
      if (command.code == ByteCode::ImportOn) {
         importMode = true;
      }
      else if (command.code == ByteCode::ImportOff) {
         importMode = false;
      }

      bool isCommand = !importMode && (command.code <= ByteCode::CallExtR && command.code >= ByteCode::Nop);

      if (index == blockEnd) {
         b_it++;
         blockEnd = getBlockEnd(b_it, length);
      }

      // clear unreachable block
      // HOTFIX : do not remove ending breakpoint coordinates
      if (*b_it != -1 && command.code != ByteCode::None && command.code != ByteCode::Breakpoint) {
         (*it).code = ByteCode::None;
         it.flush();
         modified = true;
      }

      if (isCommand)
         index++;

      ++it;
   }

   // remove idle labels
   CachedMemoryMap<int, ByteCodeIterator, 20>::Iterator i_it = idleLabels.start();
   while (!i_it.eof()) {
      *(*i_it) = ByteCode::Nop;
      (*i_it).flush();

      modified = true;

      ++i_it;
   }

   return modified;
}

//bool CommandTape :: optimizeIdleBreakpoints(CommandTape& tape)
//{
//   bool modified = false;
//   bool idle = false;

   //ByteCodeIterator it = tape.start();
   //while (!it.Eof()) {
   //   int code = (*it).code;

   //   if (code == bcJump)
   //      idle = true;
   //   else if (code == bcBreakpoint) {
   //      if (idle) {
   //         (*it).code = bcNone;
   //      }
   //   }
   //   // HOTFIX : if there is a label before breakpoint
   //   // it should not be removed
   //   else if (code == blLabel) {
   //      idle = false;
   //   }
   //   else if (code <= bcCallExtR && code >= bcNop) {
   //      idle = false;
   //   }

   //   it++;
   //}

//   return modified;
//}

bool CommandTape :: optimizeJumps(CommandTape& tape)
{
   bool modified = optimizeProcJumps(tape.start());

   return modified;
}

int CommandTape :: resolvePseudoArg(PseudoArg argument)
{
   int realArg = 0;
   switch (argument) {
      case PseudoArg::CurrentLabel:
         realArg = labels.peek();
         break;
      default:
         break;
   }

   if (argument == PseudoArg::CurrentLabel) {
      realArg = labels.peek();
   }
   else if (argument == PseudoArg::FirstLabel) {
      realArg = *labels.end();
   }
   else if (argument == PseudoArg::PreviousLabel) {
      Stack<int>::Iterator it = labels.start();
      if (!it.eof()) {
         ++it;

         realArg = *it;
      }
   }
   else if (argument == PseudoArg::Prev2Label) {
      Stack<int>::Iterator it = labels.start();
      if (!it.eof()) {
         ++it;
         ++it;

         realArg = *it;
      }
   }
   return realArg;
}

void CommandTape :: write(ByteCode code)
{
   ByteCommand command(code);

   tape.add(command);
}

void CommandTape :: write(ByteCode code, arg_t arg1)
{
   ByteCommand command(code, arg1);

   tape.add(command);
}

void CommandTape :: write(ByteCode code, PseudoArg arg)
{
   write(code, resolvePseudoArg(arg));
}

void CommandTape :: write(ByteCode code, arg_t arg1, PseudoArg arg2)
{
   write(code, arg1, resolvePseudoArg(arg2));
}

void CommandTape::write(ByteCode code, arg_t arg1, PseudoArg arg2, ref_t mask)
{
   write(code, arg1, resolvePseudoArg(arg2) | mask);
}

void CommandTape :: write(ByteCode code, arg_t arg1, arg_t arg2)
{
   ByteCommand command(code, arg1, arg2);

   tape.add(command);
}

void CommandTape :: import(ModuleBase* sourceModule, MemoryBase* source, bool withHeader, SectionScopeBase* target)
{
   MemoryReader reader(source);

   if (withHeader)
      reader.getPos();

   ByteCommand command;
   while (!reader.eof()) {
      ByteCodeUtil::read(reader, command);

      ByteCodeUtil::importCommand(command, target, sourceModule);

      tape.add(command);
   }
}

void CommandTape :: saveTo(MemoryWriter* writer)
{
   Map<int, int> labels(0);
   Map<int, int> fwdJumps(0);
   bool importMode = false;

   for (auto it = tape.start(); !it.eof(); ++it) {
      auto command = *it;
      switch (command.code) {
         case ByteCode::ImportOn:
            importMode = true;
            break;
         case ByteCode::ImportOff:
            importMode = false;
            break;
         case ByteCode::Label:
            fixJumps(writer->Memory(), writer->position(), fwdJumps, command.arg1);
            labels.add(command.arg1, writer->position());

            // JIT compiler interprets nop command as a label mark
            ByteCodeUtil::write(*writer, { ByteCode::Nop });

            break;
         case ByteCode::Jump:
         case ByteCode::Jeq:
         case ByteCode::Jne:
            writer->writeByte((char)command.code);
            if (!importMode) {
               // if forward jump, it should be resolved later
               if (!labels.exist(command.arg1)) {
                  fwdJumps.add(command.arg1, writer->position());
                  // put jump offset place holder
                  writer->writeDWord(0);
               }
               // if backward jump
               else writer->writeDWord(labels.get(command.arg1) - writer->position() - 4);
            }
            else writer->writeDWord(command.arg1);

            if (command.code > ByteCode::MaxDoubleOp)
               writer->write(&command.arg2, sizeof(arg_t));

            break;
         case ByteCode::XHookDPR:
            writer->writeByte((char)command.code);
            writer->write(&command.arg1, sizeof(arg_t));

            if ((command.arg2 & mskAnyRef) == mskLabelRef) {
               // if forward jump, it should be resolved later
               if (!labels.exist(command.arg2)) {
                  fwdJumps.add(command.arg2, writer->position());
                  // put jump offset place holder
                  writer->writeDWord(mskLabelRef);
               }
               // if backward jump
               else writer->writeDWord(labels.get(command.arg2) - writer->position() - 4);
            }
            else writer->write(&command.arg2, sizeof(arg_t));

            break;
         default:
            if ((unsigned int)command.code < 0x100) {
               ByteCodeUtil::write(*writer, command);
            }
            break;
      }
   }
}
