//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Derivation Tree class implementation
//
//                                              (C)2005-2019, by Alexei Rakov
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
      case nsL4Operand:
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
      case nsL0Operator:
         _cacheWriter.newNode(lxOperator);
         break;
      case nsAssignmentOperand:
         // HOTFIX : to indicate an assignment operation
         _cacheWriter.newNode(lxOperator, -1);
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
   
   SyntaxTree templateTree;
   SyntaxWriter templateWriter(templateTree);

   generateScope(templateWriter, _cache.readRoot(), templateScope);

   // check for duplicate declaration
   if (_scope->module->mapSection(nameNode.argument | mskSyntaxTreeRef, true))
      _scope->raiseError(errDuplicatedSymbol, _filePath, node);
   
   _Memory* target = _scope->module->mapSection(nameNode.argument | mskSyntaxTreeRef, false);

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
   if (scopeNode.existChild(lxCode, lxReturning)) {
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

         if (derivationScope.templateMode != stNormal) {
            // HOTFIX : save the template source path
            IdentifierString fullPath(_scope->module->Name());
            fullPath.append('\'');
            fullPath.append(_filePath);

            writer.appendNode(lxSourcePath, fullPath.c_str());
            //writer.appendNode(lxTemplate, scope.templateRef);
         }

         writer.appendNode(lxAttribute, V_INITIALIZER);
         writer.newNode(lxNameAttr);
         writer.appendNode(lxIdentifier, INIT_MESSAGE);
         writer.closeNode();
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
      }
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
   // COMPILER MAGIC : property declaration
   bool withPropertyTemplate = false;
   bool withInitializer = false;
   SNode current = node.prevNode();
   while (current.compare(lxAttribute, lxNameAttr, lxTarget)) {
      if (current == lxAttribute && current.argument == V_PROPERTY) {
         withPropertyTemplate = true;
      }
      else if (current == lxAttribute && current.argument == V_MEMBER) {
         withInitializer = true;
      }

      current = current.prevNode();
   }

   if (withPropertyTemplate) {
      // COMPILER MAGIC : inject a property template
      generatePropertyTemplateTree(writer, node, derivationScope);
   }
   else if (withInitializer) {
      SNode nameNode = node.prevNode().firstChild(lxTerminalMask);

      writer.newNode(lxFieldInit);
      ::copyIdentifier(writer, nameNode);
      writer.closeNode();
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
            generateExpressionTree(writer, current, derivationScope);
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
   bool allowType = templateArgMode || current.nextNode().nextNode() != lxToken;
   bool allowProperty = false;
   

   SNode identNode = current;
   if (current == lxToken) {
      identNode = current.firstChild(lxTerminalMask);
   }
      
   LexicalType attrType = lxNone;
   ref_t attrRef = mapAttribute(current, allowType, allowProperty);
   if (allowType) {
      if (attrRef == V_TEMPLATE) {
         // NOTE : template type declaration has a highest priority
         attrType = lxAttribute;
      }
      else if (derivationScope.isTypeParameter(identNode.identifier(), attrRef)) {
         // NOTE : check if it is a template parameter
         attrType = lxTemplateParam;
      }
      else attrType = isPrimitiveRef(attrRef) ? lxAttribute : lxTarget;
   }
   else if (isPrimitiveRef(attrRef))
      attrType = lxAttribute;

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
         case lxLastSwitchOption:
            writer.newNode(lxElse);
            writer.newBookmark();
            generateCodeExpression(writer, current.firstChild(lxCode), derivationScope, false);
            writer.removeBookmark();
            writer.closeNode();
            break;
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
            first = false;
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

void DerivationWriter:: declareType(SyntaxWriter& writer, SNode node/*, DerivationScope& scope*/)
{
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

// --- TemplateGenerator::TemplateScope ---

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

// --- TemplateGenerator ---

TemplateGenerator :: TemplateGenerator(SyntaxTree& tree)
{
   _root = tree.readRoot();
}

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
   else copyExpressionTree(writer, current, scope);
}

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
      else if (current == lxSize) {
         writer.appendNode(current.type, current.argument);
      }
      else if (current == lxAttribute) {
         copyTreeNode(writer, current, scope);
      }

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
