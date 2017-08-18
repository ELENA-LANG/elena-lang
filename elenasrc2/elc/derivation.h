//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Derivation Tree classes
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef derivationH
#define derivationH 1

#include "syntax.h"
#include "compilercommon.h"

#define EXPRESSION_EXPLICIT_MODE   1
#define EXPRESSION_MESSAGE_MODE    2

namespace _ELENA_
{

// --- DerivationWriter ---

class DerivationWriter : public _DerivationWriter
{
   SyntaxWriter _writer;

   void writeNode(Symbol symbol);

public:
   virtual void writeSymbol(Symbol symbol);
   virtual void writeTerminal(TerminalInfo& terminal);

   DerivationWriter(SyntaxTree& target)
      : _writer(target)
   {
   }
};

// --- DerivationWriter ---

class DerivationReader : public _DerivationReader
{
   // --- TemplateScope ---
   struct DerivationScope
   {
      enum Type
      {
         ttNone           = 0,
         ttFieldTemplate  = 1,
         ttMethodTemplate = 2,
         ttCodeTemplate   = 3,
      };

      Type         type;
      ref_t        templateRef;  // NOTE : should be zero for template declaration
      ref_t        reference;
      ForwardMap   parameters;
      ForwardMap   fields;
      SubjectMap   attributes;
      ident_t      sourcePath;

      SNode        exprNode;     // used for code template
      SNode        codeNode;
      SNode        elseNode;
      SNode        nestedNode;
      SNode        identNode;

      SyntaxTree* autogeneratedTree;

      _CompilerScope*  moduleScope;
      DerivationScope* parent;

      void raiseError(const char* message, SNode terminal)
      {
         moduleScope->raiseError(message, terminal);
      }

      bool isTypeAttribute(SNode terminal);
      bool isAttribute(SNode terminal);
      bool isImplicitAttribute(SNode terminal);
      bool isSubject(SNode terminal)
      {
         return !(isTypeAttribute(terminal) || isAttribute(terminal));
      }

      void loadAttributeValues(SNode node, bool classMode = false);
      void loadParameters(SNode node);
      void loadFields(SNode node);

      void copySubject(SyntaxWriter& writer, SNode terminal);
      void copyIdentifier(SyntaxWriter& writer, SNode terminal);

      ref_t mapNewReference(ident_t identifier);

      int mapParameter(SNode terminal);
      ref_t mapAttribute(SNode terminal, int& paramIndex);
      ref_t mapAttribute(SNode terminal)
      {
         int dummy = 0;
         return mapAttribute(terminal, dummy);
      }

      ref_t mapTemplate(SNode terminal, int prefixCounter = 0);
      ref_t mapClassTemplate(SNode terminal);
      ref_t mapTerminal(SNode terminal, bool existing = false);
      int mapIdentifier(SNode terminal);

      bool generateClassName();

      _Memory* loadTemplateTree();

      DerivationScope(DerivationScope* parent, ref_t attrRef)
      {
         this->moduleScope = parent->moduleScope;
         this->parent = parent;

         type = Type::ttNone;
         templateRef = attrRef;
         reference = 0;
         sourcePath = NULL;
         autogeneratedTree = parent->autogeneratedTree;
      }
      DerivationScope(_CompilerScope* moduleScope)
      {
         this->moduleScope = moduleScope;
         this->parent = NULL;

         type = Type::ttNone;
         templateRef = 0;
         reference = 0;
         sourcePath = NULL;
         autogeneratedTree = NULL;
      }
   };

   enum DeclarationAttr
   {
      daNone     = 0x00,
      daType     = 0x01,
      daClass    = 0x02,
      daTemplate = 0x04,
      daField    = 0x08,
      daMethod   = 0x10, 
      daLoop     = 0x20,
      daImport   = 0x40,
      daExtern   = 0x80
   };

   SNode       _root;
   MessageMap  _verbs;                            // list of verbs

   void copyFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer);
   void copyExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void copyTreeNode(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, bool methodMode = false*/);
   void copyMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer);
   void copyTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributeValues);
   void copyTemplateAttributeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, bool variableMode*/);

   bool generateTemplate(SyntaxWriter& writer, DerivationScope& scope, bool declaringClass/*, bool embeddableMode*/);

   void generateScopeMembers(SNode node, DerivationScope& scope);   

   void generateSwitchTree(SyntaxWriter& writer, SNode current, DerivationScope& scope);
   bool generateTemplateCode(SyntaxWriter& writer, DerivationScope& scope);
   void generateCodeTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void generateVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void generateArrayVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void generateMessageTree(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, bool operationMode*/);
   void generateClosureTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void generateCodeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void generateFieldTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode = false);
   void generateMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode = false);
   void generateFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode = false);
   void generateObjectTree(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, int mode = 0*/);
   void generateExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, int mode = EXPRESSION_EXPLICIT_MODE);
   void generateNewOperator(SyntaxWriter& writer, SNode node, DerivationScope& scope);
   void generateNewTemplate(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode);
   void generateAttributes(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, bool templateMode);
   void generateSymbolTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes);
   void generateClassTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, int nested = 0);

   bool generateMethodScope(SNode node, DerivationScope& scope, SNode attributes);
   bool generateSingletonScope(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes);
   bool generateDeclaration(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes);
   void generateTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes);
   void generateTemplateVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode);
   void generateAttributeTemplate(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode);

   void generateSyntaxTree(SyntaxWriter& writer, SNode node, _CompilerScope& scope, SyntaxTree& autogenerated);
   void generateScope(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes);

   bool declareType(SyntaxWriter& writer, SNode member, DerivationScope& scope, SNode attributes);
   void includeModule(SNode ns, _CompilerScope& scope);

   void saveTemplate(_Memory* target, SNode node, _CompilerScope& scope, SNode attributes, DerivationScope::Type type, SyntaxTree& autogenerated);

   bool isVerbAttribute(SNode terminal);

public:
   virtual void generateSyntaxTree(SyntaxWriter& writer, _CompilerScope& scope);

   DerivationReader(SyntaxTree& tree);
};

} // _ELENA_

#endif // derivationH
