//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains Syntax Tree Builder class declaration
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DERIVATION_H
#define DERIVATION_H

#include "elena.h"
#include "syntaxtree.h"
#include "clicommon.h"
#include "compilerlogic.h"

namespace elena_lang
{
   // --- SyntaxTreeWriter ---
   class SyntaxTreeBuilder : public SyntaxWriterBase
   {
      enum class ScopeType
      {
         Unknown = 0,
         InlineTemplate,
         ClassTemplate
      };

      struct Scope
      {
         ScopeType    type;
         bool         ignoreTerminalInfo;
         ReferenceMap parameters;
         int          nestedLevel;

         bool withTypeParameters() const
         {
            return type == ScopeType::ClassTemplate;
         }

         bool isParameter(SyntaxNode node, ref_t& parameterKey)
         {
            switch (type) {
               case ScopeType::InlineTemplate:
               {
                  ref_t key = parameters.get(node.identifier());
                  if (key) {
                     parameterKey = key;
                     return true;
                  }
                  return false;
               }
               default:
                  return false;
            }
         }

         Scope()
            : parameters(0)
         {
            type = ScopeType::Unknown;
            ignoreTerminalInfo = false;
            nestedLevel = 0;
         }
      };

      ErrorProcessor*   _errorProcessor;
      SyntaxTree        _cache;
      int               _level;

      SyntaxTreeWriter  _writer;
      SyntaxTreeWriter  _cacheWriter;

      ModuleScopeBase* _moduleScope;

      ref_t mapAttribute(SyntaxNode node, bool allowType, ref_t& previusCategory);

      void generateTemplateStatement();

      void flushNode(Scope& scope, SyntaxNode node);
      void flushCollection(Scope& scope, SyntaxNode node);

      void flushIdentifier(SyntaxNode identNode, bool ignoreTerminalInfo);
      void flushTemplateCode(Scope& scope, SyntaxNode node);
      void flushTemplateArgDescr(Scope& scope, SyntaxNode node);
      void flushTemplateArg(Scope& scope, SyntaxNode node, bool allowType);
      void flushTemplageExpression(Scope& scope, SyntaxNode node, SyntaxKey type, bool allowType);
      void flushTemplateType(Scope& scope, SyntaxNode node);
      void flushMessage(Scope& scope, SyntaxNode node);
      void flushObject(Scope& scope, SyntaxNode node);
      void flushNested(Scope& scope, SyntaxNode node);
      void flushClosure(Scope& scope, SyntaxNode node);
      void flushExpression(Scope& scope, SyntaxNode node);
      void flushStatement(Scope& scope, SyntaxNode node);
      void flushMethodCode(Scope& scope, SyntaxNode node);

      void copyHeader(Scope& scope, SyntaxNode node);

      void flushSubScopeMember(Scope& scope, SyntaxNode node, SyntaxNode headerNode);
      void flushSubScope(Scope& scope, SyntaxNode node, SyntaxNode headerNode);

      void flushClassMember(Scope& scope, SyntaxNode node);
      void flushMethod(Scope& scope, SyntaxNode node);
      void flushMethodMember(Scope& scope, SyntaxNode node);
      void flushTemplate(Scope& scope, SyntaxNode node);
      void flushAttribute(Scope& scope, SyntaxNode node, ref_t& previusCategory, bool allowType);
      void flushTypeAttribute(Scope& scope, SyntaxNode node, ref_t& previusCategory, bool allowType);
      void flushClassMemberPostfixes(Scope& scope, SyntaxNode node);
      void flushClassPostfixes(Scope& scope, SyntaxNode node);

      void flushDescriptor(Scope& scope, SyntaxNode node, bool withNameNode = true, bool typeDescriptor = false);
      void flushClass(Scope& scope, SyntaxNode node);
      void flushInlineTemplate(Scope& scope, SyntaxNode node);
      void flushDeclaration(SyntaxNode node);
      void flushDictionary(SyntaxNode node);
      void flush(SyntaxNode node);

   public:
      void newNode(parse_key_t key) override;
      void newNode(parse_key_t key, ustr_t arg) override;
      void appendTerminal(parse_key_t key, ustr_t value, LineInfo lineInfo) override;
      void injectNode(parse_key_t key) override;

      void closeNode() override;

      void clear()
      {
         _level = 0;

         _cacheWriter.clear();

         _cacheWriter.newNode(SyntaxKey::Root);
      }

      SyntaxTreeBuilder(SyntaxTree* target, ErrorProcessor* errorProcessor, ModuleScopeBase* moduleScope)
         : _writer(*target), _cacheWriter(_cache)
      {
         _errorProcessor = errorProcessor;
         _moduleScope = moduleScope;

         _writer.clear();
         _writer.newNode(SyntaxKey::Root);

         clear();
      }
   };

   // ---- TemplateProssesor ---
   class TemplateProssesor
   {
      enum class Type
      {
         None = 0,
         Inline,
         Class
      };

      typedef Map<ref_t, SyntaxNode> NodeMap;

      struct TemplateScope
      {
         Type             type;
         NodeMap          parameterValues;
         ModuleScopeBase* moduleScope;
         ref_t            targetRef;

         TemplateScope()
            : type(Type::None), parameterValues({}), moduleScope(nullptr), targetRef(0)
         {
         }
         TemplateScope(Type type, ModuleScopeBase* scope, ref_t targetRef)
            : type(type), parameterValues({}), moduleScope(scope), targetRef(targetRef)
         {
         }
      };

      void loadParameters(TemplateScope& scope, List<SyntaxNode>& parameters);

      void copyNode(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);
      void copyChildren(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);
      void copyField(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);
      void copyMethod(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);

      void generate(SyntaxTreeWriter& writer, TemplateScope& scope, MemoryBase* templateSection);

      void generateTemplate(SyntaxTreeWriter& writer, TemplateScope& scope, MemoryBase* templateBody);

      void importTemplate(Type type, MemoryBase* templateSection, SyntaxNode target,
         List<SyntaxNode>& parameters);

   public:
      void importInlineTemplate(MemoryBase* section, SyntaxNode target, List<SyntaxNode>& parameters);

      void generateClassTemplate(ModuleScopeBase* moduleScope, ref_t classRef, SyntaxTree* syntaxTree, 
         MemoryBase* sectionBody, List<SyntaxNode>& parameters);

      TemplateProssesor() = default;
   };
}

#endif