//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains Byte-code Assembler declarations
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BCASSEMBLER_H
#define BCASSEMBLER_H

#include "elena.h"
#include "scriptreader.h"
#include "module.h"
#include "bytecode.h"
#include "lbhelper.h"

namespace elena_lang
{
   // --- ByteCodeAssembler ---
   class ByteCodeAssembler
   {
   protected:
      struct ByteCodeLabelHelper : private LabelHelper
      {
         ReferenceMap      labelNames;
         Map<pos_t, bool>  declaredLabels;

         bool fixLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase* rh) override;

         void writeJumpBack(pos_t label, MemoryWriter& writer) override {}
         void writeJumpForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override {}

         void writeJeqBack(pos_t label, MemoryWriter& writer) override {}
         void writeJeqForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override {}

         void writeJneBack(pos_t label, MemoryWriter& writer) override {}
         void writeJneForward(pos_t label, MemoryWriter& writer, int byteCodeOffset)  override {}

         void writeJltBack(pos_t label, MemoryWriter& writer) override {}
         void writeJltForward(pos_t label, MemoryWriter& writer, int byteCodeOffset)  override {}

         void writeJgeBack(pos_t label, MemoryWriter& writer) override {}
         void writeJgeForward(pos_t label, MemoryWriter& writer, int byteCodeOffset)  override {}

         void writeJleBack(pos_t label, MemoryWriter& writer) override {}
         void writeJleForward(pos_t label, MemoryWriter& writer, int byteCodeOffset)  override {}

         void writeJgrBack(pos_t label, MemoryWriter& writer) override {}
         void writeJgrForward(pos_t label, MemoryWriter& writer, int byteCodeOffset)  override {}

         ref_t getLabel(ustr_t labelName)
         {
            return labelNames.get(labelName);
         }

         bool checkDeclaredLabel(ustr_t label)
         {
            return declaredLabels.exist(labelNames.get(label));
         }

         bool declareLabel(ustr_t labelName, MemoryWriter& writer)
         {
            ref_t label = getLabel(labelName);
            if (!label) {
               label = labelNames.count() + 1;

               labelNames.add(labelName, label);
            }

            declaredLabels.add(labelNames.get(labelName), true);

            return setLabel(label, writer, nullptr);
         }

         void registerJump(ustr_t labelName, MemoryWriter& writer);

         int resolveLabel(ustr_t label, MemoryWriter& writer);


         void checkAllUsedLabels(ustr_t errorMessage, ustr_t procedureName);

         ByteCodeLabelHelper()
            : labelNames(0),
              declaredLabels(false)
         {
         }
      };

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
         bool  byVal64;

         static Operand Default()
         {
            Operand op;

            return op;
         }

         Operand()
         {
            type = Type::None;
            reference = 0;
            byVal = byVal64 = false;
         }
      };

      ScriptReader _reader;
      Module*      _module;
      bool         _mode64;
      int          _rawDataAlignment;

      void read(ScriptToken& tokenInfo);
      void read(ScriptToken& tokenInfo, ustr_t expectedToken, ustr_t error)
      {
         read(tokenInfo);
         if (!tokenInfo.compare(expectedToken))
            throw SyntaxError(error, tokenInfo.lineInfo);
      }

      ref_t readReference(ScriptToken& tokenInfo, bool skipRead = false);
      mssg_t readM(ScriptToken& tokenInfo, bool skipRead = false);
      int readN(ScriptToken& tokenInfo, ReferenceMap& constants, bool skipRead = false);
      int readI(ScriptToken& tokenInfo, bool skipRead = false);
      int readFrameI(ScriptToken& tokenInfo, ReferenceMap& parameters, ReferenceMap& locals,
         bool skipRead = false);
      int readDisp(ScriptToken& tokenInfo, ReferenceMap& dataLocals, bool skipRead = false);

      void readParameterList(ScriptToken& tokenInfo, ReferenceMap& parameters);

      int readArgList(ScriptToken& tokenInfo, ReferenceMap& locals, ReferenceMap& constants,
         int factor, bool allowSize);

      bool writeArg(MemoryWriter& writer, Operand& arg, int index);

      bool declareLabel(ustr_t label, ScriptToken& tokenInfo, MemoryWriter& writer, ByteCodeLabelHelper& labelScope);

      Operand compileArg(ScriptToken& tokenInfo, ReferenceMap& parameters, ReferenceMap& locals,
         ReferenceMap& dataLocals, ReferenceMap& constants);
      void compileArgList(ScriptToken& tokenInfo, List<Operand>& operands, 
         ReferenceMap& parameters, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants);

      bool compileDDisp(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
        ReferenceMap& dataLocals, bool skipRead);
      bool compileDDispR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& dataLocals, bool skipRead);
      bool compileDDispN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& dataLocals, ReferenceMap& constants, bool skipRead);
      bool compileOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& constants, bool skipRead);
      bool compileOpM(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         bool skipRead);
      bool compileOpI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         bool skipRead);
      bool compileOpFrameI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& parameters, ReferenceMap& locals, bool skipRead);
      bool compileOpStackI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         bool skipRead);
      bool compileOpIN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         ReferenceMap& constants, bool skipRead);
      bool compileOpII(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileMR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileNR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         ReferenceMap& constants, bool skipRead);
      bool compileR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileRR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead);
      bool compileCloseOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         int dataSize, ReferenceMap& constants);
      bool compileOpenOp(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
         ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants, int& dataSize);
      bool compileCallExt(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         ReferenceMap& parameters, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants);

      bool compileJcc(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
         ByteCodeLabelHelper& lh);

      bool compileByteCode(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCodeLabelHelper& lh,
         ReferenceMap& parameters, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants, int& dataSize);

      void compileConstant(ScriptToken& tokenInfo, ReferenceMap& constants);
      void compileProcedure(ScriptToken& tokenInfo, ref_t mask, ReferenceMap& constants);

   public:
      void compile();

      ByteCodeAssembler(int tabSize, UStrReader* reader, Module* module, 
         bool mode64, int rawDataAlignment);
   };

}

#endif
