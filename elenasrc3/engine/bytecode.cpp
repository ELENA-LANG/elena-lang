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
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, "redirect", "quit", "mov env", "load", OPCODE_UNKNOWN,
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

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "set", "set dp", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   "mov mssg", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, "close", "alloc", "free", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "save dp", "store fp", "save sp", "store sp", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   "peek fp", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "call", "call vt", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,

   "open", "xstore sp", "open header", "mov sp", "new", OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN,
   OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, OPCODE_UNKNOWN, "call extern", OPCODE_UNKNOWN
};

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

bool ByteCodeUtil :: resolveMessageName(IdentifierString& messageName, ModuleBase* module, mssg_t message)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   ref_t signature = 0;
   ustr_t actionName = module->resolveAction(actionRef, signature);
   if (emptystr(actionName))
      return false;

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

   if (argCount != 0) {
      messageName.append('[');
      messageName.appendInt(argCount);
      messageName.append(']');
   }

   return true;
}

mssg_t ByteCodeUtil :: resolveMessage(ustr_t messageName, ModuleBase* module)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0;

   IdentifierString actionName;
   size_t paramIndex = messageName.find('[');
   if (paramIndex != NOTFOUND_POS) {
      actionName.copy(messageName, paramIndex);

      String<char, 12> counterStr(messageName + paramIndex + 1, messageName.length() - paramIndex - 2);
      argCount = StrConvertor::toInt(counterStr.str(), 10);
   }
   else actionName.copy(messageName);

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

   actionRef = module->mapAction(*actionName, signature, true);
   if (actionRef == 0) {
      return 0;
   }

   return encodeMessage(actionRef, argCount, flags);
}

void ByteCodeUtil :: importCommand(ByteCommand& command, SectionScopeBase* target, ModuleBase* importer)
{
   if (isRCommand(command.code)) {
      command.arg1 = target->importReference(importer, command.arg1);
   }
   else if (isMCommand(command.code)) {
      command.arg1 = target->importMessage(importer, command.arg1);
   }

   if (isR2Command(command.code)) {
      command.arg2 = target->importReference(importer, command.arg2);
   }
}

// --- CommandTape ---

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
   for (auto it = tape.start(); !it.eof(); ++it) {
      auto command = *it;
      if ((unsigned int)command.code < 0x100) {
         ByteCodeUtil::write(*writer, command);
      }
   }
}

