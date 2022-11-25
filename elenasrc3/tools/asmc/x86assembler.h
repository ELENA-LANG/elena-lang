//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains x86 and x86-64 Assembler declarations
//
//                                                  (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef X86ASSEMBLER_H
#define X86ASSEMBLER_H

#include "assembler.h"
#include "x86helper.h"

namespace elena_lang
{
   // --- X86Assembler ---
   class X86Assembler : public AssemblerBase
   {
   protected:
      virtual X86OperandType getDefaultPrefix()
      {
         return X86OperandType::M32;
      }

      void setOffsetSize(X86Operand& operand, X86OperandType prefix)
      {
         if (operand.reference) {
            operand.type = prefix;
         }
         else if (abs(operand.offset) <= 0x80) {
            operand.type = X86OperandType::DB;
         }
         else operand.type = X86OperandType::DD;
      }

      bool setOffsetValue(X86Operand& operand, X86Operand disp);
      bool setOffset(X86Operand& operand, X86Operand disp);

      virtual X86Operand defineRegister(ustr_t token);

      X86Operand defineOperand(ScriptToken& tokenInfo, X86OperandType prefix, ustr_t errorMessage);
      X86Operand defineDisplacement(ScriptToken& tokenInfo, ustr_t errorMessage);

      virtual X86Operand defineRDisp(X86Operand operand);
      virtual X86Operand defineDBDisp(X86Operand operand);
      virtual X86Operand defineDDDisp(X86Operand operand);

      virtual X86Operand readDispOperand(ScriptToken& tokenInfo, X86Operand operand, X86OperandType prefix, ustr_t errorMessage);

      X86Operand readDispOperand(ScriptToken& tokenInfo, X86OperandType prefix, ustr_t errorMessage);
      X86Operand readPtrOperand(ScriptToken& tokenInfo, X86OperandType prefix, ustr_t errorMessage);

      int readStReg(ScriptToken& tokenInfo);

      virtual X86Operand compileCodeOperand(ScriptToken& tokenInfo, ustr_t errorMessage);
      virtual X86Operand compileDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage, ref_t dataMask);
      virtual X86Operand compileMDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage);

      X86Operand compileOperand(ScriptToken& tokenInfo, ustr_t errorMessage);

      void declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;

      virtual bool compileAdc(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileAdd(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileAnd(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileCall(X86Operand source, MemoryWriter& writer);
      virtual bool compileCmp(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileCMovcc(X86Operand source, X86Operand target, MemoryWriter& writer, X86JumpType type);
      virtual bool compileDiv(X86Operand source, MemoryWriter& writer);
      virtual bool compileFadd(X86Operand source, MemoryWriter& writer);
      virtual bool compileFcomip(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileFdiv(X86Operand source, MemoryWriter& writer);
      virtual bool compileFild(X86Operand source, MemoryWriter& writer);
      virtual bool compileFistp(X86Operand source, MemoryWriter& writer);
      virtual bool compileFisub(X86Operand source, MemoryWriter& writer);
      virtual bool compileFld(X86Operand source, MemoryWriter& writer);
      virtual bool compileFmul(X86Operand source, MemoryWriter& writer);
      virtual bool compileFstp(X86Operand source, MemoryWriter& writer);
      virtual bool compileFsub(X86Operand source, MemoryWriter& writer);
      virtual bool compileIDiv(X86Operand source, MemoryWriter& writer);
      virtual bool compileIMul(X86Operand source, MemoryWriter& writer);
      virtual bool compileIMul(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileJccForward(X86JumpType type, bool shortJump, MemoryWriter& writer);
      virtual bool compileJccBack(X86JumpType type, bool shortJump, int offset, MemoryWriter& writer);
      virtual bool compileJmpForward(bool shortJump, MemoryWriter& writer);
      virtual bool compileJmpBack(bool shortJump, int offset, MemoryWriter& writer);
      virtual bool compileJmp(X86Operand source, MemoryWriter& writer);
      virtual bool compileLea(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileMov(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileMul(X86Operand source, MemoryWriter& writer);
      virtual bool compileNeg(X86Operand source, MemoryWriter& writer);
      virtual bool compileNot(X86Operand source, MemoryWriter& writer);
      virtual bool compileOr(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compilePop(X86Operand source, MemoryWriter& writer);
      virtual bool compilePush(X86Operand source, MemoryWriter& writer);
      virtual bool compileRcr(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileSar(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileSbb(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileSetcc(X86Operand source, X86JumpType type, MemoryWriter& writer);
      virtual bool compileShr(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileShl(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileShld(X86Operand source, X86Operand target, X86Operand third, MemoryWriter& writer);
      virtual bool compileShrd(X86Operand source, X86Operand target, X86Operand third, MemoryWriter& writer);
      virtual bool compileSub(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileTest(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual bool compileXor(X86Operand source, X86Operand target, MemoryWriter& writer);
      virtual void compileExternCall(ScriptToken& tokenInfo, MemoryWriter& writer);

      void compileAdc(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileAdd(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileAnd(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCall(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCdq(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCmp(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCMovcc(ScriptToken& tokenInfo, MemoryWriter& writer, X86JumpType type);
      void compileCWDE(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileDiv(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFadd(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFinit(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFcomip(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFdiv(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFild(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFistp(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFisub(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFld(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFmul(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFstp(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFsub(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileIDiv(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileIMul(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileJcc(ScriptToken& tokenInfo, MemoryWriter& writer, X86JumpType type, LabelScope& labelScope);
      void compileJmp(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileLea(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMov(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMovsb(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMul(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileNeg(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileNop(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileNot(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileOr(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compilePop(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compilePush(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileRcr(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileRep(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileRet(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSar(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSbb(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSetcc(ScriptToken& tokenInfo, MemoryWriter& writer, X86JumpType type);
      void compileShr(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileShl(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileShld(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileShrd(ScriptToken& tokenInfo, MemoryWriter& writer);
      virtual void compileStos(ScriptToken& tokenInfo, MemoryWriter& writer)
      {
         compileStosd(tokenInfo, writer);
      }
      void compileStosd(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSub(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileTest(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileXor(ScriptToken& tokenInfo, MemoryWriter& writer);

      void compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer) override;

      bool compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;
      bool compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileFOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileIOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileJOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;
      bool compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileNOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileOOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compilePOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileROpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileTOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;

      void compileProcedure(ScriptToken& tokenInfo) override;

   public:
      X86Assembler(int tabSize, UStrReader* reader, ModuleBase* target)
         : AssemblerBase(tabSize, reader, target)
      {         
      }
   };

   // --- X86_64Assembler ---
   class X86_64Assembler : public X86Assembler
   {
   protected:
      X86OperandType getDefaultPrefix() override
      {
         return X86OperandType::M64;
      }

      X86Operand defineRegister(ustr_t token) override;

      X86Operand defineRDisp(X86Operand operand) override;
      X86Operand defineDBDisp(X86Operand operand) override;
      X86Operand defineDDDisp(X86Operand operand) override;

      X86Operand readDispOperand(ScriptToken& tokenInfo, X86Operand operand, X86OperandType prefix, ustr_t errorMessage) override;

      X86Operand compileCodeOperand(ScriptToken& tokenInfo, ustr_t errorMessage) override;
      X86Operand compileDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage, ref_t dataMask) override;
      X86Operand compileMDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage) override;

      void compileExternCall(ScriptToken& tokenInfo, MemoryWriter& writer) override;

      bool compileAdd(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileAnd(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileCall(X86Operand source, MemoryWriter& writer) override;
      bool compileCMovcc(X86Operand source, X86Operand target, MemoryWriter& writer, X86JumpType type) override;
      bool compileCmp(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileIDiv(X86Operand source, MemoryWriter& writer) override;
      bool compileIMul(X86Operand source, MemoryWriter& writer) override;
      bool compileJmp(X86Operand source, MemoryWriter& writer) override;
      bool compileLea(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileMov(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileNeg(X86Operand source, MemoryWriter& writer) override;
      bool compileNot(X86Operand source, MemoryWriter& writer) override;
      bool compileOr(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compilePop(X86Operand source, MemoryWriter& writer) override;
      bool compilePush(X86Operand source, MemoryWriter& writer) override;
      bool compileShr(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileShl(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileSub(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileTest(X86Operand source, X86Operand target, MemoryWriter& writer) override;
      bool compileXor(X86Operand source, X86Operand target, MemoryWriter& writer) override;

      void compileStos(ScriptToken& tokenInfo, MemoryWriter& writer) override
      {
         compileStosq(tokenInfo, writer);
      }
      void compileStosq(ScriptToken& tokenInfo, MemoryWriter& writer);

   public:
      X86_64Assembler(int tabSize, UStrReader* reader, ModuleBase* target)
         : X86Assembler(tabSize, reader, target)
      {         
      }
   };
}

#endif