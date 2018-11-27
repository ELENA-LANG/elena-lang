//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Derivation Tree classes
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef derivationH
#define derivationH 1

#include "syntax.h"
#include "compilercommon.h"

namespace _ELENA_
{

// --- DerivationWriter ---

class DerivationWriter : public _DerivationWriter
{
   int           _level;
   int           _cachingLevel;
   SyntaxTree    _cache;
   SyntaxWriter  _cacheWriter;

   SyntaxWriter  _writer;

   _ModuleScope* _scope;
   ident_t       _ns;
   ident_t       _filePath;

   //void appendNode(LexicalType type, int argument);

   //void newNode(LexicalType type, int argument);
   //void newNode(LexicalType type, ident_t value);
   //void newNode(LexicalType type);

   void newNode(Symbol symbol);
   void closeNode();

   void saveScope();

   ref_t mapAttribute(SNode terminal/*, bool& templateParam*/);
   void declareAttribute(SNode node);

   void recognizeScope();
   void recognizeScopeAttributes(SNode node, int mode/*, DerivationScope& scope*/);
   void recognizeClassMebers(SNode node/*, DerivationScope& scope*/);

   void generateScope(SNode node);
   void generateSymbolTree(SNode node);
   void generateClassTree(SNode node/*, DerivationScope& scope, int nested = 0*/);
   void generateMethodTree(SNode node/*, DerivationScope& scope, bool templateMode, bool closureMode*/);
   void generateCodeTree(SNode node/*, DerivationScope& scope, bool withBookmark = false*/);
   void generateAttributes(SNode node/*, DerivationScope& scope, bool rootMode, bool templateMode, bool expressionMode*/);
   void generateExpressionTree(SNode node/*, DerivationScope& scope*/, int mode = 0);

public:
   void begin();
   void end();

   void newNamespace(ident_t ns, ident_t filePath);
   void closeNamespace();

   virtual void writeSymbol(Symbol symbol);
   virtual void writeTerminal(TerminalInfo& terminal);

   DerivationWriter(SyntaxTree& target, _ModuleScope* scope)
      :  _writer(target), _cacheWriter(_cache)
   {
      _cachingLevel = _level = 0;

   //   _cachingMode = true; 
      _scope = scope;

      _cacheWriter.newNode(lxRoot);
   }
};

//// --- DerivationWriter ---
//
//class DerivationTransformer //: public _DerivationTransformer
//{
//   // --- TemplateScope ---
//   struct DerivationScope
//   {
//      enum Type
//      {
//         ttNone           = 0,
//         ttFieldTemplate  = 1,
//         ttMethodTemplate = 2,
//         ttCodeTemplate   = 3,
//         ttExtTemplate    = 4
//      };
//
//      Type         type;
//      ref_t        templateRef;  // NOTE : should be zero for template declaration
//      ref_t        reference;
//      ForwardMap   parameters;
//      ForwardMap   fields;
//      SubjectMap   parameterValues;
//
//      SNode        exprNode;     // used for code template
//      SNode        codeNode;
//      SNode        elseNode;
//      SNode        nestedNode;
//      SNode        identNode;
//
//      SyntaxTree* autogeneratedTree;
//
//      _CompilerScope*  compilerScope;
//      DerivationScope* parent;
//      IdentifierList*  imports;
//
//      int              mode;
//      ref_t            extensionTemplateRef;
//
//      ident_t          ns;
//      ident_t          sourcePath;
//
//      ref_t mapIdentifier(ident_t identifier, bool referenceMode);
//      ref_t mapReference(SNode terminal);
//      ref_t mapAttribute(SNode terminal, int& paramIndex);
//      ref_t mapAttribute(SNode terminal)
//      {
//         int dummy = 0;
//         return mapAttribute(terminal, dummy);
//      }
//
////      ref_t mapIdentifier(ident_t identifier);
//
//      ident_t extractPath()
//      {
//         size_t pos = sourcePath.findLast('\'');
//         if (pos != NOTFOUND_POS) {
//            return sourcePath + pos + 1;
//         }
//         else return sourcePath;
//      }
//
//      void raiseError(const char* message, SNode terminal)
//      {
//         compilerScope->raiseError(message, extractPath(), terminal);
//      }
//      void raiseWarning(int level, const char* message, SNode terminal)
//      {
//         compilerScope->raiseWarning(level, message, extractPath(), terminal);
//      }
//
////      bool isTypeAttribute(SNode terminal);
////      bool isAttribute(SNode terminal);
////      bool isImplicitAttribute(SNode terminal);
////      bool isSubject(SNode terminal)
////      {
////         return !(isTypeAttribute(terminal) || isAttribute(terminal));
////      }
//
//      void loadParameters(SNode node);
//      void loadFields(SNode node);
//
//      void copyName(SyntaxWriter& writer, SNode terminal);
////      void copyMessageName(SyntaxWriter& writer, SNode terminal);
//////      void copyMessage(SyntaxWriter& writer, SNode terminal);
//////      void copyIdentifier(SyntaxWriter& writer, SNode terminal);
////
////      ref_t mapNewReference(ident_t identifier);
////
//      ref_t mapNewIdentifier(ident_t name, bool privateOne);
//      int mapParameter(ident_t identifier);
////      ref_t mapAttributeType(SNode terminal/*, bool& arrayMode, bool& paramMode*/);
//
//      ref_t mapTemplate(SNode terminal/*, int paramCounter = 0, int prefixCounter = 0*/);
////      ref_t mapClassTemplate(SNode terminal);
////      ref_t mapTerminal(SNode terminal, bool existing = false);
////      ref_t mapTypeTerminal(SNode terminal, bool existing = false);  // map type or reference
////      int mapIdentifier(SNode terminal);
////
////      ref_t mapTypeTemplate(SNode current);
//
//      bool generateClassName();
//
//      _Memory* loadTemplateTree();
//
//      DerivationScope(DerivationScope* parent, ref_t attrRef)
//      {
//         this->sourcePath = parent->sourcePath;
//         this->ns = parent->ns;
//
//         this->compilerScope = parent->compilerScope;
//         this->imports = parent->imports;
//         this->parent = parent;
//
//         type = Type::ttNone;
//         templateRef = attrRef;
//         reference = 0;
//         this->autogeneratedTree = parent->autogeneratedTree;
//         mode = 0;
//         extensionTemplateRef = 0;
//      }
//      DerivationScope(_CompilerScope* compilerScope, ident_t sourcePath, ident_t ns, IdentifierList* imports)
//      {
//         this->sourcePath = sourcePath;
//         this->ns = ns;
//
//         this->compilerScope = compilerScope;
//         this->imports = imports;
//         this->parent = NULL;
//
//         type = Type::ttNone;
//         templateRef = 0;
//         reference = 0;
//         autogeneratedTree = NULL;
//         mode = 0;
//         extensionTemplateRef = 0;
//      }
//   };
//
//   enum DeclarationAttr
//   {
//      daNone        = 0x0000,
//      daType        = 0x0001,
//      daClass       = 0x0002,
//      daTemplate    = 0x0004,
//      daField       = 0x0008,
//      daLoop        = 0x0020,
//      daImport      = 0x0040,
//      daExtern      = 0x0080,
//      daAccessor    = 0x0100,
//      daDblAccessor = 0x0300,
//      daCodeMask    = 0xF800,
//      daCode        = 0x0800,
//      daBlock       = 0x1800,
//      daDblBlock    = 0x3800,
//      daNestedBlock = 0x4800,
//      daExtension   = 0x8000
//   };
//
//   SNode       _root;
////   MessageMap  _verbs;                            // list of verbs
//
//   bool compareAttributes(SNode node, DerivationScope& scope);
//
//   void loadParameterValues(SNode node, DerivationScope& scope, SubjectMap* parentParameterValues/*, bool classMode = false*/);
//
////   void copyIdentifier(SyntaxWriter& writer, SNode terminal);   
//   void copyParamAttribute(SyntaxWriter& writer, SNode current, DerivationScope& scope);
//   void copyFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyFieldInitTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyTreeNode(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributeValues, SubjectMap* parentAttributes, int mode = 0);
//   void copyTemplateAttributeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyOperator(SyntaxWriter& writer, SNode& node);
//   void copyClassTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void copyTemplateInitBody(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//
//   void autoGenerateExtensions(DerivationScope& templateScope);
//   ref_t generateNewTemplate(DerivationScope& scope);
//   ref_t generateNewTemplate(DerivationScope& scope, ref_t attrRef, SNode node);
//   ref_t generateNewTemplate(SNode current, DerivationScope& scope, SubjectMap* parentAttributes);
//
//   bool generateTemplate(SyntaxWriter& writer, DerivationScope& scope, bool declaringClass, int mode = 0);
//
//
////   ref_t mapAttributeType(SNode attr, DerivationScope& scope);
//
//   ref_t mapTemplateName(SNode node, int prefixCounter, DerivationScope& scope, int postfixCounter = 0);
//   ref_t mapCodeTemplateName(SNode node, int codeCounter, int nestedCounter, int parameterCounter, DerivationScope& scope);
//
//   ref_t mapAttribute(SNode terminal, DerivationScope& scope, bool& templateParam);
//
//   ref_t mapNewTemplate(SNode node, DerivationScope& scope, bool operationMode, bool& arrayMode, int& paramIndex, bool templateMode, List<int>* templateAttributes);
//
//   void recognizeScopeMembers(SNode& node, DerivationScope& scope, int mode);
//
////   bool isImplicitAttribute(SNode node, DerivationScope& scope);
//
//   bool checkVariableDeclaration(SNode node, DerivationScope& scope);
//   bool checkPatternDeclaration(SNode node, DerivationScope& scope);
////   bool checkArrayDeclaration(SNode node, DerivationScope& scope);
//
//   void generateMessage(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, bool templateMode*/);
//   void generateTypeAttribute(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode);
//
//   void generateSwitchTree(SyntaxWriter& writer, SNode current, DerivationScope& scope);
//   bool generateCodeTemplate(SyntaxWriter& writer, DerivationScope& scope, SubjectMap* parentAttributes);
//   void generateCodeTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void generateVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
////   void generateArrayVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//     void generateMessageTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void generateClosureTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   bool generateFieldTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer, bool templateMode = false);
//   bool generateFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer, bool templateMode/* = false*/); // returns true if in-place init found
//   void generateCodeExpression(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void generateObjectTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, int mode = 0);
//   void generateAssignmentOperator(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void generateTemplateParameters(SNode& node, DerivationScope& scope, bool templateMode);
//   void generateSubTemplate(SNode& node, DerivationScope& scope, bool templateMode);
//   void generateNewTemplate(SyntaxWriter& writer, SNode& node, DerivationScope& scope, bool templateMode);
//   void generateSymbolTree(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   void generateTemplateScope(SNode node, DerivationScope& scope);
//
//
//   bool recognizeMethodScope(SNode node);
//   bool generateSingletonScope(SyntaxWriter& writer, SNode node, DerivationScope& scope);
//   bool recognizeDeclaration(SNode node, DerivationScope& scope);
//   void generateTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, SNode attributes*/);
//   void generateAttributeTemplate(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode/*, int mode = 0*/, bool expressionMode);
//
////   void generateSyntaxTree(SyntaxWriter& writer, SNode node, CompilerScope& scope, SyntaxTree& autogenerated, 
////      MessageMap* attributes, IdentidierList* imports, ident_t sourcePath);
////   void generateScope(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, SNode attributes*/, int mode);
//
//   void declareType(/*SyntaxWriter& writer, */SNode node, DerivationScope& scope);
//   void includeModule(SNode ns, DerivationScope& scope);
//
//   void saveTemplate(SNode node, DerivationScope& scope, DerivationScope::Type type, SyntaxTree& autogenerated, ref_t templateRef);
//
////   bool isVerbAttribute(SNode terminal);
//
//   void recognizeRootScope(SNode node, DerivationScope& scope/*, int mode*/);
//
//   void recognize(_CompilerScope& scope, SNode node, ident_t sourcePath, ident_t ns, IdentifierList* imports);
//
//   void generate(SyntaxWriter& writer, SNode node, _CompilerScope& scope, ident_t sourcePath, ident_t ns,
//      IdentifierList* imports, SyntaxTree& autogenerated);
//
//public:
//   void recognize(_CompilerScope& scope, ident_t sourcePath, ident_t ns, IdentifierList* imports);
//
//   void generate(SyntaxWriter& writer, _CompilerScope& scope, ident_t sourcePath, ident_t ns, IdentifierList* imports);
//
//   ref_t generateTemplate(SyntaxWriter& writer, _CompilerScope& scope, ref_t reference, List<ref_t>& parameters);
//
//   DerivationTransformer(SyntaxTree& tree);
//};

} // _ELENA_

#endif // derivationH
