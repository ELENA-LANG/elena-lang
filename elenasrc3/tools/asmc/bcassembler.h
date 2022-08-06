//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains Byte-code Assembler declarations
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BCASSEMBLER_H
#define BCASSEMBLER_H

#include "elena.h"
#include "scriptreader.h"
#include "module.h"
#include "bytecode.h"

namespace elena_lang
{
   // --- ByteCodeAssembler ---
   class ByteCodeAssembler
   {
   protected:
      struct Operand
      {
         enum class Type
         {
            None = 0,
            R,
            Variable,
            DataVariable,
            Value
         };

         Type  type;
         ref_t reference;
         bool  byVal;

         static Operand Default()
         {
            Operand op;

            return op;
         }

         Operand()
         {
            type = Type::None;
            reference = 0;
            byVal = false;
         }
      };

      ScriptReader _reader;
      Module*      _module;
      bool         _mode64;

      void read(ScriptToken& tokenInfo);
      void read(ScriptToken& tokenInfo, ustr_t expectedToken, ustr_t error)
      {
         read(tokenInfo);
         if (!tokenInfo.compare(expectedToken))
            throw SyntaxError(error, tokenInfo.lineInfo);
      }

      ref_t readReference(ScriptToken& tokenInfo, bool skipRead = false);
      mssg_t readM(ScriptToken& tokenInfo, bool skipRead = false);
      int readN(ScriptToken& tokenInfo, bool skipRead = false);
      int readI(ScriptToken& tokenInfo, bool skipRead = false);
      int readFrameI(ScriptToken& tokenInfo, ReferenceMap& locals, bool skipRead = false);
      int readDisp(ScriptToken& tokenInfo, ReferenceMap& dataLocals, bool skipRead = false);

      void readArgList(ScriptToken& tokenInfo, ReferenceMap& locals, ReferenceMap& constants, 
         int factor, bool allowSize);

      bool writeArg(MemoryWriter& writer, Operand& arg, int index);

      Operand compileArg(ScriptToken& tokenInfo, ReferenceMap& locals, 
         ReferenceMap& dataLocals, ReferenceMap& constants);
      void compileArgList(ScriptToken& tokenInfo, List<Operand>& operands, 
         ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants);

      bool compileDDisp(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
        ReferenceMap& dataLocals, bool skipRead);
      bool compileOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileOpFrameI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         ReferenceMap& locals, bool skipRead);
      bool compileOpStackI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& locals, bool skipRead);
      bool compileOpIN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileOpII(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileMR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileRR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileCloseOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, ReferenceMap& dataLocals);
      bool compileOpenOp(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants);
      bool compileCallExt(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants);

      bool compileByteCode(ScriptToken& tokenInfo, MemoryWriter& writer, 
         ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants);

      void compileConstant(ScriptToken& tokenInfo, ReferenceMap& constants);
      void compileProcedure(ScriptToken& tokenInfo, ref_t mask, ReferenceMap& constants);

   public:
      void compile();

      ByteCodeAssembler(int tabSize, UStrReader* reader, Module* module, bool mode64);
   };

}

#endif
