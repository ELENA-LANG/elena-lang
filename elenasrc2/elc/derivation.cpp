//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Derivation Tree class implementation
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "derivation.h"
#include "errors.h"
#include "bytecode.h"

using namespace _ELENA_;

//inline bool isPrimitiveRef(ref_t reference)
//{
//   return (int)reference < 0;
//}

constexpr auto MODE_ROOT            = 0x01;
//#define MODE_CODETEMPLATE    0x02
//#define MODE_OBJECTEXPR      0x04
////#define MODE_SIGNATURE       0x08
//#define MODE_IMPORTING       0x10
//#define MODE_MESSAGE_BODY    0x20  // indicates that sub-expressions should be an expression themselves
constexpr auto MODE_PROPERTYALLOWED = 0x40;

constexpr auto MODE_CLOSURE        = -2u;
constexpr auto MODE_COMPLEXMESSAGE = -3u;
constexpr auto MODE_PROPERTYMETHOD = -4u;

#define EXPRESSION_IMPLICIT_MODE   1
////#define EXPRESSION_MESSAGE_MODE    2
//#define EXPRESSION_OPERATOR_MODE   4
//#define EXPRESSION_OBJECT_REQUIRED 8

//void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

// --- DerivationWriter ---

inline SNode goToNode(SNode current, LexicalType type)
{
   while (current != lxNone && current != type)
      current = current.nextNode();

   return current;
}

inline SNode goToNode(SNode current, LexicalType type1, LexicalType type2)
{
   while (current != lxNone && !current.compare(type1, type2))
      current = current.nextNode();

   return current;
}

inline SNode goToNode(SNode current, LexicalType type1, LexicalType type2, LexicalType type3)
{
   while (current != lxNone && !current.compare(type1, type2, type3))
      current = current.nextNode();

   return current;
}

inline bool isTerminal(LexicalType type)
{
   return test(int(type), lxTerminalMask);

//   switch (type)
//   {
//      case lxIdentifier:
//      case lxPrivate:
//      case lxInteger:
//      case lxHexInteger:
//      case lxLong:
//      case lxReal:
//      case lxLiteral:
//      case lxReference:
//      case lxCharacter:
//      case lxWide:
//      case lxExplicitConst:
//      case lxMemberIdentifier:
//      case lxGlobalReference:
//         return true;
//      default:
//         return false;
//   }
}

inline void copyIdentifier(SyntaxWriter& writer, SNode ident)
{
   ident_t s = ident.identifier();
   if (!emptystr(s)) {
      writer.newNode(ident.type, s);
   }
   else writer.newNode(ident.type);

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);

   writer.closeNode();
}

inline void insertIdentifier(SyntaxWriter& writer, SNode ident)
{
   SNode col = ident.findChild(lxCol);
   SNode row = ident.findChild(lxRow);
   SNode len = ident.findChild(lxLength);

   writer.insert(0, lxEnding, 0);

   writer.insertChild(0, lxCol, col.argument);
   writer.insertChild(0, lxRow, row.argument);
   writer.insertChild(0, lxLength, len.argument);

   ident_t s = ident.identifier();
   if (!emptystr(s)) {
      writer.insert(0, ident.type, s);
   }
   else writer.insert(0, ident.type, 0);
}

void DerivationWriter :: begin()
{
   _output.newNode(lxRoot);
}

void DerivationWriter :: end()
{
   _output.closeNode();
}

void DerivationWriter :: newNamespace(ident_t ns, ident_t filePath)
{
   _ns = ns;
   _filePath = filePath;

   _output.newNode(lxNamespace, ns);
   _output.appendNode(lxSourcePath, filePath);
}

void DerivationWriter :: importModule(ident_t name)
{
   _output.appendNode(lxImport, name);

   _importedNs.add(name.clone());
}

void DerivationWriter :: closeNamespace()
{
   _output.closeNode();

   _ns = nullptr;
   _filePath = nullptr;

   _importedNs.clear();
}

void DerivationWriter :: newNode(Symbol symbol)
{
   _level++;

   switch (symbol) {
      case nsExpression:
      case nsRootExpression:
      case nsOperandExpression:
      case nsSubExpression:
      case nsSingleExpression:
      case nsSubSingleExpression:
      case nsL2Operand:
      case nsL3Operand:
      case nsL6Operand:
         //      case nsNestedRootExpression:
         _cacheWriter.newNode(lxExpression);
         break;
      case nsCodeEnd:
         _cacheWriter.newNode(lxEOF);
         break;
      case nsRetExpression:
         _cacheWriter.newNode(lxReturning);
         break;
////      case nsMethodParameter:
////         _writer.newNode(lxMethodParameter);
////         break;
////      case nsMessageParameter:
////      case nsExprMessageParameter:
////         _writer.newNode(lxMessageParameter);
////         break;
////      case nsNestedClass:
////         _writer.newNode(lxNestedClass);
////         break;
      case nsResendExpression:
         _cacheWriter.newNode(lxResendExpression);
         break;
//      case nsObject:
//         _writer.newNode(lxObject);
//         break;
//      case nsBaseClass:
//         _writer.newNode(lxBaseParent);
//         break;
      case nsL1Operator:
      case nsL2Operator:
      case nsL3Operator:
      case nsL4Operator:
      case nsL5Operator:
      case nsL6Operator:
         _cacheWriter.newNode(lxOperator);
         break;
      case nsArrayOperator:
         _cacheWriter.newNode(lxOperator, REFER_OPERATOR_ID);
         break;

      case nsSwitching:
         _cacheWriter.newNode(lxSwitching);
         break;
      case nsCollection:
         _cacheWriter.newNode(lxCollection);
         break;
      case nsScope:
      case nsAttribute:
         // whole root scope should be cached
         if (_level == 1)
            _cachingLevel = _level;
      default:
         _cacheWriter.newNode((LexicalType)(symbol & ~mskAnySymbolMask));
         break;
   }
}

void DerivationWriter :: closeNode()
{
   _level--;

   _cacheWriter.closeNode();
   if (_level < _cachingLevel) {
      _cacheWriter.closeNode();

      _cachingLevel = 0;

      saveScope(_output);

      _cacheWriter.newNode(lxRoot);
   }
}

void DerivationWriter :: writeSymbol(Symbol symbol)
{
   if (symbol != nsNone) {
      newNode(symbol);
   }
   else closeNode();
}

//void DerivationWriter :: newNode(LexicalType type)
//{
//   if (_cachingMode) {
//      _cachingLevel++;
//      _cacheWriter.newNode(type);
//   }
//   else _writer.newNode(type);
//}
//
//void DerivationWriter :: newNode(LexicalType type, ident_t value)
//{
//   if (_cachingMode) {
//      _cachingLevel++;
//      _cacheWriter.newNode(type, value);
//   }
//   else _writer.newNode(type, value);
//}
//
//void DerivationWriter :: newNode(LexicalType type, int argument)
//{
//   if (_cachingMode) {
//      _cachingLevel++;
//      _cacheWriter.newNode(type, argument);
//   }
//   else _writer.newNode(type, argument);
//}
//
//void DerivationWriter :: appendNode(LexicalType type, int argument)
//{
//   newNode(type, argument);
//   closeNode();
//}

void DerivationWriter :: writeTerminal(TerminalInfo& terminal)
{
   // HOT FIX : if there are several constants e.g. $10$13, it should be treated like literal terminal
   if (terminal == tsCharacter && terminal.value.findSubStr(1, '$', terminal.length, NOTFOUND_POS) != NOTFOUND_POS) {
      terminal.symbol = tsLiteral;
   }

   LexicalType type = (LexicalType)(terminal.symbol & ~mskAnySymbolMask | lxTerminalMask | lxObjectMask);

   if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
      // try to use local storage if the quote is not too big
      if (getlength(terminal.value) < 0x100) {
         QuoteTemplate<IdentifierString> quote(terminal.value);

         _cacheWriter.newNode(type, quote.ident());
      }
      else {
         QuoteTemplate<DynamicString<char> > quote(terminal.value);

         _cacheWriter.newNode(type, quote.ident());
      }
   }
   else _cacheWriter.newNode(type, terminal.value);

   _cacheWriter.appendNode(lxCol, terminal.col);
   _cacheWriter.appendNode(lxRow, terminal.row);
   _cacheWriter.appendNode(lxLength, terminal.length);
   //   _writer->writeDWord(terminal.disp);

   _cacheWriter.closeNode();
}

void DerivationWriter :: saveScope(SyntaxWriter& writer)
{
   recognizeScope();

   Scope outputScope;
   generateScope(writer, _cache.readRoot(), outputScope);

   _cache.clear();
   _cacheWriter.clear();
}

void DerivationWriter :: loadTemplateParameters(Scope& scope, SNode node)
{
   SNode current = node.findChild(lxToken);
   while (current == lxToken) {
      if (current.existChild(lxToken))
         _scope->raiseError(errInvalidSyntax, _filePath, current);
      
      ident_t name = current.firstChild(lxTerminalMask).identifier();
      
      if(!scope.parameters.add(name, scope.parameters.Count() + 1, true))
         _scope->raiseError(errDuplicatedDefinition, _filePath, current);

      current = current.nextNode();
   }
}

void DerivationWriter :: loadTemplateExprParameters(Scope& scope, SNode node)
{
   SNode current = node.findChild(lxParameter);
   while (current == lxParameter) {
      SNode tokenNode = current.findChild(lxToken);

      if (tokenNode.existChild(lxToken) || tokenNode.nextNode() != lxNone)
         _scope->raiseError(errInvalidSyntax, _filePath, current);

      ident_t name = tokenNode.firstChild(lxTerminalMask).identifier();

      if (!scope.parameters.add(name, scope.parameters.Count() + 1, true))
         _scope->raiseError(errDuplicatedDefinition, _filePath, current);

      current = current.nextNode();
   }
}

void DerivationWriter ::generateTemplateTree(SNode node, SNode nameNode, ScopeType templateType)
{
   Scope templateScope;
   templateScope.templateMode = templateType;
   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
      loadTemplateExprParameters(templateScope, node);
   }

   loadTemplateParameters(templateScope, nameNode);
   
   //      int count = SyntaxTree::countChild(nameNode, lxAttributeValue);
   //      int mode = MODE_ROOT;
   //
   //      IdentifierString templateName(nameNode.findChild(lxIdentifier).identifier());
   //      if (test(declType, daField)) {
   //         templateName.append("#1");
   //
   //         if (count != 1 && count != 2)
   //            scope.raiseError(errInvalidSyntax, node);
   //      }
   //      else if (test(declType, daAccessor)) {
   //         if (test(declType, daDblAccessor)) {
   //            templateName.append("#2");
   //         }
   //         else {
   //            templateName.append("#1");
   //         }
   //      }
   //      else if (test(declType, daCode)) {
   //         mode |= MODE_CODETEMPLATE;
   //
   //         templateName.append("#");
   //
   //         if (test(declType, daDblBlock)) {
   //            templateName.append("#2");
   //         }
   //         else if (test(declType, daBlock)) {
   //            templateName.append("#1");
   //         }
   //         else templateName.append("#0");
   //
   //         if (test(declType, daNestedBlock)) {
   //            templateName.append("#1");
   //         }
   //         else templateName.append("#0");
   //
   //         if (test(declType, daLoop)) {
   //            node.findChild(lxCode).injectNode(lxLoop);
   //         }
   //         else if (test(declType, daExtern)) {
   //            node.findChild(lxCode).injectNode(lxExtern);
   //         }
   //      }
   //      else if (declType == (DeclarationAttr)(daExtension | daTemplate)) {
   //         templateName.append("~");
   //      }
   //
   //      templateName.append('#');
   //      templateName.appendInt(count);
   //
   //      node.set(lxTemplate, scope.mapNewIdentifier(templateName.c_str(), privateOne));
   //
   //      recognizeScopeMembers(node, scope, mode);

   SyntaxTree templateTree;
   SyntaxWriter templateWriter(templateTree);

   generateScope(templateWriter, _cache.readRoot(), templateScope);

   // check for duplicate declaration
   if (_scope->module->mapSection(nameNode.argument | mskSyntaxTreeRef, true))
      _scope->raiseError(errDuplicatedSymbol, _filePath, node);
   
   //   ident_t name = scope.compilerScope->module->resolveReference(templateRef);
   //   // recognize the type
   //   size_t index = name.find('#');
   //   if (name[index + 1] == '#') {
   //      if (name[index + 2] == '2') {
   //         scope.mode = daDblBlock;
   //      }
   //      else if (name[index + 2] == '1') {
   //         scope.mode = daBlock;
   //      }
   //      if (name[index + 3] == '#' && name[index + 4] == '1') {
   //         scope.mode |= daNestedBlock;
   //      }
   //
   //      //scope.type = DerivationScope::ttCodeTemplate;
   //   }
   //   else if (name.find(index + 1, '#', NOTFOUND_POS) != NOTFOUND_POS) {
   //      if (name[index + 1] == '1' || name[index + 1] == '2') {
   //         scope.type = DerivationScope::ttFieldTemplate;
   //      }
   //   }
   //   else if (name.find('~') != NOTFOUND_POS) {
   //      scope.type = DerivationScope::ttExtTemplate;
   //   }
   //
   //   saveTemplate(node, scope, scope.type, *scope.autogeneratedTree, templateRef);
   //   
   _Memory* target = _scope->module->mapSection(nameNode.argument | mskSyntaxTreeRef, false);

//   SyntaxTree tree;
//   SyntaxWriter writer(tree);
//
//   writer.newNode(lxTemplate);
//
//   // HOTFIX : save the template source path
//   IdentifierString fullPath(module->Name());
//   fullPath.append('\'');
//   fullPath.append(scope.sourcePath);
//
//   DerivationScope rootScope(scope.compilerScope, fullPath.c_str(), scope.ns, scope.imports);
//   rootScope.autogeneratedTree = &autogenerated;
//   rootScope.loadParameters(node.prevNode());
////   rootScope.sourcePath = fullPath;
//   rootScope.type = type;
//   rootScope.mode = scope.mode;
//
//   //int mode = MODE_ROOT;
//   /*else if (type == DerivationScope::ttCodeTemplate) {
//      mode |= MODE_CODETEMPLATE;
//      
//      rootScope.mode = node.argument;
//   }*/
//   /*else */if (type == DerivationScope::ttExtTemplate) {
//      rootScope.extensionTemplateRef = templateRef;
//   }
//
//   generateTemplateTree(writer, node, rootScope/*, attributes*/);
//
//   writer.closeNode();

   SNode root = templateTree.readRoot();
   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
      // COMPILER MAGIC : code template : find the method body and ignore the rest, save as the attribute
      root = root.findChild(lxClassMethod).findChild(lxCode);

      ReferenceName refName(_scope->module->resolveReference(nameNode.argument));

      _scope->attributes.add(refName.ident(), nameNode.argument);
      _scope->saveAttribute(refName.ident(), nameNode.argument);
   }
   else if (templateScope.templateMode == ScopeType::stPropertyTemplate) {
      ReferenceName refName(_scope->module->resolveReference(nameNode.argument));

      _scope->attributes.add(refName.ident(), nameNode.argument);
      _scope->saveAttribute(refName.ident(), nameNode.argument);
   }

   SyntaxTree::saveNode(root, target);
}

bool DerivationWriter :: recognizeMetaScope(SNode node)
{
   // recognize the declaration type
   DeclarationAttr declType = daNone;
   SNode nameNode;
   SNode current = node.prevNode();
   if (current == lxNameAttr) {
      nameNode = current;
      current = current.prevNode();
   }      
   
   //   bool privateOne = true;
   while (current == lxAttribute/* || current == lxAttributeDecl*/) {
      switch (current.argument) {
         case V_TYPETEMPL:
            declType = (DeclarationAttr)(declType | daType);
            break;
         case V_PROPERTY:
            declType = (DeclarationAttr)(declType | daProperty);
            break;
         case V_IMPORT:
            declType = (DeclarationAttr)(declType | daImport);
            break;
         default:
            break;
      }

   //      ref_t attrRef = scope.mapAttribute(current);
   //      current.setArgument(attrRef);
   //
   //      DeclarationAttr attr = daNone;
   //      switch (attrRef) {
   //         case V_TYPETEMPL:
   //            if (!current.existChild(lxAttributeValue))
   //               attr = daType;
   //            break;
   //         case V_CLASS:
   //         case V_STRUCT:
   //         //case V_STRING:
   //            attr = daClass;
   //            break;
   //         case V_TEMPLATE:
   //            attr = daTemplate;
   //            break;
   //         case V_EXTENSION:
   //            attr = daExtension;
   //            if (current.existChild(lxAttributeValue) && SyntaxTree::countChild(current, lxAttributeValue) == 1) {
   //               // HOTFIX : recognize typified extension
   //               current = lxRefAttribute;
   //            }
   //            break;
   //         case V_FIELD:
   //            attr = daField;
   //            break;
   //         case V_LOOP:
   //            attr = (DeclarationAttr)(daLoop | daTemplate | daBlock);
   //            break;
   //         case V_ACCESSOR:
   //            if (test(declType, daAccessor)) {
   //               if (test(declType, daDblAccessor)) {
   //                  scope.raiseError(errInvalidHint, node);
   //               }
   //               else attr = daDblAccessor;
   //            }
   //            else attr = daAccessor;
   //            break;
   //         case V_IMPORT:
   //            attr = daImport;
   //            break;
   //         case V_EXTERN:
   //            attr = (DeclarationAttr)(daExtern | daTemplate | daBlock);
   //            break;
   //         case V_BLOCK:
   //            if (test(declType, daBlock)) {
   //               if (test(declType, daDblBlock)) {
   //                  scope.raiseError(errInvalidHint, node);
   //               }
   //               else attr = daDblBlock;
   //            }
   //            else attr = daBlock;
   //            break;
   //         case V_NESTEDBLOCK:
   //            if (test(declType, daNestedBlock)) {
   //               scope.raiseError(errInvalidHint, node);
   //            }
   //            else attr = daNestedBlock;
   //            break;
   //         case V_PUBLIC:
   //            privateOne = false;
   //            break;
   //         case V_INTERNAL:
   //            if (!privateOne)
   //               scope.raiseError(errInvalidHint, current);
   //            break;
   //         default:
   //            break;
   //      }
   //      declType = (DeclarationAttr)(declType | attr);
   
         current = current.prevNode();
   }
   
   if (nameNode.existChild(lxToken)) {
      declType = (DeclarationAttr)(declType | daTemplate);
   }

   ////   attributes.refresh();
   //
   if (declType == daType) {
      node = lxForward;

      return true;
   }
   else if (declType == daImport) {
      SNode name = node.prevNode();
      if (name == lxNameAttr) {
         node = lxImport;

         return true;
      }
      else return false;
   }
   else if (test(declType, daTemplate)) {
      if (testany(declType, daImport | daType))
         _scope->raiseError(errInvalidSyntax, _filePath, node);
   
      recognizeDefinition(node);

      ScopeType type = ScopeType::stClassTemplate;
      if (node.existChild(lxCode)) {
         type = ScopeType::stCodeTemplate;
      }
      else if (test(declType, daProperty)) {
         type = ScopeType::stPropertyTemplate;
      }

      generateTemplateTree(node, nameNode, type);

      node = lxIdle;

      return true;
   }
   //   else if (test(declType, daClass)) {
   //      return false;
   //   }
   //   else return false;

   return false;
}

void DerivationWriter :: recognizeDefinition(SNode scopeNode)
{
   SNode bodyNode = scopeNode.firstChild();
   if (scopeNode.existChild(lxCode)) {
      // HOTFIX : recognize returning expression
      //         SNode body = node.findChild(lxCode, lxExpression, lxDispatchCode/*, lxReturning*/, lxResendExpression);
      //         if (body == lxExpression)
      //            body = lxReturning;
      //
      // mark one method class declaration
      scopeNode.set(lxClass, MODE_CLOSURE);
   }
   else if (bodyNode == lxExpression) {
      scopeNode = lxSymbol;
   }
   else if (bodyNode == lxSizeDecl) {
      _scope->raiseError(errInvalidSyntax, _filePath, bodyNode);
   }
   else {
      scopeNode = lxClass;

      recognizeClassMebers(scopeNode);
   }
}

void DerivationWriter :: recognizeScope()
{
   SNode scopeNode = _cache.readRoot().lastChild();
   if (scopeNode == lxScope) {
      recognizeScopeAttributes(scopeNode.prevNode(), MODE_ROOT);

      if (!recognizeMetaScope(scopeNode)) {
         recognizeDefinition(scopeNode);
      }
   }
   else if (scopeNode == lxAttributeDecl) {
      declareAttribute(scopeNode);
   }
}

ref_t DerivationWriter :: mapAttribute(SNode node, bool allowType, bool& allowPropertyTemplate)
{
   if (node == lxIdentifier) {
      ident_t token = node.identifier();

      ref_t ref = _scope->attributes.get(token);
      if (!isPrimitiveRef(ref) && !allowType)
         _scope->raiseError(errInvalidHint, _filePath, node);

      return ref;
   }
   else {
      ref_t attrRef = 0;

      bool tokenNode = node.existChild(lxToken);
      bool sizeNode = node.existChild(lxDynamicSizeDecl);
      if (!allowType && (tokenNode || sizeNode))
         _scope->raiseError(errInvalidHint, _filePath, node);

      if (tokenNode)
         return V_TEMPLATE;

      SNode terminal = node.firstChild(lxTerminalMask);

      ident_t token = terminal.identifier();
      ref_t ref = _scope->attributes.get(token);      
      if (allowType) {
         // HOTFIX : check if the type was declared in the scope
         if (!ref && _types.exist(token))
            terminal.set(lxReference, _types.get(token));

         if (!isPrimitiveRef(ref))
            attrRef = ref;

         if (attrRef || !ref)
            return attrRef;
      }

      if (allowPropertyTemplate) {
         // COMPILER MAGIC : recognize property template
         IdentifierString templateName(token);
         int paramCount = allowType ? 0 : 1;

         SNode scopeNode = goToNode(node, lxClassField);
         if (scopeNode.existChild(lxScope)) {
            // if the property has a body
            recognizeClassMebers(scopeNode);

            paramCount += SyntaxTree::countChild(scopeNode, lxClassMethod);
         }

         templateName.append('#');
         templateName.appendInt(paramCount);

         ref_t templateRef = _scope->attributes.get(templateName.ident());
         if (templateRef)
            return V_PROPERTY;
      }

      if (!isPrimitiveRef(ref) && !allowType)
         _scope->raiseError(errInvalidHint, _filePath, node);

      return ref;
   }
}

void DerivationWriter :: declareAttribute(SNode node)
{
   SNode nameAttr = node.findChild(lxToken);
   ident_t name = nameAttr.findChild(lxIdentifier).identifier();

   SNode attrNode = node.firstChild();
   ref_t attrRef = 0;
   if (attrNode == lxExplicitAttr) {
      ident_t value = attrNode.identifier();
      
      attrRef = -value.toInt(1);
   }

   if (attrRef && _scope->attributes.add(name, attrRef, true)) {
      _scope->saveAttribute(name, attrRef);
   }
   else _scope->raiseError(errDuplicatedDefinition, _filePath, nameAttr);
}

void DerivationWriter :: recognizeScopeAttributes(SNode current, int mode/*, DerivationScope& scope*/)
{
   // set name
   SNode nameNode = current;
   nameNode = lxNameAttr;

   current = current.prevNode();

   bool privateOne = true;
   bool visibilitySet = false;
   bool allowType = true;
   bool allowPropertyTemplate = test(mode, MODE_PROPERTYALLOWED);
   while (current == lxToken/*, lxRefAttribute*/) {
      ref_t attrRef = mapAttribute(current, allowType, allowPropertyTemplate);
      if (isPrimitiveRef(attrRef)) {
         current.set(lxAttribute, attrRef);
         if (test(mode, MODE_ROOT) && (attrRef == V_PUBLIC || attrRef == V_INTERNAL)) {
            // the symbol visibility should be provided only once
            if (!visibilitySet) {
               privateOne = attrRef == V_INTERNAL;
               visibilitySet = true;
            }
            else _scope->raiseError(errInvalidHint, _filePath, current);
         }
      }
      else if (attrRef != 0 || allowType) {
         current.set(lxTarget, attrRef);
      }
      else _scope->raiseWarning(WARNING_LEVEL_2, wrnUnknownHint, _filePath, current);

      allowType = false;
      current = current.prevNode();
   }
   
   SNode nameTerminal = nameNode.firstChild(lxTerminalMask);
   //   if (nameTerminal != lxIdentifier)
   //      scope.raiseError(errInvalidSyntax, nameNode);
   
   IdentifierString name(nameTerminal.identifier().c_str());
   if (nameNode.existChild(lxToken)) {
      // if it is a template identifier      
      bool codeTemplate = nameNode.nextNode().findChild(lxCode) == lxCode;
      if (codeTemplate && nameNode.nextNode().existChild(lxParent)) {
         // COMPILER MAGIC : if it is complex code template
         SNode subNameNode = nameNode.nextNode().findChild(lxParent);
         while (subNameNode == lxParent) {
            name.append(':');
            name.append(subNameNode.findChild(lxToken).firstChild(lxTerminalMask).identifier());

            subNameNode = subNameNode.nextNode();
         }
      }

      int paramCounter = SyntaxTree::countChild(nameNode, lxToken);
      name.append('#');
      name.appendInt(paramCounter);
      if (codeTemplate) {
         int subParamCounter = SyntaxTree::countChild(nameNode.nextNode(), lxParameter);
         name.append('#');
         name.appendInt(subParamCounter);
      }
   }
   
   //   // verify if there is an attribute with the same name
   //   if (scope.compilerScope->attributes.exist(name))
   //      scope.raiseWarning(WARNING_LEVEL_2, wrnAmbiguousIdentifier, nameNode);
   
   if (test(mode, MODE_ROOT))
      nameNode.setArgument(_scope->mapNewIdentifier(_ns.c_str(), name.ident(), privateOne));
}

void DerivationWriter :: recognizeClassMebers(SNode node/*, DerivationScope& scope*/)
{
   bool firstParent = true;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxScope) {
         SNode bodyNode = current.findChild(lxCode, lxDispatchCode, lxReturning, lxExpression, lxResendExpression);

         int mode = 0;
         if (bodyNode == lxExpression) {
            // if it is a property, mark it as a get-property
            current.set(lxClassMethod, MODE_PROPERTYMETHOD);
         }
         else if (bodyNode != lxNone) {
            // if it is a method
            current = lxClassMethod;
         }
         else if (current.firstChild().compare(lxSizeDecl, lxFieldInit, lxNone) || current.existChild(lxScope)) {
            // if it is a field
            current = lxClassField;
            mode = MODE_PROPERTYALLOWED;
         }
         else _scope->raiseError(errInvalidSyntax, _filePath, current);

         recognizeScopeAttributes(current.prevNode(), mode);
      }
      else if (current == lxParent) {
         recognizeScopeAttributes(current.lastChild(), 0);
         //         if (current.existChild(lxAttributeValue)) {
         //            if (firstParent) {               
         //               writer.newNode(lxBaseParent);
         //               writer.appendNode(lxTemplate);
         //               generateAttributeTemplate(writer, current, scope, false, false);
         //               copyIdentifier(writer, current.firstChild(lxTerminalMask));
         //               writer.closeNode();
         //            }
         //            else {
         //               int paramCounter = SyntaxTree::countChild(current, lxAttributeValue);
         //
         //               ref_t attrRef = mapTemplateName(current.firstChild(lxTerminalMask), paramCounter, scope);
         //               if (!attrRef)
         //                  scope.raiseError(errInvalidHint, current);
         //
         //               DerivationScope templateScope(&scope, attrRef);
         //               copyTemplateTree(writer, current, templateScope, current.firstChild(), &scope.parameterValues, MODE_IMPORTING);
         //            }
         //         }
         //         else if (firstParent) {
         //            SNode terminalNode = current.firstChild(lxTerminalMask);
         //
         //            ref_t reference = scope.mapReference(terminalNode);
         //            if (!reference) {
         //               scope.raiseError(errUnknownSubject, terminalNode);
         //            }
         //            else if (isPrimitiveRef(reference))
         //               scope.raiseError(errInvalidHint, terminalNode);
         //
         //            writeParentFullReference(writer, scope.compilerScope->module, reference, terminalNode);
         //         }
         //         else scope.raiseError(errInvalidSyntax, node);
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: generateScope(SyntaxWriter& writer, SNode node, Scope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxSymbol:
         {
//            DerivationScope rootScope(&scope, sourcePath, ns, imports);
//            rootScope.autogeneratedTree = &autogenerated;
//
//            if (!generateSingletonScope(writer, current, rootScope)) {
               generateSymbolTree(writer, current, scope);
//            }            
            break;
         }
         case lxClass:
         {
            generateClassTree(writer, current, scope);
            break;
         }
         case lxForward:
            declareType(writer, current);
            break;
         case lxImport:
            generateImport(writer, current);
            break;
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: generateSymbolTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   writer.newNode(lxSymbol);
   //writer.appendNode(lxSourcePath, scope.sourcePath);

   generateAttributes(writer, node.prevNode(), derivationScope/*, true, false, false*/);

   generateExpressionTree(writer, node.findChild(lxExpression), derivationScope);

   writer.closeNode();
}

void DerivationWriter :: generateClassTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool nested)
{
   SyntaxTree buffer((pos_t)0);

   bool closureMode = false;
   if (!nested) {
      writer.newNode(lxClass);
      //writer.appendNode(lxSourcePath, scope.sourcePath);

      generateAttributes(writer, node.prevNode(), derivationScope/*, true, false, false*/);
      if (node.argument == MODE_CLOSURE) {
         // if it is a single method singleton
         writer.appendNode(lxAttribute, V_SINGLETON);

         closureMode = true;
      }
   }

   SNode current = node.firstChild();
   if (closureMode) {
      generateMethodTree(writer, node, derivationScope/*, scope.reference == INVALID_REF*/, true, current.argument == MODE_PROPERTYMETHOD);
   }
   else {
      bool withInPlaceInit = false;
      bool firstParent = true;
      while (current != lxNone) {
         if (current == lxParent) {
            SNode baseNameNode = current.findChild(lxNameAttr);
            if (firstParent) {
               writer.newNode(lxParent);               
               copyIdentifier(writer, baseNameNode.firstChild(lxTerminalMask));
               if (baseNameNode.existChild(lxToken)) {
                  generateTemplateAttributes(writer, baseNameNode, derivationScope);
               }
               writer.closeNode();

               firstParent = false;
            }
            else {
               if (baseNameNode.existChild(lxToken)) {
                  generateClassTemplateTree(writer, current, derivationScope);
               }
               else _scope->raiseError(errInvalidHint, _filePath, current);
            }
         }
         else if (current == lxClassMethod) {
            generateMethodTree(writer, current, derivationScope/*, scope.reference == INVALID_REF*/, false, current.argument == MODE_PROPERTYMETHOD);
         }
         else if (current == lxClassField/* || current == lxFieldInit*/) {
            withInPlaceInit |= generateFieldTree(writer, current, derivationScope, buffer);
         }
         //      else if (current == lxFieldTemplate) {
         //         withInPlaceInit |= generateFieldTemplateTree(writer, current, scope, buffer);
         //      }
         ////      else if (current == lxMessage) {
         ////      }
         //      else scope.raiseError(errInvalidSyntax, node);

         current = current.nextNode();
      }

      if (withInPlaceInit) {
         current = goToNode(buffer.readRoot(), lxFieldInit);
         writer.newNode(lxClassMethod);
         writer.appendNode(lxAttribute, V_INITIALIZER);
         writer.appendNode(lxIdentifier, INIT_MESSAGE);
         writer.newNode(lxCode);
         while (current != lxNone) {
            if (current == lxFieldInit) {
               writer.newNode(lxExpression);
               SyntaxTree::copyNode(writer, current);
               writer.closeNode();
            }
            current = current.nextNode();
         }
         writer.closeNode();
         writer.closeNode();
      }
   }

   if (nested)
      writer.insert(lxNestedClass);

   writer.closeNode();
}

void DerivationWriter :: generateTemplateAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   writer.newBookmark();
   generateExpressionAttribute(writer, node.findChild(lxToken), derivationScope);
   writer.removeBookmark();

   //SNode current = node.firstChild();
   //while (current != lxNone) {
   //   //if (current == lxToken) {
   //   //   generateExpressionAttribute(writer, current, derivationScope);
   //   //}

   //   current = current.nextNode();
   //}

   //SNode identNode = current;
   //if (current == lxToken)
   //   identNode = current.firstChild(lxTerminalMask);

   //bool allowType = templateArgMode || current.nextNode().nextNode() != lxToken;
   //bool allowProperty = false;
   //ref_t attrRef = mapAttribute(current, allowType, allowProperty);
   //LexicalType attrType;
   //if (isPrimitiveRef(attrRef)) {
   //   attrType = lxAttribute;
   //}
   //else {
   //   attrType = lxTarget;
   //   if (derivationScope.isTypeParameter(identNode.identifier(), attrRef)) {
   //      attrType = lxTemplateParam;
   //   }
   //}

   //writer.insert(0, lxEnding, 0);
   //if (attrRef == V_TEMPLATE) {
   //   // copy the template parameters
   //   generateExpressionAttribute(writer, current.findChild(lxToken), derivationScope, true);
   //}

   //if (current == lxToken) {
   //   insertIdentifier(writer, current.firstChild(lxTerminalMask));
   //   if (current.existChild(lxDynamicSizeDecl))
   //      writer.insertChild(0, lxSize, -1);
   //}
   //else insertIdentifier(writer, current);
   //writer.insert(0, attrType, attrRef);
}

void DerivationWriter :: generateAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool rootMode, bool templateMode, bool expressionMode*/)
{
   SNode current = node;

   SNode nameNode;
   if (current == lxNameAttr) {
      nameNode = current;

      current = current.prevNode();
   }

   while (true) {
      if (current == lxAttribute) {
//         bool templateParam = false;
//         ref_t attrRef = expressionMode ? V_ATTRTEMPLATE : mapAttribute(current, scope, templateParam);
//
//         if (templateParam) {
//            writer.appendNode(lxTemplateAttribute, attrRef);
//         }
//         else if (attrRef == V_ATTRTEMPLATE) {
//            generateAttributeTemplate(writer, current, scope, templateMode, expressionMode);
//         }
//         else /*if (isAttribute(attrRef)) */{
            writer.newNode(lxAttribute, current.argument);
            copyIdentifier(writer, current.findChild(lxIdentifier));
            if (current.argument == V_TEMPLATE) {
               generateTemplateAttributes(writer, current, derivationScope);
            }
            writer.closeNode();
            //            if (templateMode) {
//               if (current.argument == V_ACCESSOR) {
//                  // HOTFIX : recognize virtual property template
//                  // add virtual methods
//                  if (scope.fields.Count() == 0) {
//                     scope.fields.add(TEMPLATE_GET_MESSAGE, scope.fields.Count() + 1);
//                  }
//                  else scope.fields.add(TEMPLATE_SET_MESSAGE, scope.fields.Count() + 1);
//
//                  scope.type = DerivationScope::ttMethodTemplate;
//               }
//            }
  //       }
//         else if (attrRef != 0) {
//            writeFullReference(writer, scope.compilerScope->module, attrRef, current);
//         }
//         else scope.raiseError(errInvalidHint, current);
      }
//      else if (current.compare(lxAttributeValue, lxIdle)) {
//
//      }
//      else if (current == lxRefAttribute) {
//         // if it is an attribute with a reference body - treat it like T<> template
//         writer.newNode(lxAttribute, current.argument);
//         SNode attrParam = current.findChild(lxAttributeValue);
//         if (attrParam.existChild(lxAttributeValue)) {
//            attrParam = lxAttribute;
//            generateAttributeTemplate(writer, attrParam, scope, templateMode, expressionMode);
//         }
//         else {
//            current.setArgument(V_TYPETEMPL);
//            generateAttributeTemplate(writer, current, scope, templateMode, expressionMode);
//         }
//         
//         writer.closeNode();
//      }
      else if (current == lxTarget) {
         SNode terminal = current.firstChild(lxTerminalMask);

         LexicalType targetType = lxTarget;
         int targetArgument = current.argument;
         if (derivationScope.withTypeParameters()) {
            // check template parameter if required
            int index = derivationScope.parameters.get(terminal.identifier());
            if (index != 0) {
               targetType = lxTemplateParam;
               targetArgument = index;
            }
         }

         writer.newNode(targetType, targetArgument);
         copyIdentifier(writer, terminal);
         writer.closeNode();

         if (current.existChild(lxDynamicSizeDecl)) {
            writer.appendNode(lxSize, -1);
         }
      }
      else break;

      current = current.prevNode();
   }
   if (nameNode != lxNone) {
      SNode terminal = nameNode.firstChild(lxTerminalMask);

      LexicalType nameType = lxNameAttr;
      ref_t nameArgument = nameNode.argument;
      if (derivationScope.isNameParameter(terminal.identifier(), nameArgument)) {
         nameType = lxTemplateNameParam;
         //      else if (scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate) {
         //         // HOTFIX : in field template the last parameter is a name
         //         int paramIndex = scope.mapParameter(nameNode.firstChild(lxTerminalMask).identifier());
         //         if (paramIndex != 0 && paramIndex == (int)scope.parameters.Count()) {
         //            writer.appendNode(lxTemplateParam, paramIndex);
         //         }
         //         else scope.copyName(writer, nameNode.firstChild(lxTerminalMask));
         //      }
      }

      //      if (rootMode) {
      writer.newNode(nameType, nameArgument);
      copyIdentifier(writer, terminal);
      writer.closeNode();

      if (nameArgument == MODE_COMPLEXMESSAGE) {
         // COMPILER MAGIC : if it is a complex name
         SNode parentNode = node.parentNode().prevNode();

         writer.newNode(lxMessage);
         copyIdentifier(writer, parentNode.firstChild(lxTerminalMask));
         writer.closeNode();
      }
      //      }

//      else scope.copyName(writer, nameNode.firstChild(lxTerminalMask));
   }
}

bool DerivationWriter :: generateFieldTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{   
//   if (node == lxClassField) {
      // COMPILER MAGIC : property declaration
      bool withPropertyTemplate = false;
      SNode current = node.prevNode();
      while (current.compare(lxAttribute, lxNameAttr, lxTarget)) {
         if (current == lxAttribute && current.argument == V_PROPERTY) {
            withPropertyTemplate = true;
         }

         current = current.prevNode();
      }

      if (withPropertyTemplate) {
         // COMPILER MAGIC : inject a property template
         generatePropertyTemplateTree(writer, node, derivationScope);
      }
      else {
         writer.newNode(lxClassField/*, templateMode ? -1 : 0*/);
         SNode sizeNode = node.findChild(lxSizeDecl);
         if (sizeNode != lxNone) {
            writer.newNode(lxSize);
            copyIdentifier(writer, sizeNode.firstChild(lxTerminalMask));
            writer.closeNode();
         }

         //      SNode name = node.prevNode().firstChild(lxTerminalMask);
         //
         //      if (scope.type == DerivationScope::ttFieldTemplate && name.identifier().compare(TEMPLATE_FIELD)) {
         //         scope.fields.add(TEMPLATE_FIELD, scope.fields.Count() + 1);
         //
         //         writer.newNode(lxTemplateField, scope.fields.Count());
         //         copyIdentifier(writer, name);
         //         writer.closeNode();
         //
         //         generateAttributes(writer, node.prevNode().prevNode(), scope, false, templateMode, false);
         //      }
         /*else */generateAttributes(writer, node.prevNode(), derivationScope/*, false, templateMode, false*/);

         writer.closeNode();
      }

//   }
//   else if (node == lxFieldInit && !templateMode) {
//      SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
//
//      writer.newNode(lxFieldInit);
//      ::copyIdentifier(writer, nameNode);
//      writer.closeNode();
//   }

   // copy inplace initialization
   SNode bodyNode = node.findChild(lxFieldInit);
   if (bodyNode != lxNone) {
      SyntaxWriter bufferWriter(buffer);

      SNode nameNode = node.prevNode();

      bufferWriter.newNode(lxFieldInit);
      ::copyIdentifier(bufferWriter, nameNode.firstChild(lxTerminalMask));
      bufferWriter.appendNode(lxAssign);
      generateExpressionTree(bufferWriter, bodyNode.findChild(lxExpression), derivationScope);
      bufferWriter.closeNode();

      return true;
   }
   else return false;
}

void DerivationWriter :: generateMethodTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool closureMode, bool propertyMode)
{
   writer.newNode(lxClassMethod);
   if (derivationScope.templateMode != stNormal) {
      // HOTFIX : save the template source path
      IdentifierString fullPath(_scope->module->Name());
      fullPath.append('\'');
      fullPath.append(_filePath);

      writer.appendNode(lxSourcePath, fullPath.c_str());
      //writer.appendNode(lxTemplate, scope.templateRef);
   }

   if (propertyMode) {
      writer.appendNode(lxAttribute, V_GETACCESSOR);
   }

   if (closureMode) {
      writer.appendNode(lxAttribute, V_ACTION);
   }
   else generateAttributes(writer, node.prevNode(), derivationScope/*, false, templateMode, false*/);

   // copy method arguments
   SNode current = node.firstChild();
//   SNode attribute;
   while (current != lxNone) {
      switch (current) {
//         case lxAttributeValue:
//            if (current.nextNode() == lxMethodParameter) {
//               attribute = current;
//               break;
//            }
//            else attribute = SNode();             
         case lxParameter:
         {
            writer.newNode(lxMethodParameter, current.argument);

            SNode paramNode = current.lastChild();
            recognizeScopeAttributes(paramNode, 0);
            paramNode.refresh();

            generateAttributes(writer, paramNode, derivationScope);
            //            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            //            if (attribute != lxNone) {
            //               // if the type attribute available
            //               generateTypeAttribute(writer, attribute, scope, templateMode);
            //            }

            writer.closeNode();
            //            attribute = SNode();
            break;
         }
         case lxParent:
         {
            // COMPILER MAGIC : if it is a complex name
            writer.newNode(lxMessage);
            SNode identNode = current.findChild(lxToken).firstChild(lxTerminalMask);
            copyIdentifier(writer, identNode);
            writer.closeNode();
            break;
         }
         default:
            // otherwise break the loop
            current = SNode();
            break;
      }

      current = current.nextNode();
   }

//   if (templateMode)
//      scope.reference = INVALID_REF;

   if (propertyMode) {
      writer.newNode(lxReturning);
      generateExpressionTree(writer, node.findChild(lxExpression), derivationScope, 0);
      writer.closeNode();
   }
   else {
      SNode bodyNode = node.findChild(lxCode, lxDispatchCode, lxReturning, lxResendExpression);
      if (bodyNode.compare(lxReturning, lxDispatchCode)) {
         writer.newNode(bodyNode.type);
         generateExpressionTree(writer, bodyNode.firstChild(), derivationScope, EXPRESSION_IMPLICIT_MODE);
         writer.closeNode();
      }
      else if (bodyNode == lxResendExpression) {
         writer.newNode(bodyNode.type);
         generateExpressionTree(writer, bodyNode, derivationScope, EXPRESSION_IMPLICIT_MODE);
         SNode block = bodyNode.nextNode();

         if (block == lxCode)
            generateCodeTree(writer, block, derivationScope);

         writer.closeNode();
      }
      else if (bodyNode == lxCode) {
         generateCodeTree(writer, bodyNode, derivationScope);
      }
   }

   writer.closeNode();
}

void DerivationWriter :: generateCodeTree(SyntaxWriter& writer, SNode node, Scope& derivationScope/*, bool withBookmark*/)
{
   writer.newNode(node.type, node.argument);

//   if (withBookmark)
//      writer.newBookmark();
//
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxExpression:
//            if (checkVariableDeclaration(current, scope)) {
//               generateVariableTree(writer, current, scope);
//            }
//            else if (checkPatternDeclaration(current, scope)) {
//               generateCodeTemplateTree(writer, current, scope);
//            }
////            else if (checkArrayDeclaration(current, scope)) {
////               generateArrayVariableTree(writer, current, scope);
////            }
//            else if (current.existChild(lxAssignOperator)) {
//               generateAssignmentOperator(writer, current, scope);
//            }
            /*else */generateExpressionTree(writer, current, derivationScope);
            break;
         case lxReturning:
//         case lxExtension:
            writer.newNode(current.type, current.argument);
            generateExpressionTree(writer, current, derivationScope, EXPRESSION_IMPLICIT_MODE);
            writer.closeNode();
            break;
         case lxEOF:
         {
            writer.newNode(lxEOF);

            SNode terminal = current.firstChild();
            SyntaxTree::copyNode(writer, lxRow, terminal);
            SyntaxTree::copyNode(writer, lxCol, terminal);
            SyntaxTree::copyNode(writer, lxLength, terminal);

            writer.closeNode();
            break;
         }
//         case lxLoop:
//         case lxCode:
//         case lxExtern:
//            generateCodeTree(writer, current, scope);
//            break;
////         case lxObject:
////            if (isTemplateBracket(current.nextNode())) {
////               generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
////            }
////            else generateObjectTree(writer, current.firstChild(), scope, MODE_ROOT);
////            break;
////         case lxMessageParameter:
//         case lxMessage:
////         case lxIdleMsgParameter:
//            generateMessageTree(writer, current, scope);
//            writer.insert(lxExpression);
//            writer.closeNode();
//            break;
////         case lxOperator:
////            copyOperator(writer, current, scope);
////            generateExpressionTree(writer, current, scope, EXPRESSION_OPERATOR_MODE | EXPRESSION_IMPLICIT_MODE);
////            writer.insert(lxExpression);
////            writer.closeNode();
////            break;
//         default:
//            scope.raiseError(errInvalidSyntax, current);
      }
      current = current.nextNode();
   }

//   if (withBookmark)
//      writer.removeBookmark();

   writer.closeNode();
}

void DerivationWriter :: generateCodeExpression(SyntaxWriter& writer, SNode current, Scope& derivationScope, bool closureMode)
{
   if (closureMode) {
      generateCodeTree(writer, current, derivationScope);
      writer.insert(lxClosureExpr);
      writer.closeNode();
   }
   else {
      writer.newNode(lxExpression);
      generateCodeTree(writer, current, derivationScope);
      writer.closeNode();
   }

//   if (current == lxReturning) {
//      writer.closeNode();
//   }
//   else if (scope.type == DerivationScope::ttCodeTemplate && checkFirstNode(current, lxEOF)) {
//      if (test(scope.mode, daDblBlock)) {
//         if (scope.codeNode == lxNone) {
//            writer.insert(lxTemplateParam);
//            writer.closeNode();
//
//            scope.codeNode = current;
//         }
//         else {
//            writer.insert(lxTemplateParam, 3);
//            writer.closeNode();
//
//            scope.codeNode = SNode();
//         }
//      }
//      else if (test(scope.mode, daBlock)) {
//         writer.insert(lxTemplateParam);
//         writer.closeNode();
//      }
//   }
}

void DerivationWriter :: generateClassTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SNode nameNode = node.firstChild();

   SyntaxTree bufferTree;
   SyntaxWriter bufferWriter(bufferTree);
   bufferWriter.newNode(lxRoot);
   generateTemplateAttributes(bufferWriter, nameNode, derivationScope);
   bufferWriter.closeNode();

   List<SNode> parameters;
   IdentifierString templateName;
   templateName.copy(nameNode.firstChild(lxTerminalMask).identifier());

   SNode current = bufferTree.readRoot().firstChild();
   while (current == lxTarget) {
      parameters.add(current);

      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(parameters.Count());

   ref_t templateRef = _scope->resolveImplicitIdentifier(_ns, templateName.c_str(), false, &_importedNs);
   if (!templateRef)
      _scope->raiseError(errInvalidSyntax, _filePath, node);

   _scope->importClassTemplate(writer, templateRef, parameters);

   node = lxIdle;
}

void DerivationWriter :: generatePropertyTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   List<SNode> parameters;
   IdentifierString templateName;

   SNode nameNode = node.prevNode();   
   SNode current = nameNode.prevNode();
   if (current == lxTarget) {
      parameters.add(current);

      current = current.prevNode();
   }
   while (current == lxAttribute) {
      if (current.argument == V_PROPERTY) {
         templateName.copy(current.firstChild(lxTerminalMask).identifier());
         break;
      }

      current = current.prevNode();
   }

   // COMPILER MAGIC : generate property body
   current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         SNode subNameNode = current.prevNode();

         subNameNode.setArgument(MODE_COMPLEXMESSAGE);

         generateMethodTree(writer, current, derivationScope, false, current.argument == MODE_PROPERTYMETHOD);

         parameters.add(subNameNode);
      }

      current = current.nextNode();
   }

   SNode t = nameNode.firstChild(lxTerminalMask);
   ident_t s = t.identifier();

   // name parameter is always the last parameter
   parameters.add(nameNode);

   templateName.append('#');
   templateName.appendInt(parameters.Count() - 1);

   ref_t templateRef = _scope->attributes.get(templateName.c_str());
   if (!templateRef)
      _scope->raiseError(errInvalidSyntax, _filePath, node.parentNode());

   _scope->generateTemplateProperty(writer, templateRef, parameters);
}

void DerivationWriter :: generateClosureTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope)
{
   writer.insert(lxMethodParameter);
   writer.closeNode();

   node = node.nextNode();
   while (node == lxParameter) {
      writer.newNode(lxMethodParameter);
      writer.newBookmark();

      SNode tokenNode = node.findChild(lxToken);
      generateTokenExpression(writer, tokenNode, derivationScope, false);

      writer.removeBookmark();
      writer.closeNode();;

      node = node.nextNode();
   }

   if (node == lxReturning) {
      writer.newNode(lxReturning);
      generateExpressionTree(writer, node, derivationScope);
      writer.closeNode();
   }
   else if (node == lxClosureExpr) {
      generateCodeTree(writer, node.findChild(lxCode), derivationScope);
   }

   writer.insert(lxClosureExpr);
   writer.closeNode();

   while (node.nextNode() != lxNone)
      node = node.nextNode();
}

void DerivationWriter :: generateCodeTemplateTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope)
{
   IdentifierString templateName;
   templateName.copy(node.firstChild(lxTerminalMask).identifier());

   int exprCounters = 0;
   int blockCounters = 0;
   SNode current = node.nextNode();
   while (current != lxNone) {
      if (current == lxExpression) {
         if (blockCounters == 0) {
            exprCounters++;
         }
         else blockCounters++;
      }
      else if (current == lxCode) {
         blockCounters++;
      }
      else if (current == lxToken) {
         // COMPILER MAGIC : if it is complex code template
         templateName.append(':');
         templateName.append(current.firstChild(lxTerminalMask).identifier());
      }

      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(blockCounters);
   templateName.append('#');
   templateName.appendInt(exprCounters);

   ref_t templateRef = _scope->attributes.get(templateName.c_str());
   if (!templateRef)
      _scope->raiseError(errInvalidSyntax, _filePath, node.parentNode());

   // generate members
   SyntaxTree tempTree;
   SyntaxWriter tempWriter(tempTree);
   current = node;
   while (current != lxNone) {
      if (current == lxCode) {
         generateCodeExpression(tempWriter, current, derivationScope, false);
      }
      else if (current == lxExpression) {
         generateExpressionTree(tempWriter, current, derivationScope, 0);
      }

      current = current.nextNode();
   }

   // load code template parameters
   List<SNode> parameters;
   current = tempTree.readRoot();
   while (current != lxNone) {
      if (current == lxExpression) {
         parameters.add(current);
      }

      current = current.nextNode();
   }

   _scope->generateTemplateCode(writer, templateRef, parameters);

   while (node.nextNode() != lxNone)
      node = node.nextNode();
}

inline bool isTypeExpressionAttribute(SNode current)
{
   return current.nextNode() == lxToken && current.nextNode().nextNode() != lxToken;
}

void DerivationWriter :: generateExpressionAttribute(SyntaxWriter& writer, SNode current, Scope& derivationScope, bool templateArgMode)
{
   SNode identNode = current;
   if (current == lxToken)
      identNode = current.firstChild(lxTerminalMask);

   bool allowType = templateArgMode || current.nextNode().nextNode() != lxToken;
   bool allowProperty = false;
   ref_t attrRef = mapAttribute(current, allowType, allowProperty);
   LexicalType attrType;
   if (isPrimitiveRef(attrRef)) {
      attrType = lxAttribute;
   }
   else {
      attrType = lxTarget;
      if (derivationScope.isTypeParameter(identNode.identifier(), attrRef)) {
         attrType = lxTemplateParam;
      }
   }

   writer.insert(0, lxEnding, 0);
   if (attrRef == V_TEMPLATE) {
      // copy the template parameters
      generateExpressionAttribute(writer, current.findChild(lxToken), derivationScope, true);
   }

   if (current == lxToken) {
      insertIdentifier(writer, current.firstChild(lxTerminalMask));
      if (current.existChild(lxDynamicSizeDecl))
         writer.insertChild(0, lxSize, -1);
   }
   else insertIdentifier(writer, current);
   writer.insert(0, attrType, attrRef);
}

void DerivationWriter :: generateIdentifier(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
   ref_t argument = 0;
   // COMPILER MAGIC : if it is a class template declaration
   if (current.nextNode() == lxToken) {
      writer.newNode(lxTemplate);
      if (current == lxToken) {
         copyIdentifier(writer, current.firstChild(lxTerminalMask));
      }
      else copyIdentifier(writer, current);
      
      SNode argNode = current.nextNode();
      while (argNode == lxToken) {
         writer.newBookmark();
         generateExpressionAttribute(writer, argNode, derivationScope, true);
         writer.removeBookmark();

         argNode = argNode.nextNode();
      }
      
      writer.closeNode();
   }
   else if (derivationScope.templateMode == ScopeType::stCodeTemplate) {
      int paramIndex = derivationScope.parameters.get(current.identifier());
      if (paramIndex != 0) {
         writer.newNode(lxTemplateParam, paramIndex);
         copyIdentifier(writer, current);
         writer.closeNode();
      }
      else copyIdentifier(writer, current);
   }
   else if (derivationScope.isNameParameter(current.identifier(), argument)) {
      writer.newNode(lxTemplateNameParam, argument);
      copyIdentifier(writer, current);
      writer.closeNode();
   }
   else if (derivationScope.isIdentifierParameter(current.identifier(), argument)) {
      writer.newNode(lxTemplateIdentParam, argument);
      copyIdentifier(writer, current);
      writer.closeNode();
   }
   else copyIdentifier(writer, current);
}

void DerivationWriter :: generateMesage(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
   ref_t argument = 0;

   SNode identNode = current.firstChild(lxTerminalMask);
   if (current == lxMessage && derivationScope.isMessageParameter(identNode.identifier(), argument)) {
      writer.newNode(lxTemplateMsgParam, argument);
      copyIdentifier(writer, identNode);
      writer.closeNode();
   }
   else {
      writer.newNode(lxMessage);
      if (current == lxMessage) {
         copyIdentifier(writer, identNode);
      }
      writer.closeNode();

   }
}

void DerivationWriter :: generateTokenExpression(SyntaxWriter& writer, SNode& current, Scope& derivationScope, bool rootMode)
{
   if (current.nextNode() == lxToken) {
      do {
         generateExpressionAttribute(writer, current, derivationScope);
         current = current.nextNode();

      } while (current.nextNode() == lxToken);
   }
   if (rootMode) {
      if (goToNode(current, lxCode/*, lxClosureExpr*/, lxOperator) == lxCode) {
         // COMPILER MAGIC : recognize the code template
         generateCodeTemplateTree(writer, current, derivationScope);
         return;
      }
   }
   if (current == lxToken) {
      generateIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope);
   }
   else generateIdentifier(writer, current, derivationScope);
   if (current.existChild(lxDynamicSizeDecl)) {
      writer.appendNode(lxSize, -1);
   }
}

void DerivationWriter :: generateSwitchTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxSwitchOption:
//         case lxBiggerSwitchOption:
//         case lxLessSwitchOption:
//            if (current.type == lxBiggerSwitchOption) {
//               writer.newNode(lxOption, GREATER_MESSAGE_ID);
//            }
//            else if (current.type == lxLessSwitchOption) {
//               writer.newNode(lxOption, LESS_MESSAGE_ID);
//            }
            /*else */writer.newNode(lxOption, EQUAL_OPERATOR_ID);
            generateIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope);
            generateCodeExpression(writer, current.firstChild(lxCode), derivationScope, false);
            writer.closeNode();
            break;
//         case lxLastSwitchOption:
//            writer.newNode(lxElse);
//            writer.newBookmark();
//            generateObjectTree(writer, current.firstChild(), scope);
//            writer.removeBookmark();
//            writer.closeNode();
//            break;
//         default:
//            scope.raiseError(errInvalidSyntax, current);
//            break;
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: generateCollectionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   writer.newNode(lxCollection);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression) {
         generateExpressionTree(writer, current, derivationScope);
      }
      current = current.nextNode();
   }

   writer.closeNode();
}

void DerivationWriter :: generateExpressionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, int mode)
{
   writer.newBookmark();
   
   //   Stack<int> bookmarks;
   //
   bool first = true;
   //   bool implicitMode = test(mode, EXPRESSION_IMPLICIT_MODE);
   bool expressionExpected = !/*implicitMode*/test(mode, EXPRESSION_IMPLICIT_MODE);
   
   SNode current = node.firstChild();
   //   if (test(mode, EXPRESSION_OPERATOR_MODE))
   //      current = current.nextNode();
   //
   while (current != lxNone) {
      switch (current.type) {
         case lxMessage:
         case lxImplicitMessage:
            if (!first) {
               writer.insert(lxExpression);
               writer.closeNode();
            }
            else first = false;

            generateMesage(writer, current, derivationScope);
            break;
         case lxExpression:
            //first = false;
            //if (test(mode, MODE_MESSAGE_BODY)) {
            //   generateExpressionTree(writer, current, scope);
            //}
            /*else */generateExpressionTree(writer, current, derivationScope, 0/*EXPRESSION_IMPLICIT_MODE*/);
            break;
         case lxOperator:
         case lxAssign:
            if (!first) {
               writer.insert(lxExpression);
               writer.closeNode();
            }
            else first = false;
            writer.newNode(current.type, current.argument);
            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
            break;
         case lxNestedClass:
            recognizeClassMebers(current);            
            generateClassTree(writer, current, derivationScope, true);
            //         writer.insert(lxExpression);
            //         writer.closeNode();
            break;
         case lxCode:
            generateCodeExpression(writer, current, derivationScope, first);
            first = false;            
            break;
         case lxToken:
            generateTokenExpression(writer, current, derivationScope, true);
            break;
         case lxPropertyParam:
            // to indicate the get property call
            writer.appendNode(lxPropertyParam);
            break;
         case lxSwitching:
            generateSwitchTree(writer, current, derivationScope);
            writer.insert(lxSwitching);
            writer.closeNode();
            expressionExpected = true;
            break;
         case lxCollection:
            generateCollectionTree(writer, current, derivationScope);
            break;
         default:
            if (isTerminal(current.type)) {
               generateTokenExpression(writer, current, derivationScope, true);

               if (current.nextNode().compare(lxClosureExpr, lxParameter, lxReturning)) {
                  // COMPILER MAGIC : recognize the closure
                  generateClosureTree(writer, current, derivationScope);
               }
            }
            break;
      }

      current = current.nextNode();
   }

   if (expressionExpected) {
      writer.insert(lxExpression);
      writer.closeNode();
   }

//   if (first && test(mode, EXPRESSION_OBJECT_REQUIRED))
//      scope.raiseError(errInvalidSyntax, node);

   writer.removeBookmark();
}

//bool DerivationTransformer :: generateSingletonScope(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   SNode expr = node.findChild(lxExpression);
//   SNode object = expr.findChild(lxObject);
//   SNode closureNode = object.findChild(lxNestedClass);
//   if (closureNode != lxNone && isSingleStatement(expr)) {
//      recognizeScopeMembers(closureNode, scope, 0);
//
//      SNode terminal = object.firstChild(lxTerminalMask);
//
//      writer.newNode(lxClass);
//      writer.appendNode(lxAttribute, V_SINGLETON);
//
//      if (terminal != lxNone) {
//         ref_t reference = scope.mapReference(terminal);
//         if (!reference)
//            scope.raiseError(errUnknownSubject, terminal);
//
//         writeParentFullReference(writer, scope.compilerScope->module, reference, terminal);
//      }
//
//      generateAttributes(writer, node.prevNode(), scope, true, false, false);
//
//      // NOTE : generateClassTree closes the class node and copies auto generated classes after it
//      generateClassTree(writer, closureNode, scope, -2);
//
//      return true;
//   }
//   else return false;
//}

void DerivationWriter:: declareType(SyntaxWriter& writer, SNode node/*, DerivationScope& scope*/)
{
////   bool classMode = false;
////   bool templateMode = false;
////   SNode expr = node.findChild(lxExpression);
////   SNode paramNode;
////   if (expr == lxExpression) {
////      expr = expr.findChild(lxObject);
////      SNode nextNode = expr.nextNode();
////      if (nextNode == lxOperator) {
////         classMode = true;
////
////         SNode operatorNode;
////         if (checkTypeTemplateBrackets(nextNode, operatorNode)) {
////            // make tree transformation
////            //current.set(lxAttribute, -1);
////            operatorNode = lxAttribute;
////            paramNode = expr;
////
////            setTypeTemplateAttributes(nextNode);
////
////            templateMode = true;
////         }
////         else return false;
////      }
////      else if (expr != lxObject || nextNode != lxNone)
////         return false;
////   }

   SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
   SNode referenceNode;

   SNode current = node.firstChild();
   bool invalid = true;
   if (nameNode == lxIdentifier && isSingleStatement(current)) {
      referenceNode = current.firstChild(lxTerminalMask);

      invalid = !referenceNode.compare(lxIdentifier, lxReference, lxGlobalReference);
   }

   if (invalid)
      _scope->raiseError(errInvalidSyntax, _filePath, current);

   writer.newNode(lxForward);
   copyIdentifier(writer, nameNode);
   writer.newNode(lxAttribute, V_TYPE);
   copyIdentifier(writer, referenceNode);
   writer.closeNode();
   writer.closeNode();

   // HOTFIX : to recognize it in declarations
   _types.add(nameNode.identifier(), referenceNode.identifier().clone());

////   bool internalSubject = nameNode == lxPrivate;
////   bool invalid = true;
//
////   SNode classNode = expr.findChild(lxIdentifier, lxPrivate, lxReference);
////   if (classNode != lxNone) {
////      if (templateMode) {
////         DerivationScope templateScope(&scope, 0);
////         templateScope.templateRef = templateScope.mapClassTemplate(expr);
////         loadAttributeValues(paramNode, templateScope, NULL, classMode);
////
////         if (generateTemplate(writer, templateScope, true)) {
////            classRef = templateScope.reference;
////
////            invalid = false;
////         }
////         else scope.raiseError(errInvalidSyntax, node);
////      }
////      else {
////         classRef = scope.moduleScope->mapTerminal(classNode);
////
////         invalid = false;
////      }
////   }
////
////   if (!invalid) {
////      if (!scope.moduleScope->saveAttribute(typeName, classRef, internalSubject))
////         scope.raiseError(errDuplicatedDefinition, nameNode);
////   }
////
////   return !invalid;
}

void DerivationWriter :: generateImport(SyntaxWriter& writer, SNode ns)
{
   SNode nameNode = ns.prevNode().firstChild(lxTerminalMask);
   if (nameNode != lxNone) {
      ident_t name = nameNode.identifier();

      if (name.compare(STANDARD_MODULE))
         // system module is included automatically - nothing to do in this case
         return;

      writer.newNode(lxImport, name);
      copyIdentifier(writer, nameNode);
      writer.closeNode();

      _importedNs.add(name.clone());
   }
}

//////inline SNode findLastAttribute(SNode current)
//////{
//////   SNode lastAttribute;
//////   while (current == lxAttribute || current == lxIdleAttribute) {
//////      lastAttribute = current;
//////      current = current.nextNode();
//////      while (current.compare(lxAttributeValue, lxIdle, lxClassRefAttr))
//////         current = current.nextNode();
//////   }
//////
//////   return lastAttribute;
//////}
//
//inline bool setIdentifier(SNode node)
//{
//   SNode current = node.prevNode();
//   if (current == lxAttribute) {
//      current = lxNameAttr;
//
//      return true;
//   }
//
//   return false;
//}
//
//inline bool isAttribute(ref_t attr)
//{
//   return (int)attr < 0;
//}
//
//inline int readSizeValue(SNode node, int radix)
//{
//   ident_t val = node.identifier();
//
//   return val.toLong(radix);
//}
//
////inline bool isTemplateDeclaration(SNode node)
////{
////   SNode current = node.findChild(lxOperator);
////   if (current == lxOperator) {
////      SNode angleOperator = current.findChild(lxObject).findChild(lxAngleOperator);
////
////      return (angleOperator != lxNone && angleOperator.existChild(lxAssigning));
////   }
////   return false;
////}
//
//inline void copyAutogeneratedClass(SyntaxTree& sourceTree, SyntaxTree& destionationTree)
//{
//   SyntaxWriter writer(destionationTree);
//
//   SyntaxTree::moveNodes(writer, sourceTree, lxClass);
//}
//
////inline bool isArrayDeclaration(SNode node)
////{
////   SNode current = node.findChild(lxAttributeValue);
////   if (current == lxAttributeValue && current.argument == -1 && current.nextNode() == lxSize) {
////      return true;
////   }
////
////   return false;
////}
//
////inline bool verifyNode(SNode node, LexicalType type1, LexicalType type2)
////{
////   return node == type1 || node == type2;
////}
//
//inline bool isClosingOperator(SNode current)
//{
//   return current.firstChild().firstChild().identifier().compare(">");
//}
//
//inline SNode findTemplateEnd(SNode current)
//{
//   int level = 0;
//   while (current == lxOperator) {
//      if (isClosingOperator(current)) {
//         level--;
//         if (level == 0)
//            return current;
//      }
//      else level++;
//
//      current = current.nextNode();
//      while (current == lxObject)
//         current = current.nextNode();
//   }
//
//   return SNode();
//}

// --- TemplateGenerator::TemplateScope ---

//ref_t DerivationTransformer::DerivationScope :: mapNewIdentifier(ident_t name, bool privateOne)
//{
//   return compilerScope->mapNewIdentifier(ns.c_str(), name, privateOne);
//}
//
//ref_t DerivationTransformer::DerivationScope :: mapAttribute(SNode attribute, int& paramIndex)
//{
//   if (attribute.argument)
//      return attribute.argument;
//
//   SNode terminal = attribute.firstChild(lxTerminalMask);
//   if (terminal == lxNone || attribute == lxIdentifier)
//      terminal = attribute;
//
//   if (terminal == lxExplicitAttr) {
//      ident_t value = terminal.identifier();
//
//      int attrRef = value.toInt(1);
//      return -attrRef;
//   }
//   else {
//      ident_t token = terminal.identifier();
//      //      if (emptystr(token))
//      //         token = terminal.findChild(lxTerminal).identifier();
//
//      paramIndex = mapParameter(token);
//      if (paramIndex != 0) {
//         return V_PARAMETER;
//      }
//      else return compilerScope->attributes.get(token);
//   }
//}
//
//ref_t DerivationTransformer::DerivationScope :: mapIdentifier(ident_t identifier, bool referenceMode)
//{
//   ref_t reference = compilerScope->resolveImplicitIdentifier(ns, identifier, referenceMode, imports);
//   if (referenceMode && !reference) {
//      return compilerScope->mapFullReference(identifier, true);
//   }
//   else return reference;
//}
//
//ref_t DerivationTransformer::DerivationScope :: mapReference(SNode terminal)
//{
//   if (terminal == lxIdentifier) {
//      // try try resolve as an identifier
//      ref_t ref = mapIdentifier(terminal.identifier(), false);
//      if (!ref) {
//         // otherwise try to resolve it as an attribute
//         ref = mapAttribute(terminal);
//      }
//      return ref;
//   }
//   else if (terminal == lxReference) {
//      ref_t reference = compilerScope->resolveImplicitIdentifier(ns, terminal.identifier(), true, imports);
//      if (!reference)
//         reference = compilerScope->mapFullReference(terminal.identifier(), true);
//
//      return reference;
//   }
//   else if (terminal == lxGlobalReference) {
//      return compilerScope->mapFullReference(terminal.identifier() + 1, true);
//   }
//   else if (terminal == lxClassRefAttr) {
//      return compilerScope->mapFullReference(terminal.identifier(), true);
//   }
//
//   return 0;
//}
//
//void DerivationTransformer::DerivationScope :: loadParameters(SNode node)
//{
//   SNode current = node.firstChild();
//   // load template parameters
//   while (current != lxNone) {
//      if (current == lxAttributeValue) {
//         if (current.existChild(lxAttributeValue))
//            raiseError(errInvalidSyntax, node);
//
//         ident_t name = current.firstChild(lxTerminalMask).identifier();
//
//         parameters.add(name, parameters.Count() + 1);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void DerivationTransformer::DerivationScope :: loadFields(SNode node)
//{
//   SNode current = node;
//   // load template parameters
//   while (current != lxNone) {
//      if (current == lxBaseParent) {
//         SNode ident = current.firstChild(lxTerminalMask);
//         ident_t name = ident.identifier();
//
//         fields.add(name, fields.Count() + 1);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//int DerivationTransformer::DerivationScope :: mapParameter(ident_t identifier)
//{
//   int index = parameters.get(identifier);
//   if (!index) {
//      if (parent != NULL) {
//         return parent->mapParameter(identifier);
//      }
//      else return 0;
//   }
//   else return index;
//}
//
//void DerivationTransformer::DerivationScope :: copyName(SyntaxWriter& writer, SNode terminal)
//{
//   int index = mapParameter(terminal.identifier());
//   if (index) {
//      writer.newNode(lxTemplateParam, index);
//      copyIdentifier(writer, terminal);
//      writer.closeNode();
//   }
//   else if (type == DerivationScope::ttMethodTemplate) {
//      ident_t identifier = terminal.identifier();
//      if (emptystr(identifier))
//         identifier = terminal.identifier();
//
//      index = fields.get(identifier);
//      if (index != 0) {
//         writer.newNode(lxTemplateMethod, parameters.Count() + index);
//         copyIdentifier(writer, terminal);
//         writer.closeNode();
//      }
//      else copyIdentifier(writer, terminal);
//   }
//   else ::copyIdentifier(writer, terminal);
//}
//
////void DerivationTransformer::DerivationScope :: copyMessageName(SyntaxWriter& writer, SNode terminal)
////{
////   copyIdentifier(writer, terminal);
////}

bool TemplateGenerator::TemplateScope :: generateClassName()
{
   ident_t templateName = moduleScope->module->resolveReference(templateRef);
   NamespaceName rootNs(templateName);
   IdentifierString name;
   if (isWeakReference(templateName)) {
      name.copy(moduleScope->module->Name());
      name.append(templateName);

      rootNs.copy(moduleScope->module->Name());
   }
   else name.copy(templateName);

   auto it = parameterValues.start();
   while (!it.Eof()) {
      name.append('&');

      ident_t param = moduleScope->module->resolveReference((*it).argument);
      if (NamespaceName::compare(param, rootNs)) {
         name.append(param + getlength(rootNs) + 1);
      }
      else if (isWeakReference(param) && !isTemplateWeakReference(param)) {
         if (!moduleScope->module->Name().compare(rootNs.c_str())) {
            name.append(moduleScope->module->Name());
            name.append(param);
         }
         else name.append(param + 1);
      }
      else name.append(param);

      it++;
   }
   name.replaceAll('\'', '@', 0);

   bool alreadyDeclared = false;
   reference = moduleScope->mapTemplateClass(ns, name, alreadyDeclared);

   return !alreadyDeclared;
}

_Memory* TemplateGenerator::TemplateScope :: loadTemplateTree()
{
   ref_t ref = 0;
   templateModule = moduleScope->loadReferenceModule(moduleScope->module->resolveReference(templateRef), ref);

   return templateModule ? templateModule->mapSection(ref | mskSyntaxTreeRef, true) : NULL;
}

////ref_t DerivationReader::DerivationScope :: mapTypeTemplate(SNode current)
////{
////   SNode attrNode = current.findChild(lxAttributeValue).firstChild(lxTerminalObjMask);
////   ref_t attrRef = mapTerminal(attrNode, true);
////   if (attrRef == 0)
////      attrRef = mapTerminal(attrNode);
////
////   return attrRef;
////}

// --- TemplateGenerator ---

TemplateGenerator :: TemplateGenerator(SyntaxTree& tree)
{
   _root = tree.readRoot();

//   ByteCodeCompiler::loadVerbs(_verbs);
}

//void DerivationTransformer :: loadParameterValues(SNode attributes, DerivationScope& scope, SubjectMap* parentParameterValues/*, bool classMode*/)
//{
//   SNode current = attributes;
//   // load template parameters
//   while (current != lxNone) {
//      if (current == lxAttributeValue/* || current == lxIdleAttribute*/) {
//         ref_t classRef = 0;
//
//         SNode terminalNode = current.findChild(lxClassRefAttr);
//         if (terminalNode == lxNone) {
//            if (current.existChild(lxAttributeValue)) {
//               // if it is a template-based declaration 
//               classRef = V_TYPETEMPL;
//            }
//            else {
//               terminalNode = current.firstChild(lxTerminalMask);
//
//               classRef = scope.mapReference(terminalNode);
//            }
//         }
//         else classRef = scope.compilerScope->mapFullReference(terminalNode.identifier(), true);
//
////         if (current == lxIdleAttribute) {
//            
////         }
////         else terminalNode = current.firstChild(lxObjectMask);
////
//         if (!classRef)
//            scope.raiseError(errInvalidHint, current);
//
//         if (isPrimitiveRef(classRef)) {
//            if (classRef == V_TYPETEMPL) {
//               bool arrayMode = false;
//               int paramIndex = 0;
//
//               classRef = mapNewTemplate(current, scope, false, arrayMode, paramIndex, scope.reference == INVALID_REF, NULL);
//               if (paramIndex != 0) {
//                  classRef = -1 - paramIndex;
//               }
//               else if (!classRef || arrayMode) {
//                  scope.raiseError(errInvalidHint, current);
//               }
//            }
//            else if (classRef == V_PARAMETER) {
//               classRef = -1 - scope.mapParameter(terminalNode.identifier());
//            }
//            else scope.raiseError(errInvalidHint, current);
//         }
//            
////            if (classMode) {
////               // if it is not a declared type - check if it is a class
////               attr = scope.mapTerminal(terminalNode, true);
////
////               if (attr == 0)
////                  //HOTFIX : declare a new type
////                  attr = scope.mapTerminal(terminalNode, false);
////            }
////
////            if (!attr)
////               scope.raiseError(errInvalidHint, current);
////         }
////         else if (isPrimitiveRef(attr)) {
////            if (attr == V_TYPETEMPL) {
////               // HOTFIX : recognize type template
////               attr = scope.mapTypeTemplate(current);
////            }
////         }
//
//         scope.parameterValues.add(scope.parameterValues.Count() + 1, classRef);
//      }
////      //else if (/*current == lxExpression || */current == lxObject) {
////      //   //SNode item = current == lxObject ? current : current.findChild(lxObject);
////      //   SNode terminal = current.findChild(lxIdentifier, lxPrivate);
////      //   if (terminal != lxNone && terminal.nextNode() == lxNone) {
////      //      ref_t attr = 0;
////      //      if (classMode) {
////      //         attr = mapTypeTerminal(terminal, true);
////      //      }
////      //      //else attr = mapAttribute(item);
////      //      if (attr == 0) {
////      //         //HOTFIX : support newly declared classes
////      //         attr = mapTerminal(terminal, false);
////
////      //         moduleScope->validateReference(terminal, attr);
////      //      }
////
////      //      this->attributes.add(this->attributes.Count() + 1, attr);
////      //   }
////      //   else raiseError(errInvalidHint, current);
////      //}
////      else if (current == lxClassRefAttr) {
////         ref_t attr = attr = scope.mapTerminal(current, true);
////         if (attr == 0) {
////            scope.raiseError(errInvalidHint, current);
////         }
////
////         scope.attributes.add(scope.attributes.Count() + 1, attr);
////      }
//      else if (current == lxNameAttr && scope.type == DerivationScope::ttFieldTemplate) {
//         scope.parameterValues.add(scope.parameterValues.Count() + 1, INVALID_REF);
//
//         scope.identNode = current;
//
//         break;
//      }
//      else if (current == lxTemplateAttribute) {
//         ref_t subject = parentParameterValues->get(current.argument);
//
//         scope.parameterValues.add(scope.parameterValues.Count() + 1, subject);
//      }
//      else if (current == lxTemplateParam && current.argument == INVALID_REF) {
//         ref_t templateRef = generateNewTemplate(current, scope, parentParameterValues);
//
//         scope.parameterValues.add(scope.parameterValues.Count() + 1, templateRef);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void DerivationTransformer :: copyClassTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      copyTreeNode(writer, current, scope);
//
//      current = current.nextNode();
//   }
//}

void TemplateGenerator :: copyExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   if (node.strArgument != -1) {
      writer.newNode(node.type, node.identifier());
   }
   else writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

//inline void writeFullReference(SyntaxWriter& writer, _Module* module, ref_t reference)
//{
//   ident_t referenceName = module->resolveReference(reference);
//   if (isWeakReference(referenceName) && !isTemplateWeakReference(referenceName)) {
//      IdentifierString fullName(module->Name(), referenceName);
//
//      writer.appendNode(lxClassRefAttr, fullName.c_str());
//
//   }
//   else writer.appendNode(lxClassRefAttr, referenceName);
//}
//
//inline void writeFullReference(SyntaxWriter& writer, _Module* module, ref_t reference, SNode node)
//{
//   ident_t referenceName = module->resolveReference(reference);
//   if (isWeakReference(referenceName) && !isTemplateWeakReference(referenceName)) {
//      IdentifierString fullName(module->Name(), referenceName);
//
//      writer.newNode(lxClassRefAttr, fullName.c_str());
//   }
//   else if (!emptystr(referenceName)) {
//      writer.newNode(lxClassRefAttr, referenceName);
//   }
//   else throw InternalError(errCrUnknownReference);
//
//   if (test(node.type, lxTerminalMask)) {
//      copyIdentifier(writer, node);
//   }
//   else copyIdentifier(writer, node.firstChild(lxTerminalMask));
//
//   writer.closeNode();
//}
//
//inline void writeParentFullReference(SyntaxWriter& writer, _Module* module, ref_t reference, SNode node)
//{
//   ident_t referenceName = module->resolveReference(reference);
//   if (isWeakReference(referenceName) && !isTemplateWeakReference(referenceName)) {
//      IdentifierString fullName(module->Name(), referenceName);
//
//      writer.newNode(lxBaseParent, fullName.c_str());
//   }
//   else writer.newNode(lxBaseParent, referenceName);
//
//   if (test(node.type, lxTerminalMask)) {
//      copyIdentifier(writer, node);
//   }
//   else copyIdentifier(writer, node.firstChild(lxTerminalMask));
//
//   writer.closeNode();
//}
//
//void DerivationTransformer :: copyParamAttribute(SyntaxWriter& writer, SNode current, DerivationScope& scope)
//{
//   ref_t classRef = scope.parameterValues.get(current.argument);
//   if ((scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate) && classRef == INVALID_REF) {
//      copyIdentifier(writer, scope.identNode.firstChild(lxTerminalMask));
//   }
//   else if ((int)classRef < -1) {
//      writer.newNode(lxTemplateAttribute, -((int)classRef + 1));
//      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//      writer.closeNode();
//   }
//   else writeFullReference(writer, scope.compilerScope->module, classRef, current);
//}
//
////void DerivationTransformer :: copyIdentifier(SyntaxWriter& writer, SNode terminal)
////{
////   ::copyIdentifier(writer, terminal);
////}

void TemplateGenerator :: copyTreeNode(SyntaxWriter& writer, SNode current, TemplateScope& scope)
{
   if (test(current.type, lxTerminalMask | lxObjectMask)) {
      copyIdentifier(writer, current);
   }
//   else if (current == lxTemplate) {
//      writer.appendNode(lxTemplate, scope.templateRef);
//   }
   else if (current == lxTarget && current.argument != 0) {
      if (scope.moduleScope->module != scope.templateModule)
         current.setArgument(importReference(scope.templateModule, current.argument, scope.moduleScope->module));

      copyExpressionTree(writer, current, scope);
   }
   else if (current == lxTemplateParam) {
      if (scope.type == TemplateScope::ttCodeTemplate) {
         SNode nodeToInject = scope.parameterValues.get(current.argument);
         if (nodeToInject == lxCode) {
            writer.newNode(lxExpression);
            copyExpressionTree(writer, nodeToInject, scope);
            writer.closeNode();
         }
         else if (nodeToInject == lxExpression) {
            copyExpressionTree(writer, nodeToInject, scope);
         }
      }
      else if (scope.type == TemplateScope::ttPropertyTemplate || scope.type == TemplateScope::ttClassTemplate) {
         SNode nodeToInject = scope.parameterValues.get(current.argument);
         copyExpressionTree(writer, nodeToInject, scope);
      }
      else throw InternalError("Not yet supported");

//      if (scope.type == DerivationScope::ttCodeTemplate) {
//         if (current.argument == 1) {
//            // if it is a code template parameter
//            DerivationScope* parentScope = scope.parent;
//         
//            generateExpressionTree(writer, scope.exprNode, *parentScope);
//         }
//         else if (current.argument == 0) {
//            // if it is a code template parameter
//            DerivationScope* parentScope = scope.parent;
//         
//            generateCodeTree(writer, scope.codeNode, *parentScope);
//         }
//         else if (current.argument == 3) {
//            DerivationScope* parentScope = scope.parent;
//         
//            // if it is an else code template parameter
//            SNode subParam = current.findSubNode(lxTemplateParam);
//            if (subParam == lxTemplateParam && subParam.argument == 0) {
//               // HOTFIX : insert if-else code
//               generateCodeTree(writer, scope.codeNode, *parentScope);
//            }
//         
//            generateCodeTree(writer, scope.elseNode, *parentScope);
//         }
//         else if (current.argument == 2) {
//            // if it is a code template parameter
//            DerivationScope* parentScope = scope.parent;
//         
//            writer.newBookmark();
//            generateObjectTree(writer, scope.nestedNode, *parentScope);
//            writer.removeBookmark();
//         }
//      }
//      else if (current.argument == INVALID_REF) {
//         ref_t templateRef = generateNewTemplate(current, scope, &scope.parameterValues);
//
//         writeFullReference(writer, scope.compilerScope->module, templateRef, current);
//      }
//      else {
//         // if it is a template parameter
//         ref_t attrRef = scope.parameterValues.get(current.argument);
//         if (attrRef == INVALID_REF && (scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate)) {
//            copyIdentifier(writer, scope.identNode.firstChild(lxTerminalMask));
//         }
//         //else if ((int)attrRef < -1) {
//         //   copyParamAttribute(writer, current, scope);
//         //}
//         else writeFullReference(writer, scope.compilerScope->module, attrRef, current);
//      }
   }
   else if (current == lxTemplateNameParam) {
      // name node is always the last parameter
      SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());

      copyChildren(writer, nodeToInject, scope);
   }
   else if (current == lxTemplateIdentParam) {
      // name node is always the last parameter
      SNode nodeToInject = scope.parameterValues.get(current.argument);

      copyChildren(writer, nodeToInject, scope);
   }
   else if (current == lxTemplateMsgParam) {
      // name node is always the last parameter
      SNode nodeToInject = scope.parameterValues.get(current.argument);

      writer.newNode(lxMessage);
      copyChildren(writer, nodeToInject, scope);
      writer.closeNode();
      if (nodeToInject.argument == MODE_COMPLEXMESSAGE) {
         // COMPILER MAGIC : if it is a complex name
         SNode parentNode = nodeToInject.parentNode().prevNode();

         writer.newNode(lxMessage);
         copyIdentifier(writer, parentNode.firstChild(lxTerminalMask));
         writer.closeNode();
      }
   }
   //   else if (current == lxTemplateField && current.argument >= 0) {
//      ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();
//
//      writer.newNode(lxIdentifier, fieldName);
//
//      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//      writer.closeNode();
//   }
//   else if (current == lxTemplateMethod && current.argument >= 0) {
//      ident_t methodName = retrieveIt(scope.fields.start(), current.argument - scope.parameterValues.Count()).key();
//
//      writer.newNode(lxIdentifier, methodName);
//
//      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//      writer.closeNode();
//   }
//   else if (current == lxTemplateBoxing) {
//      SNode bodyNode = current.findChild(lxReturning, lxNestedClass);
//
//      if (bodyNode == lxNestedClass) {
//         writer.newBookmark();
//      }
//      else writer.newNode(lxBoxing);
//
//      SNode attrNode = current.findChild(lxAttribute);
//      if (attrNode == lxNone) {
//         ref_t attrRef = scope.compilerScope->mapFullReference(current.findChild(lxClassRefAttr).identifier(), true);
//
//         DerivationScope templateScope(&scope, attrRef);
//         loadParameterValues(current.firstChild(), templateScope, &scope.parameterValues/*, true*/);
//
//         SyntaxTree buffer;
//         SyntaxWriter bufferWriter(buffer);
//         generateTemplate(bufferWriter, templateScope, true);
//         copyAutogeneratedClass(buffer, *scope.autogeneratedTree);
//
//         writeFullReference(writer, scope.compilerScope->module, templateScope.reference, current);
//      }
//      else if (attrNode.argument == V_TYPETEMPL) {
//         SNode attr = current.findChild(lxTemplateAttribute);
//         if (attr != lxNone) {
//            copyParamAttribute(writer, current.findChild(lxTemplateAttribute), scope);
//         }
//         else copyExpressionTree(writer, current.findChild(lxAttributeValue).findChild(lxClassRefAttr), scope);
//      }
//      else if (attrNode.argument == V_OBJARRAY && current.existChild(lxSize)) {
//         SNode attr = current.findChild(lxTemplateAttribute);
//         if (attr != lxNone) {
//            copyParamAttribute(writer, current.findChild(lxTemplateAttribute), scope);
//         }
//         else copyExpressionTree(writer, current.findChild(lxAttributeValue).findChild(lxClassRefAttr), scope);
//
//         writer.appendNode(lxOperator, -1);
//         //         SNode attrNode = current.findChild(lxTemplateAttribute, lxClassRefAttr);
//         //         if (attrNode == lxTemplateAttribute) {
//         //            copyParamAttribute(writer, attrNode, scope);
//         //            writer.appendNode(lxOperator, -1);
//         //         }
//         //         else if (attrNode == lxClassRefAttr) {
//         //            writer.newNode(lxClassRefAttr, attrNode.identifier());
//         //            writer.closeNode();
//         //            writer.appendNode(lxOperator, -1);
//         //         }
//      }
//      else scope.raiseError(errInvalidHint, current);
//
//      if (bodyNode == lxNestedClass) {
//         copyClassTree(writer, bodyNode, scope);
//         writer.insert(lxNestedClass);
//         writer.closeNode();
//
//         writer.removeBookmark();
//      }
//      else {
//         //generateExpressionTree(writer, bodyNode, scope, 0);
//         //copyTemplateInitBody(writer, bodyNode, scope);
//         if (bodyNode != lxNone)
//            copyClassTree(writer, bodyNode, scope);
//
//         writer.closeNode();
//      }
//   }
//   else if (current == lxTemplateAttribute) {
//      copyParamAttribute(writer, current, scope);
//   }
//   else if (current == lxAttributeValue) {
//      copyExpressionTree(writer, current.findChild(lxClassRefAttr), scope);
//   }
////   else if (current == lxTemplateParamAttr) {
////      copyParamAttribute(writer, current, scope);
////   }
   else copyExpressionTree(writer, current, scope);
}

//void DerivationTransformer :: copyTemplateInitBody(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   //writer.newBookmark();
//
//   //Stack<int> bookmarks;
//
//   //bool first = true;
//   //bool implicitMode = test(mode, EXPRESSION_IMPLICIT_MODE);
//   //bool expressionExpected = !implicitMode;
//
//   SNode current = node.firstChild();
//   //if (test(mode, EXPRESSION_OPERATOR_MODE))
//   //   current = current.nextNode();
//
//   while (current != lxNone) {
//      switch (current.type) {
//   //   case lxObject:
//   //      if (!first) {
//   //         if (expressionExpected) {
//   //            insertBookmarks(writer, bookmarks);
//   //            writer.insert(lxExpression);
//   //            writer.closeNode();
//   //         }
//   //         writer.removeBookmark();
//   //         writer.newBookmark();
//   //      }
//   //      else first = false;
//
//   //      expressionExpected = !implicitMode;
//   //      if (isTemplateBracket(current.nextNode())) {
//   //         generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
//   //      }
//   //      else generateObjectTree(writer, current.firstChild(), scope, MODE_OBJECTEXPR);
//   //      break;
//   //   case lxCatchOperation:
//   //   case lxAltOperation:
//   //      writer.newBookmark();
//   //   case lxIdleMsgParameter:
//   //   case lxMessageParameter:
//   //   case lxMessage:
//   //      //insertBookmarks(writer, bookmarks);
//   //      expressionExpected = false;
//   //      generateMessageTree(writer, current, scope);
//   //      writer.insert(lxExpression);
//   //      writer.closeNode();
//   //      if (current == lxCatchOperation) {
//   //         writer.removeBookmark();
//   //         writer.insert(lxTrying);
//   //         writer.closeNode();
//   //         expressionExpected = true;
//   //      }
//   //      else if (current == lxAltOperation) {
//   //         writer.removeBookmark();
//   //         writer.insert(lxAlt);
//   //         writer.closeNode();
//   //         expressionExpected = true;
//   //      }
//   //      break;
//   //   case lxArrOperator:
//   //      expressionExpected = false;
//   //      copyOperator(writer, current);
//   //      if (isTemplateBracket(current.nextNode())) {
//   //         generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
//   //      }
//   //      else generateExpressionTree(writer, current, scope, EXPRESSION_OPERATOR_MODE | EXPRESSION_IMPLICIT_MODE | EXPRESSION_OBJECT_REQUIRED);
//   //      writer.insert(lxExpression);
//   //      writer.closeNode();
//   //      break;
//   //   case lxOperator:
//   //   {
//   //      expressionExpected = true;
//
//   //      // HOTFIX : arranging the operator precedence
//   //      int level = defineOperatorLevel(current);
//   //      int last_level = bookmarks.peek() & 7;
//   //      while (last_level && last_level <= level) {
//   //         bookmarks.pop();
//   //         writer.removeBookmark();
//
//   //         if (bookmarks.Count() != 0) {
//   //            writer.insert(bookmarks.peek() >> 3, lxExpression, 0);
//   //         }
//   //         else writer.insert(lxExpression);
//   //         writer.closeNode();
//
//   //         last_level = bookmarks.peek() & 7;
//   //      }
//
//   //      copyOperator(writer, current);
//   //      int bm = writer.newBookmark();
//   //      bookmarks.push((bm << 3) + level);
//
//   //      if (isTemplateBracket(current.nextNode())) {
//   //         generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
//   //      }
//   //      else generateExpressionTree(writer, current, scope, EXPRESSION_OPERATOR_MODE | EXPRESSION_IMPLICIT_MODE | EXPRESSION_OBJECT_REQUIRED);
//
//   //      break;
//   //   }
//      case lxExpression:
//         copyTreeNode(writer, current, scope);
//   //      first = false;
//   //      if (test(mode, MODE_MESSAGE_BODY)) {
//   //         generateExpressionTree(writer, current, scope);
//   //      }
//   //      else generateExpressionTree(writer, current, scope, EXPRESSION_IMPLICIT_MODE);
//         break;
//   //   case lxAssigning:
//   //      insertBookmarks(writer, bookmarks);
//   //      if (expressionExpected) {
//   //         writer.insert(lxExpression);
//   //         writer.closeNode();
//   //      }
//
//   //      writer.appendNode(lxAssign);
//   //      generateExpressionTree(writer, current, scope);
//   //      expressionExpected = true;
//   //      break;
//   //   case lxCode:
//   //      first = false;
//   //      generateCodeExpression(writer, current, scope);
//   //      break;
//   //   case lxExtension:
//   //      writer.newNode(current.type, current.argument);
//   //      generateExpressionTree(writer, current, scope, EXPRESSION_IMPLICIT_MODE);
//   //      writer.closeNode();
//   //      break;
//   //   case lxIdle:
//   //      break;
//      default:
//         scope.raiseError(errInvalidSyntax, current);
//         break;
//      }
//
//      current = current.nextNode();
//   }
//
//   //insertBookmarks(writer, bookmarks);
//
//   //if (expressionExpected) {
//   //   writer.insert(lxExpression);
//   //   writer.closeNode();
//   //}
//
//   //if (first && test(mode, EXPRESSION_OBJECT_REQUIRED))
//   //   scope.raiseError(errInvalidSyntax, node);
//
//   //writer.removeBookmark();
//}

void TemplateGenerator :: copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
//      if (current == lxIdentifier || current == lxPrivate || current == lxReference) {
//         copyIdentifier(writer, current);
//      }
      /*else */if (current == lxNameAttr) {
         copyTreeNode(writer, current, scope);
      }
      else if (current == lxTemplateNameParam) {
         // name node is always the last parameter
         SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());

         copyTreeNode(writer, nodeToInject, scope);
      }
      else if(current == lxTemplateParam) {
         SNode nodeToInject = scope.parameterValues.get(current.argument);

         // NOTE : target should not be imported / exported
         copyExpressionTree(writer, nodeToInject, scope);
      }
      //      else if (current == lxTemplateParam && current.argument == INVALID_REF) {
//         if (current.argument == INVALID_REF) {
//            ref_t templateRef = generateNewTemplate(current, scope, &scope.parameterValues);
//
//            writeFullReference(writer, scope.compilerScope->module, templateRef, current);
//         }
//         else {
//            // if it is a template parameter
//            ref_t attrRef = scope.parameterValues.get(current.argument);
//            if (attrRef == INVALID_REF && (scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate)) {
//               copyIdentifier(writer, scope.identNode.firstChild(lxTerminalMask));
//            }
//            //else if ((int)attrRef < -1) {
//            //   copyParamAttribute(writer, current, scope);
//            //}
//            else writeFullReference(writer, scope.compilerScope->module, attrRef, current);
//         }
//      }
//      else if (current == lxTemplateField && current.argument >= 0) {
//         ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();
//
//         writer.newNode(lxIdentifier, fieldName);
//
//         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//         writer.closeNode();
//      }
//      else if (current == lxTemplateAttribute) {
//         copyParamAttribute(writer, current, scope);
//      }
//      else if (current == lxClassRefAttr) {
//         writer.appendNode(current.type, current.identifier());
//      }
      else if (current == lxSize) {
         writer.appendNode(current.type, current.argument);
      }
      else if (current == lxAttribute)
         writer.appendNode(current.type, current.argument);

      current = current.nextNode();
   }

   writer.closeNode();
}

//void DerivationTransformer :: copyFieldInitTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   writer.newNode(node.type, node.argument);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxMemberIdentifier) {
//         copyIdentifier(writer, current);
//      }
//      else copyExpressionTree(writer, current, scope);
//
//      current = current.nextNode();
//   }
//
//   writer.closeNode();
//}

void TemplateGenerator :: copyNodes(SyntaxWriter& writer, SNode current, TemplateScope& scope)
{
   while (current != lxNone) {
      copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }
}

void TemplateGenerator :: copyChildren(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }
}

void TemplateGenerator :: copyMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplateNameParam) {
         // name node is always the last parameter
         SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());
         copyExpressionTree(writer, nodeToInject, scope);
      }
      else copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

//void DerivationTransformer :: copyTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributeValues, SubjectMap* parentAttributes, int mode)
//{
//   loadParameterValues(attributeValues, scope, parentAttributes/*, true*/);
//
//   if (generateTemplate(writer, scope, false, mode)) {
//      //if (/*variableMode && */scope.reference != 0)
//      //   writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(scope.reference));
//   }
//   else scope.raiseError(errInvalidHint, node);
//}
//
//bool DerivationTransformer :: compareAttributes(SNode node, DerivationScope& scope)
//{
//   size_t index = 1;
//
//   SNode current = node;
//   // validare template parameters
//   while (current != lxNone) {
//      if (current == lxAttributeValue/* || current == lxIdleAttribute*/) {
//         SNode attrNode;
//      //   if (current == lxIdleAttribute) {
//      //      attrNode = current.findChild(lxAttributeValue);
//      //   }
//         /*else */attrNode = current;
//
//      //   if (attrNode.existChild(lxAttributeValue)) {
//      //      return false;
//      //   }
//      //   else {
//            int paramIndex = 0;
//            ref_t attrRef = scope.mapAttribute(attrNode.findChild(lxIdentifier), paramIndex);
//            if (attrRef == V_PARAMETER) {
//               if ((size_t)paramIndex != index) {
//                  return false;
//               }
//               else index++;
//            }
//      //      else return false;
//      //   }
//      }
//      //else if (current == lxClassRefAttr) {
//      //   if (current.existChild(lxTemplateAttribute)) {
//      //      SNode attr = current.firstChild();
//      //      while (attr != lxNone) {
//      //         if (attr == lxTemplateAttribute) {
//      //            if ((int)attr.argument < 0) {
//      //               if (index == -((int)attr.argument + 1)) {
//      //                  index++;
//      //               }
//      //               else return false;
//      //            }
//      //            else return false;
//      //         }
//      //         attr = attr.nextNode();
//      //      }
//      //   }
//      //   else return false;
//      //}
//
//      current = current.nextNode();
//   }
//
//   return (index - 1) == scope.parameters.Count();
//}
//
//void DerivationTransformer :: copyTemplateAttributeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   SNode current = node;
//   // validare template parameters
//   while (current != lxNone) {
//      if (current == lxAttributeValue/* || current == lxIdleAttribute*/) {
//         if (current.existChild(lxAttributeValue)) {
//            generateAttributeTemplate(writer, current, scope, true, false);
//         }
//         else {
//            int paramIndex = 0;
//            ref_t attrRef = scope.mapAttribute(current, paramIndex);
//            if (attrRef == V_PARAMETER) {
//               writer.appendNode(lxTemplateAttribute, paramIndex);
//            }
//            else {
//               writer.newNode(lxAttributeValue);
//
//               SNode terminalNode = current.firstChild(lxTerminalMask);
//
//               ref_t classRef = scope.mapReference(terminalNode);
//               if (classRef && !isPrimitiveRef(classRef)) {
//                  writeFullReference(writer, scope.compilerScope->module, classRef, terminalNode);
//               }
//               else scope.raiseError(errInvalidHint, terminalNode);
//
//               writer.closeNode();
//            }
//         }
//
//        //ref_t classRef = scope.mapReference(terminalNode);
//         //if (!classRef)
//         //   scope.raiseError(errInvalidHint, current);
//
////         SNode attrNode;
////         if (current == lxIdleAttribute) {
////            attrNode = current.findChild(lxAttributeValue);
////         }
////         else attrNode = current;
////
////         if (attrNode.existChild(lxAttributeValue)) {
////            generateAttributeTemplate(writer, attrNode, scope, true);
////         }
////         else {
////            int paramIndex = 0;
////            ref_t attrRef = scope.mapAttribute(attrNode.findChild(lxPrivate, lxIdentifier), paramIndex);
////            if (attrRef == INVALID_REF) {
////               
////            }
////            else if (attrRef != 0) {
////               writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(attrRef));
////               copyIdentifier(writer, attrNode.firstChild(lxTerminalMask));
////               writer.closeNode();
////            }
////         }
//      }
//      else if (current == lxClassRefAttr) {
//         if (current.existChild(lxTemplateAttribute)) {
//            writer.newNode(lxTemplateParam, INVALID_REF);
//            writer.appendNode(lxClassRefAttr, current.identifier());
//            SNode attr = current.firstChild();
//            while (attr != lxNone) {
//               if (attr == lxTemplateAttribute) {
//                  if ((int)attr.argument < 0) {
//                     writer.appendNode(lxTemplateAttribute, -((int)attr.argument + 1));
//                  }
//                  else writeFullReference(writer, scope.compilerScope->module, attr.argument);
//               }
//               attr = attr.nextNode();
//            }
//            writer.closeNode();
//         }
//         else {
//            writer.newNode(lxClassRefAttr, current.identifier());
//            copyIdentifier(writer, current.firstChild(lxTerminalMask));
//            writer.closeNode();
//         }
//      }
//
//      current = current.nextNode();
//   }
//}

bool TemplateGenerator :: generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass/*, int mode*/)
{
   _Memory* body = scope.loadTemplateTree();
   if (body == NULL)
      return false;

   SyntaxTree templateTree(body);
   SNode root = templateTree.readRoot();

   if (declaringClass) {
      // HOTFIX : exiting if the class was already declared in this module
      if (!scope.generateClassName())
         return true;

      ident_t fullName = scope.moduleScope->resolveFullName(scope.reference);

      writer.newNode(lxClass, -1);
      writer.newNode(lxNameAttr, scope.moduleScope->mapFullReference(fullName, true));
//      writer.appendNode(lxReference, fullName);
      writer.closeNode();
   }

//   //SyntaxTree buffer;

   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         //if (current.argument == V_TEMPLATE/* && scope.type != TemplateScope::ttAttrTemplate*/) {
         //   // ignore template attributes
         //}
         //else if (current.argument == V_FIELD/* && scope.type != TemplateScope::ttAttrTemplate*/) {
         //   // ignore template attributes
         //}
         //else if (current.argument == V_ACCESSOR) {
         //   if (scope.type == DerivationScope::ttFieldTemplate) {
         //      // HOTFIX : is it is a method template, consider the field name as a message subject
         //      scope.type = DerivationScope::ttMethodTemplate;
         //   }
         //}
         if (scope.type == TemplateScope::ttPropertyTemplate) {
            // do not copy the property attributes
         }
         else /*if (!test(mode, MODE_IMPORTING))*/ {
            // do not copy the class attributes in the import mode 
            writer.newNode(current.type, current.argument);
            SyntaxTree::copyNode(writer, current);
            writer.closeNode();
         }
      }
//      else if (current == lxTemplateParent && !test(mode, MODE_IMPORTING)) {
//         // HOTFIX : class based template
//         writer.newNode(lxBaseParent, -1);
//         copyClassTree(writer, current.findChild(lxTypeAttr), scope);
//         writer.closeNode();
//      }
      else if (current == lxParent && declaringClass) {
         // generate a parent node only for the template based class; it should be ignored for the template class import
         writer.newNode(lxParent, -1);
         copyChildren(writer, current, scope);
         writer.closeNode();
      }
      else if (current == lxClassMethod) {
         copyMethodTree(writer, current, scope);
      }
      else if (current == lxClassField) {
         copyFieldTree(writer, current, scope);
      }
//      else if (current == lxFieldInit) {
//         writer.newNode(lxFieldInit);
//         copyIdentifier(writer, current.findChild(lxMemberIdentifier));
//         writer.closeNode();
//
//         SyntaxWriter initWriter(*scope.autogeneratedTree);
//         copyFieldInitTree(initWriter, current, scope);
//      }
      else if (current == lxExpression) {
         if (current.nextNode() == lxExpression) {
            // HOTFIX : if the code template contains several expressions
            writer.newNode(lxCodeExpression);
            copyNodes(writer, current, scope);
            writer.closeNode();
            break;
         }
         else copyExpressionTree(writer, current, scope);
      }
      current = current.nextNode();
   }

   if (declaringClass) {
      writer.closeNode();
   }

   return true;
}

void TemplateGenerator :: importClass(SyntaxWriter& output, SNode classNode)
{
   SNode current = classNode.firstChild();
   while (current != lxNone) {
      if (current.compare(lxClassMethod, lxClassField)) {
         output.newNode(current.type, current.argument);
         output.appendNode(lxAutogenerated);
         SyntaxTree::copyNode(output, current);
         output.closeNode();
      }      

      current = current.nextNode();
   }
}

//void DerivationTransformer :: generateClosureTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   SNode current = node.firstChild();
//
//   // COMPILER MAGIC : advanced closure syntax
//   writer.newBookmark();
//
//   generateObjectTree(writer, current, scope);
//
//   writer.removeBookmark();
//}
//
//void DerivationTransformer :: generateMessage(SyntaxWriter& writer, SNode current, DerivationScope& scope/*, bool templateMode*/)
//{
//   writer.newNode(lxMessage);
//   /*SNode attrNode = current.findChild(lxAttributeValue, lxSize);
//   if (attrNode != lxAttributeValue) {
//      scope.copySubject(writer, current.firstChild(lxTerminalMask));
//      if (attrNode == lxSize)
//         writer.appendNode(lxSize, -1);
//   }
//   else generateAttributeTemplate(writer, current, scope, templateMode);*/
//   scope.copyName(writer, current.firstChild(lxTerminalMask));
//
//   writer.closeNode();
//}
//
//void DerivationTransformer :: generateTypeAttribute(SyntaxWriter& writer, SNode current, DerivationScope& scope, bool templateMode)
//{
//////   SNode attr = current.findChild(lxAttributeValue, lxSize);
//////   if (attr == lxAttributeValue) {
//////      writer.newNode(lxMessage);
//////      generateAttributeTemplate(writer, current, scope, templateMode, MODE_SIGNATURE);
//////      writer.closeNode();
//////   }
//////   else {
////      // if it is an explicit type declaration
////      bool arrayMode = attr == lxSize;
//      bool paramMode = false;
//
//      writer.newNode(lxTypeAttr);
//      ref_t attrRef = mapAttribute(current, scope/*, arrayMode*/, paramMode);
//      if (paramMode) {
//         writer.appendNode(lxTemplateAttribute, attrRef);
//      }
//      else if (attrRef == V_ATTRTEMPLATE) {
//         generateAttributeTemplate(writer, current, scope, templateMode, false);
//      }
//      else if (isAttribute(attrRef)) {
//         scope.raiseError(errInvalidHint, current);
//      }
//      else {
//         ref_t classRef = attrRef;
//         if (!classRef)
//            // HOTFIX : try to resolve the reference type directly
//            classRef = scope.mapReference(current.firstChild(lxTerminalMask));
//
//         if (classRef) {
//            writeFullReference(writer, scope.compilerScope->module, classRef, current);
//         }
//         else scope.raiseError(errInvalidHint, current);
//      }
//
////      if (paramMode) {
////         writer.newNode(lxTemplateAttribute, ref);
////      }
////      else {
////         if (!ref) {
////            ref = scope.mapIdentifier(typeNode.identifier());
////         }
////         else scope.raiseError(errInvalidHint, typeNode);
////
////         if (ref) {
////            //scope.mapAttributeType(current/*, arrayMode, paramMode*/);
////            //      if (paramMode) {
////            //         writer.newNode(lxTemplateParamAttr, ref);
////            //      }
////            //
////            //      if (arrayMode) {
////            //         writer.appendNode(lxSize, -1);
////            //      }
////         }
////         else scope.raiseError(errInvalidHint, typeNode);
////      }
////
//      writer.closeNode();
//////   }
//}
//
//inline bool checkFirstNode(SNode node, LexicalType type)
//{
//   SNode current = node.firstChild();
//   return (current == type);
//}
//
//inline bool isTemplateBracket(SNode current)
//{
//   if (current == lxOperator) {
//      /*if (current.existChild(lxSize)) {
//         SNode sizeNode = current.findChild(lxSize);
//         if (sizeNode.argument == -1)
//            return true;
//      }
//      else*/ if (current.firstChild().firstChild().identifier().compare("<")) {
//         SNode closingNode = findTemplateEnd(current);
//         return closingNode != lxNone;
//      }
//   }
//
//   return false;
//}
//
//void DerivationTransformer :: autoGenerateExtensions(DerivationScope& templateScope)
//{
//   SubjectList* list = templateScope.compilerScope->getAutogerenatedExtensions(templateScope.templateRef);
//   if (list) {
//      for (auto it = list->start(); !it.Eof(); it++) {
//         DerivationScope scope(templateScope.compilerScope, templateScope.sourcePath, NULL, NULL);
//         scope.templateRef = *it;
//         scope.autogeneratedTree = templateScope.autogeneratedTree;
//
//         // HOTFIX : to copy only required number of parameters
//         ident_t templateName = templateScope.compilerScope->module->resolveReference(*it);
//         int paramCounter = templateName.toInt(templateName.find('#') + 1);
//
//         // copy parameters
//         for (auto param_it = templateScope.parameterValues.start(); !param_it.Eof(); param_it++) {
//            if (paramCounter > 0) {
//               scope.parameterValues.add(param_it.key(), *param_it);
//               paramCounter--;
//            }
//            else break;
//         }
//
//         generateNewTemplate(scope);
//      }
//   }
//}
//
//ref_t DerivationTransformer :: generateNewTemplate(SNode current, DerivationScope& scope, SubjectMap* parentAttributes)
//{
//   ident_t name = current.findChild(lxClassRefAttr).identifier();
//
//   ref_t attrRef = scope.compilerScope->mapFullReference(name, true);
//   if (!attrRef)
//      scope.raiseError(errUnknownSubject, current);
//
//   DerivationScope templateScope(&scope, attrRef);
//   loadParameterValues(current.firstChild(), templateScope, parentAttributes/*, true*/);
//
//   SyntaxTree buffer;
//   SyntaxWriter bufferWriter(buffer);
//   generateTemplate(bufferWriter, templateScope, true);
//   copyAutogeneratedClass(buffer, *scope.autogeneratedTree);
//
//   return templateScope.reference;
//}

ref_t TemplateGenerator :: generateTemplate(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters, bool importMode)
{
//   SyntaxTree autogeneratedTree;

   TemplateScope templateScope(&scope, reference, NULL, NULL/*, NULL*/);
//   templateScope.autogeneratedTree = &autogeneratedTree;
   templateScope.sourcePath = "compiling template...";

   for (auto it = parameters.start(); !it.Eof(); it++) {
      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   // NOTE : for the import mode, no need to declare a new class
   generateTemplate(writer, templateScope, !importMode);

//   SyntaxTree::moveNodes(writer, autogeneratedTree, lxClass);

   return templateScope.reference;
}

void TemplateGenerator :: generateTemplateCode(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL/*, NULL*/);
   templateScope.type = TemplateScope::Type::ttCodeTemplate;

   for (auto it = parameters.start(); !it.Eof(); it++) {
      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   generateTemplate(writer, templateScope, false);
}

void TemplateGenerator :: generateTemplateProperty(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL/*, NULL*/);
   templateScope.type = TemplateScope::Type::ttPropertyTemplate;

   for (auto it = parameters.start(); !it.Eof(); it++) {
      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   generateTemplate(writer, templateScope, false);
}
