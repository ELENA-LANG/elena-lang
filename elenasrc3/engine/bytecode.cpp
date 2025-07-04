//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                                (C)2021-2025, by Aleksey Rakov
//------------------------------------------------------------------------------

#include "bytecode.h"

using namespace elena_lang;

constexpr auto OPCODE_UNKNOWN = "unknown";

const char* _fnOpcodes[256] =
{
   "nop", "breakpoint", "snop", "redirect", "quit", "mov env", "load", "len",
   "class", "save", "throw", "unhook", "loadv", "xcmp", "bload", "wload",

   "exclude", "include", "assign", "mov frm", "loads", "mlen", "dalloc", "tststck",
   "dtrans", "xassign", "lload", "convl", "xlcmp", "xload", "xlload", "lneg",

   "coalesce", "not", "neg", "bread", "lsave", "fsave", "wread", "xjump",
   "bcopy", "wcopy", "xpeekeq", "trylock", "freelock", "parent", "xget", "xcall",

   "xfsave", "altmode", "xnop", OPCODE_UNKNOWN, "xquit", "dfree", OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "fiadd", "fisub", "fimul", "fidiv", OPCODE_UNKNOWN, "shl", "shr", "xsave n",
   "fabs dp", "fsqrt dp", "fexp dp", "fln dp", "fsin dp", "fcos dp", "farctan dp", "fpi dp",

   "set", "set dp", "nlen", "xassign i", "peek", "store", "xswap sp", "swap sp",
   "mov mssg", "mov n", "load dp", "xcmp dp", "sub n", "add n", "set fp", "create",

   "copy", "close", "alloc i", "free i", "and n", "read", "write", "cmp n",
   "nconf dp", "ftrunc dp", "dcopy", "or n", "mul n", "xadd dp", "xset fp", "fround dp",

   "save dp", "store fp", "save sp", "store sp", "xflush sp", "get i", "assign i", "xrefresh sp",
   "peek fp", "peek sp", "lsave dp", "lsave sp", "lload dp", "xfill", "xstore i", "set sp",

   "call", "call vt", "jump", "jeq", "jne", "jump vt", "xredirect mssg", "jlt",
   "jge", "jgr", "jle", "peek tls", "store tls", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "cmp", "fcmp", "icmp", "tst flag", "tst n", "tst mssg", "xcmp sp", OPCODE_UNKNOWN,
   "cmp fp", "cmp sp", "extclose", "lload sp", "load sp", "xloadarg fp", "xcreate", "system",

   "fadd dp", "fsub dp", "fmul dp", "fdiv dp", "udiv dp", "xsave disp", "xlabel dp", "selgr",
   "iand dp", "ior dp", "ixor dp", "inot dp", "ishl dp", "ishr dp", "xopen", "selult",

   "copy dp", "iadd dp", "isub dp", "imul dp", "idiv dp", "nsave dp", "xhook dp", "xnewn",
   "nadd dp", "dcopy dp", "xwrite offs", "xcopy offs", "vjump mssg", "jump mssg", "seleq", "sellt",

   "open", "xstore sp", "extopen", "mov sp", "new", "newn", "xmov sp", "createn",
   "fillir", "xstore fp", "xdispatch", "dispatch mssg", "vcall mssg", "call mssg", "call extern", OPCODE_UNKNOWN
};

const ByteCode opNotUsingAcc[] = { 
   ByteCode::Nop, ByteCode::Breakpoint, ByteCode::SNop, ByteCode::MovEnv, ByteCode::Unhook, ByteCode::Exclude, ByteCode::Include, ByteCode::MovFrm, ByteCode::MLen, ByteCode::DAlloc, 
   ByteCode::ConvL, ByteCode::LNeg, ByteCode::Not, ByteCode::Neg, ByteCode::AltMode, ByteCode::XNop, ByteCode::XQuit, ByteCode::Shl, ByteCode::Shr, ByteCode::FAbsDP, 
   ByteCode::FSqrtDP, ByteCode::FExpDP, ByteCode::FLnDP, ByteCode::FSinDP, ByteCode::FCosDP, ByteCode::FArctanDP, ByteCode::FPiDP, ByteCode::XSwapSI, ByteCode::MovM, ByteCode::MovN, 
   ByteCode::LoadDP, ByteCode::XCmpDP, ByteCode::SubN, ByteCode::AddN, ByteCode::CloseN, ByteCode::AllocI, ByteCode::FreeI, ByteCode::AddN, ByteCode::CmpN, ByteCode::FTruncDP, 
   ByteCode::OrN, ByteCode::MulN, ByteCode::XAddDP, ByteCode::FRoundDP, ByteCode::SaveDP, ByteCode::SaveSI, ByteCode::XFlushSI, ByteCode::XRefreshSI, ByteCode::LSaveDP, ByteCode::LSaveSI, 
   ByteCode::LLoadDP, ByteCode::TstM, ByteCode::TstN, ByteCode::XCmpSI, ByteCode::ExtCloseN, ByteCode::LLoadSI, ByteCode::LoadSI, ByteCode::XLoadArgFI, ByteCode::FAddDPN, ByteCode::FSubDPN, 
   ByteCode::FMulDPN, ByteCode::FDivDPN, ByteCode::UDivDPN, ByteCode::XLabelDPR, ByteCode::IAndDPN, ByteCode::IOrDPN, ByteCode::IXorDPN, ByteCode::INotDPN, ByteCode::IShlDPN, ByteCode::IShrDPN, 
   ByteCode::XOpenIN, ByteCode::CopyDPN, ByteCode::IAddDPN, ByteCode::ISubDPN, ByteCode::IMulDPN, ByteCode::IDivDPN, ByteCode::NSaveDPN, ByteCode::XHookDPR, ByteCode::NAddDPN, ByteCode::DCopyDPN, 
   ByteCode::OpenIN, ByteCode::XStoreSIR, ByteCode::ExtOpenIN, ByteCode::MovSIFI, ByteCode::XMovSISI, ByteCode::XStoreFIR, ByteCode::CallExtR, ByteCode::DFree
};

const ByteCode opSetAcc[] = {
   ByteCode::SetR, ByteCode::SetDP, ByteCode::PeekR, ByteCode::SetFP, ByteCode::CreateR, ByteCode::XSetFP, ByteCode::PeekFI, ByteCode::PeekSI, ByteCode::SetSP, 
   ByteCode::PeekTLS, ByteCode::XCreateR, ByteCode::SelGrRR, ByteCode::NewIR, ByteCode::NewNR, ByteCode::CreateNR, 
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

   return command.compare("idle") ? ByteCode::Idle : ByteCode::None;
}

void ByteCodeUtil :: decode(ByteCode code, IdentifierString& target)
{
   target.append(_fnOpcodes[(int)code]);
}

inline void addMessageNamePrefix(IdentifierString& messageName, ref_t flags)
{
   if (test(flags, STATIC_MESSAGE))
      messageName.append("static:");

   if (test(flags, FUNCTION_MESSAGE))
      messageName.append("function:");

   switch (flags & PREFIX_MESSAGE_MASK) {
      case PROPERTY_MESSAGE:
         messageName.append("prop:");
         break;
      case VARIADIC_MESSAGE:
         messageName.append("params:");
         break;
      case CONVERSION_MESSAGE:
         messageName.append("typecast:");
         break;
      default:
         break;
   }
}

inline void addMessageNamePostfix(IdentifierString& messageName, pos_t argCount)
{
   if (argCount != 0) {
      messageName.append('[');
      messageName.appendInt(argCount);
      messageName.append(']');
   }
}

void ByteCodeUtil :: formatMessageName(IdentifierString& messageName, ModuleBase* module, ustr_t actionName,
   ref_t* references, size_t len, pos_t argCount, ref_t flags)
{
   addMessageNamePrefix(messageName, flags);

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

   addMessageNamePostfix(messageName, argCount);
}

void ByteCodeUtil :: formatMessageNameWithNullableArgs(IdentifierString& messageName, ModuleBase* module, ustr_t actionName,
   ref_t* references, size_t len, pos_t argCount, ref_t flags, int nullableArgs)
{
   addMessageNamePrefix(messageName, flags);

   messageName.append(actionName);
   if (len > 0) {
      messageName.append('<');

      int currentArg = 1;
      for (size_t i = 0; i < len; i++) {
         if (i != 0)
            messageName.append(',');

         messageName.append(module->resolveReference(references[i]));

         if (test(nullableArgs, currentArg))
            messageName.append('?');

         currentArg <<= 1;
      }
      messageName.append('>');
   }

   addMessageNamePostfix(messageName, argCount);
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

bool ByteCodeUtil :: resolveMessageNameWithNullableArgs(IdentifierString& messageName, ModuleBase* module, mssg_t message, int nullableArgs)
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

   formatMessageNameWithNullableArgs(messageName, module, actionName, references, len, argCount, flags, nullableArgs);

   return true;
}

void ByteCodeUtil :: parseMessageName(ustr_t messageName, IdentifierString& actionName, ref_t& flags, pos_t& argCount)
{
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
   if (messageName.startsWith("typecast:")) {
      flags |= CONVERSION_MESSAGE;
      messageName += getlength("typecast:");
   }

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
}

mssg_t ByteCodeUtil :: resolveMessage(ustr_t messageName, ModuleBase* module, bool readOnlyMode)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0;

   IdentifierString actionName;
   parseMessageName(messageName, actionName, flags, argCount);

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
         references[len++] = module->mapReference(*temp, readOnlyMode);

         i = j + 1;
      }

      signature = module->mapSignature(references, len, readOnlyMode);

      actionName.truncate(index);
   }

   actionRef = module->mapAction(*actionName, signature, readOnlyMode);
   if (actionRef == 0) {
      return 0;
   }

   return encodeMessage(actionRef, argCount, flags);
}

mssg_t ByteCodeUtil :: resolveMessageName(ustr_t messageName, ModuleBase* module, bool readOnlyMode)
{
   pos_t argCount = 0;
   ref_t flags = 0;

   IdentifierString actionName;
   parseMessageName(messageName, actionName, flags, argCount);

   ref_t actionRef = module->mapAction(*actionName, 0, readOnlyMode);
   if (actionRef == 0) {
      return 0;
   }

   return encodeMessage(actionRef, argCount, flags);
}

inline ref_t importRArg(ref_t arg, ModuleBase* exporter, ModuleBase* importer)
{
   if (arg != INVALID_REF) {
      ref_t mask = arg & mskAnyRef;
      switch (mask) {
         case mskMssgLiteralRef:
         case mskMssgNameLiteralRef:
         case mskPropNameLiteralRef:
            return ImportHelper::importMessageConstant(exporter, arg & ~mskAnyRef, importer) | mask;
            break;
         case mskExtMssgLiteralRef:
            return ImportHelper::importExtMessageConstant(exporter, arg & ~mskAnyRef, importer) | mask;
            break;
         case mskExternalRef:
            return ImportHelper::importExternal(exporter, arg & ~mskAnyRef, importer) | mask;
            break;
         default:
            return ImportHelper::importReference(exporter, arg & ~mskAnyRef, importer) | mask;
            break;
      }
   }
   return arg;
}

void ByteCodeUtil :: importCommand(ByteCommand& command, ModuleBase* exporter, ModuleBase* importer)
{
   if (isRCommand(command.code)) {
      command.arg1 = importRArg(command.arg1, exporter, importer);
   }
   else if (isMCommand(command.code)) {
      command.arg1 = ImportHelper::importMessage(exporter, command.arg1, importer);
   }

   if (isR2Command(command.code)) {
      command.arg2 = importRArg(command.arg2, exporter, importer);
   }
}

void ByteCodeUtil :: generateAutoSymbol(ModuleInfoList& symbolList, ModuleBase* module, MemoryDump& tapeSymbol, bool withExtFrame)
{
   MemoryWriter writer(&tapeSymbol);

   pos_t sizePlaceholder = writer.position();
   writer.writePos(0);

   ByteCodeUtil::write(writer, withExtFrame ? ByteCode::ExtOpenIN : ByteCode::OpenIN, 2, 0);

   // generate the preloaded list
   for (auto it = symbolList.start(); !it.eof(); ++it) {
      auto info = *it;
      ustr_t symbolName = info.module->resolveReference(info.reference);
      if (isWeakReference(symbolName)) {
         IdentifierString fullName(info.module->name(), symbolName);

         ByteCodeUtil::write(writer, ByteCode::CallR, module->mapReference(*fullName) | mskSymbolRef);
      }
      else ByteCodeUtil::write(writer, ByteCode::CallR, module->mapReference(symbolName) | mskSymbolRef);
   }

   ByteCodeUtil::write(writer, withExtFrame ? ByteCode::ExtCloseN : ByteCode::CloseN);
   ByteCodeUtil::write(writer, ByteCode::Quit);

   pos_t size = writer.position() - sizePlaceholder - sizeof(pos_t);

   writer.seek(sizePlaceholder);
   writer.writePos(size);
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
         case ByteCode::Jlt:
         case ByteCode::Jle:
         case ByteCode::Jgr:
         case ByteCode::Jge:
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
      else if (command.code <= ByteCode::CallExtR && command.code >= ByteCode::Nop
         && command.code != ByteCode::Breakpoint)
      {
         switch (command.code) {
            case ByteCode::Throw:
            case ByteCode::Quit:
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
            case ByteCode::Jlt:
            case ByteCode::Jle:
            case ByteCode::Jge:
            case ByteCode::Jgr:
               // remove the label from idle list
               idleLabels.exclude(command.arg1);

               addJump(command.arg1, index, labels, jumps, fixes);
               break;
            case ByteCode::XHookDPR:
            case ByteCode::XLabelDPR:
               idleLabels.exclude(command.arg2 & ~mskLabelRef);

               addJump(command.arg2 & ~mskLabelRef, index, labels, jumps, fixes);
               break;
            default:
               break;
         }

         index++;
      }
      ++it;
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

      bool isCommand = !importMode && (command.code <= ByteCode::CallExtR && command.code >= ByteCode::Nop
         && command.code != ByteCode::Breakpoint);

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

void CommandTape :: write(ByteCode code, arg_t arg1, int arg2, ref_t mask)
{
   write(code, arg1, arg2 | mask);
}

void CommandTape :: write(ByteCode code, arg_t arg1, arg_t arg2)
{
   ByteCommand command(code, arg1, arg2);

   tape.add(command);
}

void CommandTape :: import(ModuleBase* sourceModule, MemoryBase* source, bool withHeader, ModuleBase * targetModule)
{
   MemoryReader reader(source);

   if (withHeader)
      reader.getPos();

   ByteCommand command;
   while (!reader.eof()) {
      ByteCodeUtil::read(reader, command);

      ByteCodeUtil::importCommand(command, sourceModule, targetModule);

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
         //case ByteCode::BreakLabel:
         //   // nop in command tape is ignored (used in replacement patterns)
         //   break;
         case ByteCode::Label:
            fixJumps(writer->Memory(), writer->position(), fwdJumps, command.arg1);
            labels.add(command.arg1, writer->position());

            // JIT compiler interprets nop command as a label mark
            ByteCodeUtil::write(*writer, { ByteCode::Nop });

            break;
         case ByteCode::Jump:
         case ByteCode::Jeq:
         case ByteCode::Jne:
         case ByteCode::Jlt:
         case ByteCode::Jle:
         case ByteCode::Jge:
         case ByteCode::Jgr:
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
         case ByteCode::XLabelDPR:
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

// --- ByteCodeTransformer ---

inline bool isOperational(ByteCode code)
{
   return test((int)code, (int)ByteCode::Label)
      || (code <= ByteCode::CallExtR && code > ByteCode::Breakpoint);
}

bool ByteCodePattern :: checkLabel(ByteCodeIterator it, int label, int offset)
{
   int current = 0;
   while (!it.eof()) {
      auto bc = *it;
      if (bc.code == ByteCode::Label && bc.arg1 == label) {
         return (current == offset);
      }
      if (isOperational(bc.code)) {
         current++;
      }

      ++it;
   }

   return false;
}

void ByteCodeTransformer :: transform(ByteCodeIterator trans_it, ByteCodeTrieNode replacement, PatternArg& arg)
{
   //ByteCodeIterator target_it = trans_it;

   ByteCodePattern pattern = replacement.Value();
   while (pattern.code != ByteCode::None) {
      // skip meta commands (except label)
      while (!isOperational((*trans_it).code))
         --trans_it;

      switch (pattern.argType) {
         case ByteCodePatternType::Set:
            if (pattern.argValue == 3) {
               (*trans_it).arg1 = arg.arg1;
               trans_it.flush();
               (*trans_it).arg2 = arg.arg2;
            }
            else if (pattern.argValue == 1) {
               (*trans_it).arg1 = arg.arg1;
            }
            else (*trans_it).arg1 = arg.arg2;
            trans_it.flush();
            break;
         case ByteCodePatternType::MatchArg:
            (*trans_it).arg1 = pattern.argValue;
            trans_it.flush();
            break;
      //   case braAdd:
      //      (*trans_it).argument += pattern.argument;
      //      break;
      //   case braValue:
      //      (*trans_it).argument = pattern.argument;
      //      break;
      //   case braAditionalValue:
      //      (*trans_it).additional = pattern.argument;
      //      break;
      //   case braCopy:
      //      if (pattern.argument == 1) {
      //         (*target_it).argument = (*trans_it).argument;
      //      }
      //      else if (pattern.argument == 2) {
      //         (*target_it).additional = (*trans_it).argument;
      //      }
      //      break;
         default:
            break;
      }

      (*trans_it).code = pattern.code;
      trans_it.flush();

      --trans_it;
      replacement = replacement.FirstChild();
      pattern = replacement.Value();
   }

}

inline void skipImport(ByteCodeIterator& bc_it)
{
   while ((*bc_it).code != ByteCode::ImportOff)
      ++bc_it;
}

inline bool contains(const ByteCode* list, size_t len, ByteCode bc)
{
   for (size_t i = 0; i < len; i++) {
      if (list[i] == bc)
         return true;
   }

   return false;
}

inline bool isAccFree(ByteCodeIterator bc_it)
{
   while (bc_it.eof()) {
      ByteCode bc = (*bc_it).code;
      if (contains(opSetAcc, sizeof(opSetAcc) / sizeof(ByteCode), bc))
         return true;

      if (!contains(opNotUsingAcc, sizeof(opNotUsingAcc) / sizeof(ByteCode), bc))
         return false;

      ++bc_it;
   }

   return true;
}

inline bool endOfPattern(ByteCodePattern& currentPattern, ByteCodeIterator& bc_it)
{
   if (currentPattern.argType == ByteCodePatternType::IfAccFree) {
      return isAccFree(bc_it);
   }
   return currentPattern.code == ByteCode::Match;
}

bool ByteCodeTransformer :: apply(CommandTape& commandTape)
{
   ByteCodePatterns  matchedOnes;
   ByteCodePatterns  nextOnes;

   ByteCodePatterns* matched = &matchedOnes;
   ByteCodePatterns* followers = &nextOnes;
   bool              reversed = false;

   ByteCodeIterator bc_it = commandTape.start();
   while (!bc_it.eof()) {
      auto bc = *bc_it;

      // HOTFIX : skip an optimization for import block due to issues with branhing
      if (bc.code == ByteCode::ImportOn)
         skipImport(bc_it);

      if (isOperational(bc.code)) {
         matched->add({ &trie });
         followers->clear();

         for (auto it = matched->start(); !it.eof(); ++it) {
            PatternArg arg = (*it).arg;
            auto patternNode = (*it).node;
            //auto pattern = patternNode.Value();

            for (auto child_it = patternNode.Children(); !child_it.eof(); ++child_it) {
               auto currentPatternNode = child_it.Node();
               auto currentPattern = currentPatternNode.Value();

               if (currentPattern.match(bc_it, arg)) {
                  if (endOfPattern(currentPattern, bc_it)) {
                     transform(--bc_it, currentPatternNode.FirstChild(), arg);

                     return true;
                  }
                  followers->add({ currentPatternNode, arg });
               }
            }
         }

         if (reversed) {
            reversed = false;
            followers = &nextOnes;
            matched = &matchedOnes;
         }
         else {
            reversed = true;
            matched = &nextOnes;
            followers = &matchedOnes;
         }
      }

      ++bc_it;
   }

   return false;
}

// --- ImportHelper ---

ref_t ImportHelper :: importReference(ModuleBase* exporter, ref_t exportRef, ModuleBase* importer)
{
   if (!exportRef)
      return 0;

   ustr_t referenceName = exporter->resolveReference(exportRef);
   if (isWeakReference(referenceName) && !isTemplateWeakReference(referenceName)) {
      IdentifierString fullName(exporter->name(), referenceName);

      return importer->mapReference(*fullName);
   }
   else return importer->mapReference(referenceName);
}

ref_t ImportHelper :: importMessage(ModuleBase* exporter, mssg_t exportRef, ModuleBase* importer)
{
   if (!exportRef)
      return 0;

   pos_t paramCount = 0;
   ref_t actionRef, flags;
   decodeMessage(exportRef, actionRef, paramCount, flags);

   if (actionRef) {
      // signature and custom verb should be imported
      ref_t signature = 0;
      ustr_t actionName = exporter->resolveAction(actionRef, signature);

      actionRef = importer->mapAction(actionName, importSignature(exporter, signature, importer), false);
   }

   return encodeMessage(actionRef, paramCount, flags);
}

ref_t ImportHelper :: importSignature(ModuleBase* exporter, ref_t signRef, ModuleBase* importer)
{
   if (!signRef)
      return 0;

   ref_t dump[ARG_COUNT];
   size_t len = exporter->resolveSignature(signRef, dump);
   for (size_t i = 0; i < len; i++) {
      dump[i] = importReference(exporter, dump[i], importer);
   }

   return importer->mapSignature(dump, len, false);
}

ref_t ImportHelper :: importReferenceWithMask(ModuleBase* exporter, ref_t exportRef, ModuleBase* importer)
{
   ref_t mask = exportRef & mskAnyRef;
   ref_t refId = exportRef & ~mskAnyRef;
   if (refId)
      refId = importReference(exporter, refId, importer);

   return refId | mask;
}

ref_t ImportHelper :: importAction(ModuleBase* exporter, ref_t exportRef, ModuleBase* importer)
{
   if (!exportRef)
      return 0;

   ref_t signRef = 0;
   ustr_t value = exporter->resolveAction(exportRef, signRef);

   return  importer->mapAction(value, 0, signRef ? importSignature(exporter, signRef, importer) : 0);
}

ref_t ImportHelper :: importExternal(ModuleBase* exporter, ref_t reference, ModuleBase* importer)
{
   ustr_t refName = exporter->resolveReference(reference);

   return importer->mapReference(refName);
}

ref_t ImportHelper :: importConstant(ModuleBase* exporter, ref_t reference, ModuleBase* importer)
{
   if (!reference)
      return 0;

   ustr_t value = exporter->resolveConstant(reference);

   return  importer->mapConstant(value);
}

ref_t ImportHelper :: importMessageConstant(ModuleBase* exporter, ref_t reference, ModuleBase* importer)
{
   if (!reference)
      return 0;

   ustr_t value = exporter->resolveConstant(reference);

   ByteCodeUtil::resolveMessage(value, importer, false);

   return importer->mapConstant(value);
}

ref_t ImportHelper :: importExtMessageConstant(ModuleBase* exporter, ref_t reference, ModuleBase* importer)
{
   if (!reference)
      return 0;

   ustr_t value = exporter->resolveConstant(reference);

   size_t index = value.find('<');
   assert(index != NOTFOUND_POS);
   size_t endIndex = value.findSub(index, '>');

   IdentifierString messageName(value);
   messageName.cut(index, endIndex - index + 1);

   ByteCodeUtil::resolveMessage(*messageName, importer, false);

   return importer->mapConstant(value);
}
