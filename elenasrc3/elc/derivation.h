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
         ReferenceMap arguments;
         ReferenceMap parameters;
         int          nestedLevel;

         bool withTypeParameters() const
         {
            return type == ScopeType::ClassTemplate;
         }

         bool isParameter(SyntaxNode node, SyntaxKey& parameterKey, ref_t& parameterIndex)
         {
            switch (type) {
               case ScopeType::InlineTemplate:
               {
                  ref_t index = arguments.get(node.identifier());
                  if (index) {
                     parameterKey = SyntaxKey::TemplateArgParameter;
                     parameterIndex = index;
                     return true;
                  }
                  index = parameters.get(node.identifier());
                  if (index) {
                     parameterKey = SyntaxKey::TemplateParameter;
                     parameterIndex = index;
                     return true;
                  }
                  return false;
               }
               default:
                  return false;
            }
         }

         Scope()
            : arguments(0), parameters(0)
         {
            type = ScopeType::Unknown;
            ignoreTerminalInfo = false;
            nestedLevel = 0;
         }
      };

      ErrorProcessor*         _errorProcessor;
      SyntaxTree              _cache;
      int                     _level;

      SyntaxTreeWriter        _writer;
      SyntaxTreeWriter        _cacheWriter;

      ModuleScopeBase*        _moduleScope;
      TemplateProssesorBase*  _templateProcessor;

      ref_t mapAttribute(SyntaxNode node, bool allowType, ref_t& previusCategory);

      void parseStatement(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode current, 
         List<SyntaxNode>& arguments, List<SyntaxNode>& parameters, IdentifierString& postfix);
      void generateTemplateStatement(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);

      void flushNode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushCollection(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);

      void flushIdentifier(SyntaxTreeWriter& writer, SyntaxNode identNode, bool ignoreTerminalInfo);
      void flushTemplateCode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushTemplateArgDescr(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushParameterArgDescr(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushTemplateArg(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool allowType);
      void flushTemplageExpression(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, SyntaxKey type, bool allowType);
      void flushTemplateType(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushMessage(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushObject(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushNested(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushClosure(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushExpression(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushStatement(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushMethodCode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);

      void copyHeader(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);

      void flushSubScopeMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, SyntaxNode headerNode);
      void flushSubScope(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, SyntaxNode headerNode);

      void flushClassMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool functionMode = false);
      void flushMethod(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushMethodMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushAttribute(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, ref_t& previusCategory, 
         bool allowType);
      void flushTypeAttribute(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, ref_t& previusCategory, 
         bool allowType);
      void flushInlineTemplatePostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushClassMemberPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushClassPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);

      void flushDescriptor(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool withNameNode = true, 
         bool typeDescriptor = false);
      void flushClass(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool functionMode);
      void flushInlineTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node);
      void flushDeclaration(SyntaxTreeWriter& writer, SyntaxNode node);
      void flushDictionary(SyntaxTreeWriter& writer, SyntaxNode node);
      void flushNamespace(SyntaxTreeWriter& writer, SyntaxNode node);
      void flush(SyntaxTreeWriter& writer, SyntaxNode node);

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

      SyntaxTreeBuilder(SyntaxTree* target, ErrorProcessor* errorProcessor, 
         ModuleScopeBase* moduleScope, TemplateProssesorBase* templateProcessor)
         : _writer(*target), _cacheWriter(_cache)
      {
         _errorProcessor = errorProcessor;
         _moduleScope = moduleScope;
         _templateProcessor = templateProcessor;

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
         CodeTemplate,
         Class
      };

      typedef Map<ref_t, SyntaxNode> NodeMap;

      struct TemplateScope
      {
         Type             type;
         NodeMap          argValues;
         NodeMap          parameterValues;
         ModuleScopeBase* moduleScope;
         ref_t            targetRef;

         TemplateScope() :
            type(Type::None),
            argValues({}),
            parameterValues({}),
            moduleScope(nullptr), targetRef(0)
         {
         }
         TemplateScope(Type type, ModuleScopeBase* scope, ref_t targetRef) :
            type(type),
            argValues({}),
            parameterValues({}),
            moduleScope(scope),
            targetRef(targetRef)
         {
         }
      };

      void loadArguments(TemplateScope& scope, List<SyntaxNode>* parameters);
      void loadParameters(TemplateScope& scope, List<SyntaxNode>* parameters);

      void copyNode(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);
      void copyChildren(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);
      void copyField(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);
      void copyMethod(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node);

      void generate(SyntaxTreeWriter& writer, TemplateScope& scope, MemoryBase* templateSection);

      void generateTemplate(SyntaxTreeWriter& writer, TemplateScope& scope, MemoryBase* templateBody);

      void importTemplate(Type type, MemoryBase* templateSection, SyntaxNode target,
         List<SyntaxNode>* arguments, List<SyntaxNode>* parameters);

   public:
      void importInlineTemplate(MemoryBase* section, SyntaxNode target, List<SyntaxNode>& parameters);
      void importCodeTemplate(MemoryBase* templateSection,
         SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters);

      void generateClassTemplate(ModuleScopeBase* moduleScope, ref_t classRef, SyntaxTree* syntaxTree, 
         MemoryBase* sectionBody, List<SyntaxNode>& parameters);

      TemplateProssesor() = default;
   };
}

#endif