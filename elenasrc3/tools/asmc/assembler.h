//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract Assembler declarations
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "elena.h"
#include "scriptreader.h"
#include "lbhelper.h"

namespace elena_lang
{
   // --- ProcedureInfo ---
   struct ProcedureInfo
   {
      ref_t reference;

      ProcedureInfo()
      {
         reference = 0;
      }
   };

   struct LabelScope
   {
      ReferenceMap      labelNames;
      Map<pos_t, bool>  declaredLabels;
      LabelHelper*      helper;

      ref_t getLabel(ustr_t labelName)
      {
         return labelNames.get(labelName);
      }

      bool checkDeclaredLabel(ustr_t label)
      {
         return declaredLabels.exist(labelNames.get(label));
      }

      void registerJump(ustr_t labelName, MemoryWriter& writer)
      {
         ref_t label = labelNames.get(labelName);
         if (!label) {
            label = labelNames.count() + 1;

            labelNames.add(labelName, label);
         }

         helper->declareJump(label, writer);
      }

      bool declareLabel(ustr_t labelName, MemoryWriter& writer)
      {
         ref_t label = getLabel(labelName);
         if (!label) {
            label = labelNames.count() + 1;

            labelNames.add(labelName, label);
         }

         declaredLabels.add(labelNames.get(labelName), true);

         return helper->setLabel(label, writer);
      }

      int resolveLabel(ustr_t label, MemoryWriter& writer)
      {
         int offset = helper->labels.get(getLabel(label)) - writer.position();

         return offset;
      }

      LabelScope(LabelHelper* lh)
         : labelNames(0), declaredLabels(false), helper(lh)
      {
      }
   };

   // --- AssemblerBase ---
   class AssemblerBase
   {
   protected:
      typedef Map<ustr_t, int, allocUStr, freeUStr> ConstantMap;

      ScriptReader _reader;
      ModuleBase*  _target;

      ConstantMap  constants;

      void checkComma(ScriptToken& tokenInfo);

      void read(ScriptToken& tokenInfo);
      void read(ScriptToken& tokenInfo, ustr_t expectedToken, ustr_t error)
      {
         read(tokenInfo);
         if (!tokenInfo.compare(expectedToken))
            throw SyntaxError(error, tokenInfo.lineInfo);
      }

      ref_t readReference(ScriptToken& tokenInfo);
      int readInteger(ScriptToken& tokenInfo);

      bool getArgReference(ScriptToken& tokenInfo, int& target, ref_t& reference);
      bool getIntConstant(ScriptToken& tokenInfo, int& target, ref_t& reference);

      void declareProcedure(ScriptToken& tokenInfo, ProcedureInfo& procInfo);

      virtual void declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) = 0;

      virtual bool compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) = 0;
      virtual bool compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileIOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileJOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) = 0;
      virtual bool compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileNOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileOOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compilePOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileROpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileTOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual bool compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;

      virtual bool compileOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);

      virtual void compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual void compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual void compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;
      virtual void compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer) = 0;

      virtual void compileProcedure(ScriptToken& tokenInfo) = 0;

      void compileProcedure(ScriptToken& tokenInfo, LabelHelper* helper);
      void compileStructure(ScriptToken& tokenInfo);
      void declareConstant(ScriptToken& tokenInfo);

   public:
      void compile();

      AssemblerBase(int tabSize, UStrReader* reader, ModuleBase* target);
   };

}

#endif