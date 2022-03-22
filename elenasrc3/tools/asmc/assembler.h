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
      ReferenceMap      labels;
      Map<ref_t, pos_t> declaredLabels;
      Map<ref_t, pos_t> jumps;

      ref_t getLabel(ustr_t labelName)
      {
         return labels.get(labelName);
      }

      LabelScope()
         : labels(0), declaredLabels(0), jumps(0)
      {
      }
   };

   // --- BaseJumpHelper
   class BaseJumpHelper
   {
   public:
      virtual bool fixLabel(ref_t label, LabelScope& labelScope, MemoryWriter& writer) = 0;

      bool checkDeclaredLabel(ustr_t label, LabelScope& labelScope)
      {
         return labelScope.declaredLabels.exist(labelScope.labels.get(label));
      }

      void registerLabel(ustr_t labelName, LabelScope& labelScope, MemoryWriter& writer)
      {
         ref_t label = labelScope.labels.get(labelName);
         if (!label) {
            label = labelScope.labels.count() + 1;

            labelScope.labels.add(labelName, label);
         }

         labelScope.jumps.add(label, writer.position());
      }

      bool addLabel(ustr_t labelName, LabelScope& labelScope, MemoryWriter& writer)
      {
         pos_t labelPosition = writer.position();

         ref_t label = labelScope.labels.get(labelName);
         if (!label) {
            label = labelScope.labels.count() + 1;

            labelScope.labels.add(labelName, label);
         }

         if (!fixLabel(label, labelScope, writer))
            return false;

         labelScope.declaredLabels.add(label, labelPosition);

         return true;
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

      void compileProcedure(ScriptToken& tokenInfo);
      void compileStructure(ScriptToken& tokenInfo);
      void declareConstant(ScriptToken& tokenInfo);

   public:
      void compile();

      AssemblerBase(int tabSize, UStrReader* reader, ModuleBase* target);
   };

}

#endif