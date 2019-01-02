//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Derivation Tree classes
//
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef derivationH
#define derivationH 1

#include "syntax.h"
#include "compilercommon.h"

namespace _ELENA_
{

// --- DerivationWriter ---

typedef Map<ident_t, char*> TypedMap;


class DerivationWriter : public _DerivationWriter
{
   enum DeclarationAttr
   {
      daNone        = 0x0000,
      daType        = 0x0001,
   //      daClass       = 0x0002,
      daTemplate    = 0x0004,
      daProperty    = 0x0008,
   //      daLoop        = 0x0020,
         daImport      = 0x0040,
   //      daExtern      = 0x0080,
   //      daAccessor    = 0x0100,
   //      daDblAccessor = 0x0300,
   //      daCodeMask    = 0xF800,
   //      daCode        = 0x0800,
   //      daBlock       = 0x1800,
   //      daDblBlock    = 0x3800,
   //      daNestedBlock = 0x4800,
   //      daExtension   = 0x8000
   };

   enum ScopeType
   {
      stNormal = 0,
      stClassTemplate,
      stCodeTemplate,
      stPropertyTemplate
   };

   struct Scope
   {
      ScopeType      templateMode;
      ForwardMap     parameters;

      bool isNameParameter(ident_t name, ref_t& argument)
      {
         if (templateMode == stPropertyTemplate && name.compare(PROPERTY_VAR)) {
            argument = 1;

            return true;
         }
         else return false;
      }

      bool isMessageParameter(ident_t name, ref_t& argument)
      {
         if (templateMode == stPropertyTemplate) {
            int index = parameters.get(name);
            if (index) {
               argument = index;

               return true;
            }
         }
         return false;
      }

      bool isTypeParameter(ident_t name, ref_t& argument)
      {
         if (withTypeParameters()) {
            int index = parameters.get(name);
            if (index) {
               argument = index;

               return true;
            }
         }
         return false;
      }

      bool isIdentifierParameter(ident_t name, ref_t& argument)
      {
         if (withTypeParameters()) {
            int index = parameters.get(name);
            if (index) {
               argument = index;

               return true;
            }
         }
         return false;
      }

      bool withTypeParameters() const
      {
         return templateMode == stClassTemplate || templateMode == stPropertyTemplate;
      }

      Scope()
      {
         templateMode = ScopeType::stNormal;
      }
   };

   TypedMap      _types;

   int           _level;
   int           _cachingLevel;
   SyntaxTree    _cache;
   SyntaxWriter  _cacheWriter;

   SyntaxWriter  _output;

   _ModuleScope* _scope;
   ident_t       _ns;
   ident_t       _filePath;
   IdentifierList _importedNs;

   void loadTemplateParameters(Scope& scope, SNode node);
   void loadTemplateExprParameters(Scope& scope, SNode node);

   //void appendNode(LexicalType type, int argument);

   //void newNode(LexicalType type, int argument);
   //void newNode(LexicalType type, ident_t value);
   //void newNode(LexicalType type);

   void newNode(Symbol symbol);
   void closeNode();

   void saveScope(SyntaxWriter& writer);

   ref_t mapAttribute(SNode terminal, bool allowType, bool& allowPropertyTemplate);
   void declareAttribute(SNode node);

   void recognizeScope();
   void recognizeDefinition(SNode scopeNode);
   void recognizeScopeAttributes(SNode node, int mode/*, DerivationScope& scope*/);
   void recognizeClassMebers(SNode node/*, DerivationScope& scope*/);

   bool recognizeMetaScope(SNode node);
   
   void generateTemplateTree(SNode node, SNode nameNode, ScopeType templateType);
   void generateScope(SyntaxWriter& writer, SNode node, Scope& scope);
   void generateClosureTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope);
   void generateCodeTemplateTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope);
   void generatePropertyTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void generateClassTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void generateSymbolTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void generateClassTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool nested = false);
   void generateMethodTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool closureMode, bool propertyMode);
   // returns true if in-place init found
   bool generateFieldTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer); 
   void generateCodeTree(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool withBookmark = false*/);
   void generateTokenExpression(SyntaxWriter& writer, SNode& node, Scope& derivationScope, bool rootMode);
   void generateAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool rootMode, bool templateMode, bool expressionMode*/);
   void generateTemplateAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void generateExpressionAttribute(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool rootMode, bool templateMode, bool expressionMode*/, bool templateArgMode = false);
   void generateExpressionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, int mode = 0);
   void generateCollectionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope);
   void generateSwitchTree(SyntaxWriter& writer, SNode current, Scope& derivationScope);
   void generateCodeExpression(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool closureMode);
   void generateIdentifier(SyntaxWriter& writer, SNode current, Scope& derivationScope);
   void generateMesage(SyntaxWriter& writer, SNode current, Scope& derivationScope);

   void declareType(SyntaxWriter& writer, SNode node/*, DerivationScope& scope*/);
   void generateImport(SyntaxWriter& writer, SNode ns);

public:
   void begin();
   void end();

   void newNamespace(ident_t ns, ident_t filePath);
   void importModule(ident_t moduke);
   void closeNamespace();

   virtual void writeSymbol(Symbol symbol);
   virtual void writeTerminal(TerminalInfo& terminal);

   DerivationWriter(SyntaxTree& target, _ModuleScope* scope)
      :  _output(target), _cacheWriter(_cache), _types(NULL, freestr), _importedNs(nullptr, freestr)
   {
      _cachingLevel = _level = 0;

   //   _cachingMode = true; 
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
//         ttMethodTemplate = 2,
         ttCodeTemplate     = 3,
//         ttExtTemplate    = 4
      };

      Type         type;
      ref_t        templateRef;
      ref_t        reference;
      NodeMap      parameterValues;

      _Module*       templateModule;
      _ModuleScope*  moduleScope;

      ident_t          ns;
      ident_t          sourcePath;

      bool withTypeParameters()
      {
         return type == ttPropertyTemplate || type == ttClassTemplate;
      }

      bool generateClassName();

      _Memory* loadTemplateTree();

      TemplateScope(_ModuleScope* moduleScope, ref_t templateRef, ident_t sourcePath, ident_t ns/*, IdentifierList* imports*/)
         : parameterValues(SNode())
      {
         this->sourcePath = sourcePath;
         this->ns = ns;

         this->moduleScope = moduleScope;
//         this->imports = imports;
//         this->parent = NULL;

         type = Type::ttClassTemplate;
         this->templateRef = templateRef;
         this->reference = 0;
         this->templateModule = nullptr;
//         autogeneratedTree = NULL;
//         mode = 0;
//         extensionTemplateRef = 0;
      }
   };

   SNode       _root;
   void copyNodes(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyChildren(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyTreeNode(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);   

   bool generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass/*, int mode = 0*/);

public:
   ref_t generateTemplate(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters, bool importMode = false);
   void generateTemplateCode(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters);
   void generateTemplateProperty(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters);

   void importClass(SyntaxWriter& output, SNode node);

   TemplateGenerator(SyntaxTree& tree);
};

} // _ELENA_

#endif // derivationH
