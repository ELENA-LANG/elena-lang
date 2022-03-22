//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                                (C)2021-2022, by Aleksey Rakov
//------------------------------------------------------------------------------

#ifndef BYTECODE_H
#define BYTECODE_H

#include "elena.h"

namespace elena_lang
{

   // --- ByteCode ---
   enum class ByteCode : unsigned int
   {
      // commands:
      Nop            = 0x00,
      Redirect       = 0x03,
      Quit           = 0x04,
      MovEnv         = 0x05,
      Load           = 0x06,

      MaxSingleOp    = 0x7F,

      SetR           = 0x80,
      SetDDisp       = 0x81,
      MovM           = 0x88,

      CloseN         = 0x91,
      AllocI         = 0x92,
      FreeI          = 0x93,

      SaveDDisp      = 0xA0,
      StoreFI        = 0xA1,
      SaveSI         = 0xA2,
      StoreSI        = 0xA3,

      PeekFI         = 0xA8,

      CallR          = 0xB0,
      CallVI         = 0xB1,

      MaxDoubleOp    = 0xEF,

      OpenIN         = 0xF0,
      XStoreSIR      = 0xF1,
      OpenHeaderIN   = 0xF2,
      MovSIFI        = 0xF3,
      NewIR          = 0xF4,
      CallExtR       = 0xFE,

      None           = 0x1000,
   };

   // --- ByteCommand ---
   struct ByteCommand
   {
      ByteCode code;
      arg_t    arg1;
      arg_t    arg2;

      ByteCommand()
      {
         code = ByteCode::Nop;
         arg1 = 0;
         arg2 = 0;
      }
      ByteCommand(ByteCode code)
      {
         this->code = code;
         this->arg1 = 0;
         this->arg2 = 0;
      }
      ByteCommand(ByteCode code, arg_t arg1)
      {
         this->code = code;
         this->arg1 = arg1;
         this->arg2 = 0;
      }
      ByteCommand(ByteCode code, arg_t arg1, arg_t arg2)
      {
         this->code = code;
         this->arg1 = arg1;
         this->arg2 = arg2;
      }
   };

   struct CommandTape
   {
      MemoryList<ByteCommand> tape;

      void write(ByteCode code);
      void write(ByteCode code, arg_t arg1);
      void write(ByteCode code, arg_t arg1, arg_t arg2);

      void import(ModuleBase* sourceModule, MemoryBase* source, bool withHeader, SectionScopeBase* target);

      void saveTo(MemoryWriter* writer);

      CommandTape()
         : tape({})
      {
      }
   };

   // --- ByteCodeUtil ---
   class ByteCodeUtil
   {
   public:
      static ByteCode code(ustr_t command);
      static void decode(ByteCode code, IdentifierString& target);

      static bool isDoubleOp(ByteCode code)
      {
         return code > ByteCode::MaxSingleOp && code <= ByteCode::MaxDoubleOp;
      }

      static bool isSingleOp(ByteCode code)
      {
         return code <= ByteCode::MaxSingleOp;
      }

      static bool isMCommand(ByteCode code)
      {
         switch (code) {
            case ByteCode::MovM:
               return true;
            default:
               return false;
         }
      }

      static bool isRCommand(ByteCode code)
      {
         switch (code) {
         case ByteCode::SetR:
         case ByteCode::CallR:
         case ByteCode::CallExtR:
            return true;
         default:
            return false;
         }
      }

      static bool isR2Command(ByteCode code)
      {
         switch (code) {
            case ByteCode::XStoreSIR:
            case ByteCode::CallR:
               return true;
            default:
               return false;
         }
      }

      static void write(MemoryWriter& writer, ByteCommand& command)
      {
         writer.writeByte((char)command.code);
         if (command.code > ByteCode::MaxSingleOp) {
            writer.write(&command.arg1, sizeof(arg_t));
            if (command.code > ByteCode::MaxDoubleOp)
               writer.write(&command.arg2, sizeof(arg_t));
         }
      }

      static void write(MemoryWriter& writer, ByteCode code)
      {
         ByteCommand command(code);
         write(writer, command);
      }
      static void write(MemoryWriter& writer, ByteCode code, ref_t reference)
      {
         ByteCommand command(code, reference);
         write(writer, command);
      }
      static void write(MemoryWriter& writer, ByteCode code, int index, ref_t reference)
      {
         ByteCommand command(code, index, reference);
         write(writer, command);
      }

      static void read(MemoryReader& reader, ByteCommand& command)
      {
         command.code = (ByteCode)reader.getUChar();
         if (command.code > ByteCode::MaxSingleOp) {
            reader.read(&command.arg1, sizeof(arg_t));
            if (command.code > ByteCode::MaxDoubleOp)
               reader.read(&command.arg2, sizeof(arg_t));
         }            
      }

      static void importCommand(ByteCommand& command, SectionScopeBase* target, ModuleBase* importer);

      static bool resolveMessageName(IdentifierString& messageName, ModuleBase* module, mssg_t message);

      static mssg_t resolveMessage(ustr_t messageName, ModuleBase* module);
   };

}

#endif
