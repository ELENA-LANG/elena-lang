//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Derivation Tree classes
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef derivationH
#define derivationH 1

#include "parser.h"
#include "compilercommon.h"

namespace _ELENA_
{

// --- DerivationWriter ---

//typedef Map<ident_t, char*> TypedMap;

class DerivationWriter : public _DerivationWriter
{
   enum class MetaScope : int
   {
      None,
      Namespace,
      Type,
      Import,
   };

   enum ScopeType
   {
      stNormal = 0,
      stClassTemplate,
      stCodeTemplate,
      stPropertyTemplate,
      stExtensionTemplate,
      stInlineTemplate
   };

   struct Scope
   {
      ScopeType      templateMode;
      ForwardMap     parameters;
      int            nestedLevel;
      bool           ignoreTerminalInfo;
      int            bookmark;

      bool isNameParameter(ident_t name, ref_t& argument)
      {
         if (/*templateMode == stInlineTemplate || */templateMode == stPropertyTemplate) {
            ref_t index = parameters.get(name);
            if (index == parameters.Count()) {
               argument = nestedLevel + index;

               return true;
            }
            else return false;
         }
         else return false;
      }

      bool isIdentifierParameter(ident_t name, ref_t& argument)
      {
         if (withTypeParameters()) {
            int index = parameters.get(name);
            if (index) {
               argument = index + nestedLevel;

               return true;
            }
         }
         return false;
      }

      bool withTypeParameters() const
      {
         return templateMode == stClassTemplate || templateMode == stPropertyTemplate
            || templateMode == stExtensionTemplate;
      }

      Scope()
      {
         templateMode = ScopeType::stNormal;
         nestedLevel = 0;
         ignoreTerminalInfo = false;
         bookmark = 0;
      }
   };

   int           _level;
   int           _cachingLevel;
   SyntaxTree    _cache;
   SyntaxWriter  _cacheWriter;

   SyntaxWriter  _output;

   _ModuleScope* _scope;

   void loadTemplateParameters(Scope& scope, SNode node);
   void loadTemplateExprParameters(Scope& scope, SNode node);

   MetaScope recognizeMetaScope(SNode node);

   void saveScope(SyntaxWriter& writer);

   ref_t resolveTemplate(ident_t templateName);

   ref_t mapAttribute(SNode terminal, bool allowType, bool& allowPropertyTemplate, ref_t& previusCategory);
   ref_t mapInlineAttribute(SNode terminal);
   void declareAttribute(SNode node);
   void declareStatement(SNode node, ScopeType templateType);

   void recognizeAttributes(SNode node, int mode, LexicalType nameType);

   void recognizeScope();
   void recognizeDefinition(SNode scopeNode);
   void recognizeScopeAttributes(SNode node, int mode);
   void recognizeClassMembers(SNode node);
   void recognizeMethodMembers(SNode node);

// //  bool recognizeMetaScope(SNode node);

   void declareNestedNamespace(SNode node, Scope& derivationScope);

   void saveTemplateParameters(SyntaxWriter& tempWriter, SNode current, Scope& derivationScope, bool rootMode);

   void copyScope(SyntaxWriter& writer, SNode node, Scope& derivationScope);

   void generateOperatorTemplateTree(SyntaxWriter& writer, SNode& current, Scope& derivationScope);
   void generateClassImport(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer);
   void generateTemplateTree(SNode node, ScopeType templateType);
   void flushScope(SyntaxWriter& writer, SNode node, Scope& scope);
//   void generateClosureTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope);
   void generateStatementTemplateTree(SyntaxWriter& writer, SNode node, SyntaxTree& tempTree, ident_t templateName, 
      Scope& derivationScope);
   void flushPropertyBody(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer);
   void flushPropertyTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer);
   void flushInlineTemplateTree(SyntaxWriter& writer, SNode node, SNode owner, Scope& derivationScope, SyntaxTree& buffer);
//   //void generateClassTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
////   //void generateMetaTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void flushSymbolTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void flushClassTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void flushMethodTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool functionMode, bool propertyMode, 
      SyntaxTree& buffer);
   void flushPropertyTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer);
   void flushFieldTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer);
   void flushCodeTree(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool withBookmark = false*/);
   void flushTokenExpression(SyntaxWriter& writer, SNode& node, Scope& derivationScope/*, bool rootMode*/);
   void flushStatement(SyntaxWriter& writer, SNode& node, Scope& derivationScope/*, bool rootMode*/);
   void flushTypeAttribute(SyntaxWriter& writer, SNode terminal, ref_t typeRef, Scope& derivationScope);
   void flushTemplateTypeAttribute(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void flushArrayTypeAttribute(SyntaxWriter& writer, SNode terminal, Scope& derivationScope);
   void flushAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer);
   void flushTemplateAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void flushExpressionAttribute(SyntaxWriter& writer, SNode node, Scope& derivationScope, ref_t& previousCategory, 
      /*int dimensionCounter, */bool templateArgMode = false);
   void flushExpressionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, int mode = 0);
   void flushExpressionNode(SyntaxWriter& writer, SNode& current,/*bool& first, bool& expressionExpected, */Scope& derivationScope);
//   void generateCollectionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void flushSwitchTree(SyntaxWriter& writer, SNode current, Scope& derivationScope);
   void flushCodeExpression(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool closureMode*/);
   void flushIdentifier(SyntaxWriter& writer, SNode current, Scope& derivationScope);
   void flushMesage(SyntaxWriter& writer, SNode current, Scope& derivationScope);

   void declareType(SNode node);
   void flushImport(SyntaxWriter& writer, SNode ns);

   void raiseError(ident_t err, SNode node);
   void raiseWarning(int level, ident_t msg, SNode node);

   void appendFilePath(SNode node, IdentifierString& path);

public:
   void newNodeDirectly(LexicalType symbol);
   void newNodeDirectly(LexicalType symbol, ident_t arg);
   void closeNodeDirectly();

   virtual void newNode(LexicalType symbol);
   virtual void appendTerminal(TerminalInfo& terminal);
   virtual void closeNode();

   DerivationWriter(SyntaxTree& target, _ModuleScope* scope)
      :  _output(target), _cacheWriter(_cache)
   {
      _cachingLevel = _level = 0;

      _scope = scope;

      _cacheWriter.newNode(lxRoot);
   }
};

// --- TemplateGenerator ---

class TemplateGenerator
{
   typedef Map<int, SNode> NodeMap;

   // --- TemplateScope ---
   struct TemplateScope
   {
      enum Type
      {
         ttClassTemplate    = 0,
         ttPropertyTemplate = 1,
         ttCodeTemplate     = 3,
         ttInlineTemplate   = 4
      };

      Type          type;
      ref_t         templateRef;
      ref_t         reference;
      NodeMap       parameterValues;

      _Module*      templateModule;
      _ModuleScope* moduleScope;

      ident_t       ns;
      ident_t       sourcePath;

      bool generateClassName();

      _Memory* loadTemplateTree();

      TemplateScope(_ModuleScope* moduleScope, ref_t templateRef, ident_t sourcePath, ident_t ns/*, IdentifierList* imports*/)
         : parameterValues(SNode())
      {
         this->sourcePath = sourcePath;
         this->ns = ns;

         this->moduleScope = moduleScope;

         type = Type::ttClassTemplate;
         this->templateRef = templateRef;
         this->reference = 0;
         this->templateModule = nullptr;
      }
   };

   void copyNodes(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyChildren(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyFieldInitTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, int bookmark);
   void copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, int bookmark);
   void copyExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyTreeNode(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyModuleInfo(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyTemplateIdenParam(SyntaxWriter& writer, SNode nodeToInject, TemplateScope& scope);

   bool generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass, 
      bool importModuleInfo, int bookmark);

public:
   ref_t declareTemplate(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters);
   ref_t generateTemplate(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters,
                           bool importModuleInfo, bool importMode);

   void generateTemplateCode(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters);
   void generateTemplateProperty(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, 
      List<SNode>& parameters, int bookmark, bool inlineMode);

   void importClass(SyntaxWriter& output, SNode node);

   TemplateGenerator();
};

} // _ELENA_

#endif // derivationH
