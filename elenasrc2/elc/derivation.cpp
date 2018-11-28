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

#define MODE_ROOT            0x01
//#define MODE_CODETEMPLATE    0x02
//#define MODE_OBJECTEXPR      0x04
////#define MODE_SIGNATURE       0x08
//#define MODE_IMPORTING       0x10
//#define MODE_MESSAGE_BODY    0x20  // indicates that sub-expressions should be an expression themselves

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
   _writer.newNode(lxRoot);
}

void DerivationWriter :: end()
{
   _writer.closeNode();
}

void DerivationWriter :: newNamespace(ident_t ns, ident_t filePath)
{
   _ns = ns;
   _filePath = filePath;

   _writer.newNode(lxNamespace, ns);
   _writer.appendNode(lxSourcePath, filePath);
}

void DerivationWriter :: closeNamespace()
{
   _writer.closeNode();

   _ns = nullptr;
   _filePath = nullptr;
}

void DerivationWriter :: newNode(Symbol symbol)
{
   _level++;

   switch (symbol) {
////      case nsToken:
////         _writer.newNode(lxAttribute);
////         break;
      case nsExpression:
//      case nsNestedRootExpression:
         _cacheWriter.newNode(lxExpression);
         break;
      case nsCodeEnd:
         _cacheWriter.newNode(lxEOF);
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
////      case nsAssigning:
////         _writer.newNode(lxAssigning);
////         break;
////      case nsResendExpression:
////         _writer.newNode(lxResendExpression);
////         break;
////      case nsObject:
////         _writer.newNode(lxObject);
////         break;
//////      case nsAngleOperator:
//////         _writer.newNode(lxAngleOperator);
//////         break;
////      case nsBaseClass:
////         _writer.newNode(lxBaseParent);
////         break;
////      case nsL1Operation:
////      case nsL2Operation:
//////      case nsL3Operation:
////      case nsL4Operation:
////      case nsL5Operation:
////         _writer.newNode(lxOperator);
////         break;
////      case nsL8Operation:
////         _writer.newNode(lxAssignOperator);
////         break;
////      case nsArrayOperation:
////         _writer.newNode(lxArrOperator, REFER_MESSAGE_ID);
////         break;
////      case nsXInlineClosure:
////         _writer.newNode(lxInlineClosure);
////         break;
////      case nsMessageOperation:
////         _writer.newNode(lxMessage);
////         break;
////      case nsSubjectArg:
////         _writer.newNode(lxMessage/*, -1*/);
////         break;
////      case nsLazyExpression:
////         _writer.newNode(lxLazyExpression);
////         break;
////      case nsRetStatement:
////         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask | lxExprMask));
////         break;
////      case nsMessageReference:
////         _writer.newNode(lxMessageReference);
////         break;
//////      case nsDynamicSize:
//////         _writer.newNode(lxSize, -1);
//////         break;
////      case nsSwitching:
////         _writer.newNode(lxSwitching);
////         break;
////      case nsSubCode:
////      case nsTokenParam:
////      case nsDispatchExpression:
////      case nsExtension:
////      case nsCatchMessageOperation:
////      case nsAltMessageOperation:
////      case nsSwitchOption:
////      case nsLastSwitchOption:
//////      case nsBiggerSwitchOption:
//////      case nsLessSwitchOption:
////         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask));
////         break;
////      case nsAttribute:
////         _writer.newNode(lxAttributeDecl);
////         break;
////      case nsNestedSubCode:
////         _writer.newNode(lxCode);
////         break;
////      case nsIdleMessageParameter:
////         _writer.newNode(lxIdleMsgParameter);
////         break;
////      case nsReferenceExpression:
////         _writer.newNode(lxReferenceExpr);
////         break;
////      case nsClosingOperator:
////         _writer.newNode((LexicalType)nsL3Operator);
////         break;
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

      saveScope();

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
//   // HOT FIX : if there are several constants e.g. $10$13, it should be treated like literal terminal
//   if (terminal == tsCharacter && terminal.value.findSubStr(1, '$', terminal.length, NOTFOUND_POS) != NOTFOUND_POS) {
//      terminal.symbol = tsLiteral;
//   }

   LexicalType type = (LexicalType)(terminal.symbol & ~mskAnySymbolMask | lxTerminalMask | lxObjectMask);

//   if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
//      // try to use local storage if the quote is not too big
//      if (getlength(terminal.value) < 0x100) {
//         QuoteTemplate<IdentifierString> quote(terminal.value);
//
//         _writer.newNode(type, quote.ident());
//      }
//      else {
//         QuoteTemplate<DynamicString<char> > quote(terminal.value);
//
//         _writer.newNode(type, quote.ident());
//      }
//   }
   /*else*/ _cacheWriter.newNode(type, terminal.value);

   _cacheWriter.appendNode(lxCol, terminal.col);
   _cacheWriter.appendNode(lxRow, terminal.row);
   _cacheWriter.appendNode(lxLength, terminal.length);
   //   _writer->writeDWord(terminal.disp);

   _cacheWriter.closeNode();
}

void DerivationWriter :: saveScope()
{
   recognizeScope();

   generateScope(_cache.readRoot());

   _cache.clear();
   _cacheWriter.clear();
}

void DerivationWriter :: recognizeScope()
{
   SNode scopeNode = _cache.readRoot().lastChild();
   if (scopeNode == lxScope) {
      recognizeScopeAttributes(scopeNode.prevNode(), MODE_ROOT);

      SNode bodyNode = scopeNode.firstChild();
      if (bodyNode == lxExpression) {
         scopeNode = lxSymbol;
      }
      else {
         scopeNode = lxClass;

         recognizeClassMebers(scopeNode);
      }
   }
   else if (scopeNode == lxAttributeDecl) {
      declareAttribute(scopeNode);
   }
}

ref_t DerivationWriter :: mapAttribute(SNode terminal/*, bool& templateParam*/)
{
   ident_t token = terminal.identifier();
   //      //      if (emptystr(token))
   //      //         token = terminal.findChild(lxTerminal).identifier();
   
   return _scope->attributes.get(token);
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
   while (current == lxToken/*, lxRefAttribute*/) {
   //      if (current == lxAttribute) {
   //         bool templateParam = false;
      ref_t attrRef = mapAttribute(current.firstChild(lxTerminalMask)/*, templateParam*/);
      if (attrRef) {
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
      else _scope->raiseWarning(WARNING_LEVEL_2, wrnUnknownHint, _filePath, current);
   //
   //         if (templateParam) {
   //            // ignore template attributes
   //         }
   //      }
      current = current.prevNode();
   }
   
   SNode nameTerminal = nameNode.firstChild(lxTerminalMask);
   //   if (nameTerminal != lxIdentifier)
   //      scope.raiseError(errInvalidSyntax, nameNode);
   
   ident_t name = nameTerminal.identifier();
   
   //   // verify if there is an attribute with the same name
   //   if (scope.compilerScope->attributes.exist(name))
   //      scope.raiseWarning(WARNING_LEVEL_2, wrnAmbiguousIdentifier, nameNode);
   
   if (test(mode, MODE_ROOT))
      nameNode.setArgument(_scope->mapNewIdentifier(_ns.c_str(), name, privateOne));
}

void DerivationWriter :: recognizeClassMebers(SNode node/*, DerivationScope& scope*/)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxScope) {
         recognizeScopeAttributes(current.prevNode(), 0);

         SNode bodyNode = current.findChild(lxCode, lxDispatchCode);
         if (bodyNode != lxNone) {
            current = lxClassMethod;
         }
         else _scope->raiseError(errInvalidSyntax, _filePath, current);
      }
      current = current.nextNode();
   }
}

void DerivationWriter :: generateScope(SNode node)
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
               generateSymbolTree(/*writer, */current/*, rootScope*/);
//            }            
            break;
         }
         case lxClass:
         {
            generateClassTree(current);
            break;
         }
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: generateSymbolTree(SNode node/*, DerivationScope& scope*/)
{
   _writer.newNode(lxSymbol);
   ////writer.appendNode(lxSourcePath, scope.sourcePath);

   generateAttributes(node.prevNode()/*, scope, true, false, false*/);

   generateExpressionTree(node.findChild(lxExpression)/*, scope*/);

   _writer.closeNode();
}

void DerivationWriter :: generateClassTree(SNode node/*, DerivationScope& scope, int nested*/)
{
//   SyntaxTree buffer((pos_t)0);
//
//   bool closureMode = false;
//   if (!nested) {
      _writer.newNode(lxClass);
//      //writer.appendNode(lxSourcePath, scope.sourcePath);
//
      generateAttributes(node.prevNode()/*, scope, true, false, false*/);
//      if (node.argument == -2) {
//         // if it is a single method singleton
//         writer.appendNode(lxAttribute, V_SINGLETON);
//
//         closureMode = true;
//      }
//   }
//
   SNode current = node.firstChild();
//   bool withInPlaceInit = false;
//   bool firstParent = true;
   while (current != lxNone) {
//      if (current == lxAttribute || current == lxNameAttr) {
//      }
//      else if (current == lxBaseParent) {
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
//
//         firstParent = false;
//      }
      /*else */if (current == lxClassMethod) {
         generateMethodTree(/*writer, */current/*, scope, scope.reference == INVALID_REF, closureMode*/);
      }
//      else if (current == lxClassField || current == lxFieldInit) {
//         withInPlaceInit |= generateFieldTree(writer, current, scope, buffer, false);
//      }
//      else if (current == lxFieldTemplate) {
//         withInPlaceInit |= generateFieldTemplateTree(writer, current, scope, buffer);
//      }
////      else if (current == lxMessage) {
////      }
//      else scope.raiseError(errInvalidSyntax, node);

      current = current.nextNode();
   }

//   if (withInPlaceInit) {
//      current = goToNode(buffer.readRoot(), lxFieldInit);
//      writer.newNode(lxClassMethod);
//      writer.appendNode(lxAttribute, V_INITIALIZER);
//      writer.appendNode(lxIdentifier, INIT_MESSAGE);
//      writer.newNode(lxCode);
//      while (current != lxNone) {
//         if (current == lxFieldInit) {
//            writer.newNode(lxExpression);
//            SyntaxTree::copyNode(writer, current);
//            writer.closeNode();
//         }
//         current = current.nextNode();
//      }
//      writer.closeNode();
//      writer.closeNode();
//   }
//
//   if (nested == -1)
//      writer.insert(lxNestedClass);

   _writer.closeNode();
}

void DerivationWriter :: generateAttributes(/*SyntaxWriter& writer, */SNode node/*, DerivationScope& scope, bool rootMode, bool templateMode, bool expressionMode*/)
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
//         else if (isAttribute(attrRef)) {
            _writer.newNode(lxAttribute, current.argument);
            copyIdentifier(_writer, current.findChild(lxIdentifier));
            _writer.closeNode();
//
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
//         }
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
      else break;

      current = current.prevNode();
   }
   if (nameNode != lxNone) {
//      if (rootMode) {
         _writer.newNode(lxNameAttr, nameNode.argument);
         copyIdentifier(_writer, nameNode.firstChild(lxTerminalMask));
         _writer.closeNode();
//      }
//      else if (scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate) {
//         // HOTFIX : in field template the last parameter is a name
//         int paramIndex = scope.mapParameter(nameNode.firstChild(lxTerminalMask).identifier());
//         if (paramIndex != 0 && paramIndex == (int)scope.parameters.Count()) {
//            writer.appendNode(lxTemplateParam, paramIndex);
//         }
//         else scope.copyName(writer, nameNode.firstChild(lxTerminalMask));
//      }
//      else scope.copyName(writer, nameNode.firstChild(lxTerminalMask));
   }
}

void DerivationWriter :: generateMethodTree(SNode node/*, DerivationScope& scope, bool templateMode, bool closureMode*/)
{
   _writer.newNode(lxClassMethod);
//   if (templateMode) {
//      writer.appendNode(lxSourcePath, scope.sourcePath);
//      writer.appendNode(lxTemplate, scope.templateRef);
//   }
//
//   if (closureMode)
//      writer.appendNode(lxAttribute, V_ACTION);

   generateAttributes(node.prevNode()/*, scope, false, templateMode, false*/);

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
            _writer.newNode(lxMethodParameter, current.argument);

            SNode paramNode = current.lastChild();
            paramNode = lxNameAttr;
            generateAttributes(paramNode);
            //            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            //            if (attribute != lxNone) {
            //               // if the type attribute available
            //               generateTypeAttribute(writer, attribute, scope, templateMode);
            //            }

            _writer.closeNode();
            //            attribute = SNode();
            break;
         }
//         case lxMessage:
//            writer.newNode(lxMessage);
//            copyIdentifier(writer, current.firstChild(lxTerminalMask));
//            writer.closeNode();
//            attribute = SNode();
//            break;
//         default:
//            // otherwise break the loop
//            current = SNode();
//            break;
      }

      current = current.nextNode();
   }

//   if (templateMode)
//      scope.reference = INVALID_REF;

   SNode bodyNode = node.findChild(lxCode, lxDispatchCode/*, lxReturning, lxResendExpression*/);
   if (bodyNode/*.compare(lxReturning, */ == lxDispatchCode/*)*/) {
      _writer.newNode(bodyNode.type);
      generateExpressionTree(bodyNode.firstChild()/*, scope*/, EXPRESSION_IMPLICIT_MODE);
      _writer.closeNode();
   }
//   else if (bodyNode == lxResendExpression) {
//      generateCodeTree(writer, bodyNode, scope, true);
//   }
   else if (bodyNode == lxCode) {
      generateCodeTree(bodyNode/*, scope*/);
   }

   _writer.closeNode();
}

void DerivationWriter :: generateCodeTree(SNode node/*, DerivationScope& scope, bool withBookmark*/)
{
   _writer.newNode(node.type, node.argument);

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
            /*else */generateExpressionTree(current/*, scope*/);
            break;
//         case lxReturning:
////         case lxExtension:
//            writer.newNode(current.type, current.argument);
//            generateExpressionTree(writer, current, scope, EXPRESSION_IMPLICIT_MODE);
//            writer.closeNode();
//            break;
         case lxEOF:
         {
            _writer.newNode(lxEOF);

            SNode terminal = current.firstChild();
            SyntaxTree::copyNode(_writer, lxRow, terminal);
            SyntaxTree::copyNode(_writer, lxCol, terminal);
            SyntaxTree::copyNode(_writer, lxLength, terminal);

            _writer.closeNode();
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

   _writer.closeNode();
}

void DerivationWriter :: generateExpressionAttribute(SNode current)
{
   ref_t attrRef = 0;

   if (current == lxToken) {
      attrRef = mapAttribute(current.firstChild(lxTerminalMask));
   }
   else attrRef = mapAttribute(current);

   if (attrRef != 0) {
      _writer.insert(0, lxEnding, 0);
      if (current == lxToken) {
         insertIdentifier(_writer, current.firstChild(lxTerminalMask));
      }
      else insertIdentifier(_writer, current);
      _writer.insert(0, lxAttribute, attrRef);
   }
   else _scope->raiseError(errInvalidSyntax, _filePath, current); // !! temporal
}

void DerivationWriter :: generateExpressionTree(SNode node/*, DerivationScope& scope*/, int mode)
{
   _writer.newBookmark();
   
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
            if (!first) {
               _writer.insert(lxExpression);
               _writer.closeNode();
            }
            else first = false;

            _writer.newNode(lxMessage);
            copyIdentifier(_writer, current.firstChild(lxTerminalMask));
            _writer.closeNode();
            break;
         case lxExpression:
            //first = false;
            //if (test(mode, MODE_MESSAGE_BODY)) {
            //   generateExpressionTree(writer, current, scope);
            //}
            /*else */generateExpressionTree(current, 0/*EXPRESSION_IMPLICIT_MODE*/);
            break;
         case lxAssign:
            _writer.appendNode(lxAssign);
            break;
         default:
            if (isTerminal(current.type)) {
               if (current.nextNode() == lxToken) {
                  do {
                     generateExpressionAttribute(current);
                     current = current.nextNode();

                  } while (current.nextNode() == lxToken);

                  copyIdentifier(_writer, current.firstChild(lxTerminalMask));
               }
            //            identMode = true;
            //            if (scope.type == DerivationScope::ttFieldTemplate) {
            //               int index = scope.fields.get(current.identifier());
            //               if (index != 0) {
            //                  writer.newNode(lxTemplateField, index);
            //                  copyIdentifier(writer, current);
            //                  writer.closeNode();
            //               }
            //               else copyIdentifier(writer, current);
            //            }
            //            else if (scope.type == DerivationScope::ttCodeTemplate && scope.mapParameter(current.identifier())) {
            //               writer.newNode(lxTemplateParam, 1);
            //               copyIdentifier(writer, current);
            //               writer.closeNode();
            //            }
            //            else if (nextNode == lxNestedClass && /*scope.mapParameter(current.identifier())*/scope.reference == INVALID_REF) {
            //               int paramIndex = scope.mapParameter(current.identifier());
            //               if (paramIndex) {
            //                  writer.newNode(lxTemplateParam, paramIndex);
            //                  copyIdentifier(writer, current);
            //                  writer.closeNode();
            //               }
            //               else {
            //                  ref_t reference = scope.mapReference(current);
            //                  if (!reference) {
            //                     scope.raiseError(errUnknownSubject, current);
            //                  }
            //                  else if (isPrimitiveRef(reference))
            //                     scope.raiseError(errInvalidHint, current);
            //
            //                  writeFullReference(writer, scope.compilerScope->module, reference);
            //               }
            //            }
               else copyIdentifier(_writer, current);
            }
            //         else scope.raiseError(errInvalidSyntax, current);            
            break;
      }

      current = current.nextNode();
   }

//      switch (current.type) {
//         case lxObject:
//            if (!first) {
//               if (expressionExpected) {
//                  insertBookmarks(writer, bookmarks);
//                  writer.insert(lxExpression);
//                  writer.closeNode();
//               }
//               writer.removeBookmark();
//               writer.newBookmark();
//            }
//            else first = false;
//
//            expressionExpected = !implicitMode;
//            if (isTemplateBracket(current.nextNode())) {
//               generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
//            }
//            else generateObjectTree(writer, current.firstChild(), scope, MODE_OBJECTEXPR);
//            break;
//         case lxCatchOperation:
//         case lxAltOperation:
//            writer.newBookmark();
//         case lxIdleMsgParameter:
//         case lxMessageParameter:
//         case lxMessage:
//            //insertBookmarks(writer, bookmarks);
//            expressionExpected = false;
//            generateMessageTree(writer, current, scope);
//            writer.insert(lxExpression);
//            writer.closeNode();
//            if (current == lxCatchOperation) {
//               writer.removeBookmark();
//               writer.insert(lxTrying);
//               writer.closeNode();
//               expressionExpected = true;
//            }
//            else if (current == lxAltOperation) {
//               writer.removeBookmark();
//               writer.insert(lxAlt);
//               writer.closeNode();
//               expressionExpected = true;
//            }
//            break;
//         case lxArrOperator:
//            expressionExpected = false;
//            copyOperator(writer, current);
//            if (isTemplateBracket(current.nextNode())) {
//               generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
//            }
//            else generateExpressionTree(writer, current, scope, EXPRESSION_OPERATOR_MODE | EXPRESSION_IMPLICIT_MODE | EXPRESSION_OBJECT_REQUIRED);
//            writer.insert(lxExpression);
//            writer.closeNode();
//            break;
//         case lxOperator:
//         {
//            expressionExpected = true;
//
//            // HOTFIX : arranging the operator precedence
//            int level = defineOperatorLevel(current);
//            int last_level = bookmarks.peek() & 7;
//            while (last_level && last_level <= level) {
//               bookmarks.pop();
//               writer.removeBookmark();
//
//               if (bookmarks.Count() != 0) {
//                  writer.insert(bookmarks.peek() >> 3, lxExpression, 0);
//               }
//               else writer.insert(lxExpression);
//               writer.closeNode();
//
//               last_level = bookmarks.peek() & 7;
//            }
//
//            copyOperator(writer, current);
//            int bm = writer.newBookmark();
//            bookmarks.push((bm << 3) + level);
//
//            if (isTemplateBracket(current.nextNode())) {
//               generateNewTemplate(writer, current, scope, scope.reference == INVALID_REF);
//            }
//            else generateExpressionTree(writer, current, scope, EXPRESSION_OPERATOR_MODE | EXPRESSION_IMPLICIT_MODE | EXPRESSION_OBJECT_REQUIRED);
//
//            break;
//         }
//         case lxExpression:
//            first = false;
//            if (test(mode, MODE_MESSAGE_BODY)) {
//               generateExpressionTree(writer, current, scope);
//            }
//            else generateExpressionTree(writer, current, scope, EXPRESSION_IMPLICIT_MODE);
//            break;
//         case lxAssigning:
//            insertBookmarks(writer, bookmarks);
//            if (expressionExpected) {
//               writer.insert(lxExpression);
//               writer.closeNode();
//            }
//
//            writer.appendNode(lxAssign);
//            generateExpressionTree(writer, current, scope);
//            expressionExpected = true;
//            break;
//         case lxCode:
//            first = false;
//            generateCodeExpression(writer, current, scope);
//            break;
//         case lxExtension:
//            writer.newNode(current.type, current.argument);
//            generateExpressionTree(writer, current, scope, EXPRESSION_IMPLICIT_MODE);
//            writer.closeNode();
//            break;
//         case lxSwitching:
//            generateSwitchTree(writer, current, scope);
//            writer.insert(lxSwitching);
//            writer.closeNode();
//            expressionExpected = true;
//            break;
//         case lxIdle:
//            break;
//         default:
//            scope.raiseError(errInvalidSyntax, current);
//            break;
//      }
//
//      current = current.nextNode();
//   }
//
//   insertBookmarks(writer, bookmarks);

   if (expressionExpected) {
      _writer.insert(lxExpression);
      _writer.closeNode();
   }

//   if (first && test(mode, EXPRESSION_OBJECT_REQUIRED))
//      scope.raiseError(errInvalidSyntax, node);

   _writer.removeBookmark();
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
//

//// ---  DerivationTransformer ---
//
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
//inline SNode goToNode(SNode current, LexicalType type)
//{
//   while (current != lxNone && current != type)
//      current = current.nextNode();
//
//   return current;
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
//
//// --- DerivationTransformer::DerivationScope ---
//
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
//
//bool DerivationTransformer::DerivationScope :: generateClassName()
//{
//   ident_t templateName = compilerScope->module->resolveReference(templateRef);
//   NamespaceName rootNs(templateName);
//   IdentifierString name;
//   if (isWeakReference(templateName)) {
//      name.copy(compilerScope->module->Name());
//      name.append(templateName);
//
//      rootNs.copy(compilerScope->module->Name());
//   }
//   else name.copy(templateName);
//
//   SubjectMap::Iterator it = parameterValues.start();
//   while (!it.Eof()) {
//      name.append('&');
//
//      ident_t param = compilerScope->module->resolveReference(*it);
//      if (NamespaceName::compare(param, rootNs)) {
//         name.append(param + getlength(rootNs) + 1);
//      }
//      else if (isWeakReference(param) && !isTemplateWeakReference(param)) {
//         if (!compilerScope->module->Name().compare(rootNs.c_str())) {
//            name.append(compilerScope->module->Name());
//            name.append(param);
//         }
//         else name.append(param + 1);
//      }
//      else name.append(param);
//
//      it++;
//   }
//   name.replaceAll('\'', '@', 0);
//
//   bool alreadyDeclared = false;
//   reference = compilerScope->mapTemplateClass(ns, name, alreadyDeclared);
//
//   return !alreadyDeclared;
//}
//
//_Memory* DerivationTransformer::DerivationScope :: loadTemplateTree()
//{
//   ref_t ref = 0;
//   _Module* argModule = compilerScope->loadReferenceModule(compilerScope->module->resolveReference(templateRef), ref);
//
//   return argModule ? argModule->mapSection(ref | mskSyntaxTreeRef, true) : NULL;
//}
//
////ref_t DerivationReader::DerivationScope :: mapTypeTemplate(SNode current)
////{
////   SNode attrNode = current.findChild(lxAttributeValue).firstChild(lxTerminalObjMask);
////   ref_t attrRef = mapTerminal(attrNode, true);
////   if (attrRef == 0)
////      attrRef = mapTerminal(attrNode);
////
////   return attrRef;
////}
//
//// --- DerivationReader ---
//
//DerivationTransformer :: DerivationTransformer(SyntaxTree& tree)
//{
//   _root = tree.readRoot();
//
////   ByteCodeCompiler::loadVerbs(_verbs);
//}
//
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
//void DerivationTransformer :: copyExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   if (node.strArgument != -1) {
//      writer.newNode(node.type, node.identifier());
//   }
//   else writer.newNode(node.type, node.argument);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      copyTreeNode(writer, current, scope);
//
//      current = current.nextNode();
//   }
//
//   writer.closeNode();
//}
//
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
//
//void DerivationTransformer :: copyTreeNode(SyntaxWriter& writer, SNode current, DerivationScope& scope)
//{
//   if (test(current.type, lxTerminalMask | lxObjectMask)) {
//      copyIdentifier(writer, current);
//   }
//   else if (current == lxTemplate) {
//      writer.appendNode(lxTemplate, scope.templateRef);
//   }
//   else if (current == lxTemplateParam) {
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
//   }
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
//   else copyExpressionTree(writer, current, scope);
//}
//
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
//   //   case lxSwitching:
//   //      generateSwitchTree(writer, current, scope);
//   //      writer.insert(lxSwitching);
//   //      writer.closeNode();
//   //      expressionExpected = true;
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
//
//void DerivationTransformer :: copyFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   writer.newNode(node.type, node.argument);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxIdentifier || current == lxPrivate || current == lxReference) {
//         copyIdentifier(writer, current);
//      }
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
//      else if (current == lxSize) {
//         writer.appendNode(current.type, current.argument);
//      }
//      else if (current == lxAttribute)
//         writer.appendNode(current.type, current.argument);
//
//      current = current.nextNode();
//   }
//
//   writer.closeNode();
//}
//
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
//
//void DerivationTransformer :: copyMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   writer.newNode(node.type, node.argument);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      copyTreeNode(writer, current, scope);
//
//      current = current.nextNode();
//   }
//
//   writer.closeNode();
//}
//
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
//
//bool DerivationTransformer :: generateTemplate(SyntaxWriter& writer, DerivationScope& scope, bool declaringClass, int mode)
//{
//   _Memory* body = scope.loadTemplateTree();
//   if (body == NULL)
//      return false;
//
//   SyntaxTree templateTree(body);
//   SNode root = templateTree.readRoot();
//
//   if (declaringClass) {
//      // HOTFIX : exiting if the class was already declared in this module
//      if (!scope.generateClassName())
//         return true;
//
//      ident_t fullName = scope.compilerScope->resolveFullName(scope.reference);
//
//      writer.newNode(lxClass, -1);
//      writer.newNode(lxNameAttr, scope.compilerScope->mapFullReference(fullName, true));
//      writer.appendNode(lxReference, fullName);
//      writer.closeNode();
//   }
//
//   //SyntaxTree buffer;
//
//   SNode current = root.firstChild();
//   while (current != lxNone) {
//      if (current == lxAttribute) {
//         if (current.argument == V_TEMPLATE/* && scope.type != TemplateScope::ttAttrTemplate*/) {
//            // ignore template attributes
//         }
//         else if (current.argument == V_FIELD/* && scope.type != TemplateScope::ttAttrTemplate*/) {
//            // ignore template attributes
//         }
//         else if (current.argument == V_ACCESSOR) {
//            if (scope.type == DerivationScope::ttFieldTemplate) {
//               // HOTFIX : is it is a method template, consider the field name as a message subject
//               scope.type = DerivationScope::ttMethodTemplate;
//            }
//         }
//         else if (!test(mode, MODE_IMPORTING)) {
//            // do not copy the class attributes in the import mode 
//            writer.newNode(current.type, current.argument);
//            SyntaxTree::copyNode(writer, current);
//            writer.closeNode();
//         }
//      }
//      else if (current == lxTemplateParent && !test(mode, MODE_IMPORTING)) {
//         // HOTFIX : class based template
//         writer.newNode(lxBaseParent, -1);
//         copyClassTree(writer, current.findChild(lxTypeAttr), scope);
//         writer.closeNode();
//      }
//      else if (current == lxClassMethod) {
//         copyMethodTree(writer, current, scope);
//      }
//      else if (current == lxClassField) {
//         copyFieldTree(writer, current, scope);
//      }
//      else if (current == lxFieldInit) {
//         writer.newNode(lxFieldInit);
//         copyIdentifier(writer, current.findChild(lxMemberIdentifier));
//         writer.closeNode();
//
//         SyntaxWriter initWriter(*scope.autogeneratedTree);
//         copyFieldInitTree(initWriter, current, scope);
//      }
//      current = current.nextNode();
//   }
//
//   if (declaringClass) {
//      writer.closeNode();
//   }
//
//   return true;
//}
//

//void DerivationTransformer :: generateSwitchTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxSwitchOption:
//         case lxBiggerSwitchOption:
//         case lxLessSwitchOption:
//            if (current.type == lxBiggerSwitchOption) {
//               writer.newNode(lxOption, GREATER_MESSAGE_ID);
//            }
//            else if (current.type == lxLessSwitchOption) {
//               writer.newNode(lxOption, LESS_MESSAGE_ID);
//            }
//            else writer.newNode(lxOption, EQUAL_MESSAGE_ID);
//            writer.newBookmark();
//            generateObjectTree(writer, current.firstChild(), scope);
//            writer.removeBookmark();
//            writer.closeNode();
//            break;
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
//      }
//
//      current = current.nextNode();
//   }
//}
//
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
//
//ref_t DerivationTransformer :: generateNewTemplate(DerivationScope& templateScope)
//{
//   SyntaxTree buffer;
//   SyntaxWriter bufferWriter(buffer);
//   generateTemplate(bufferWriter, templateScope, true);
//
//   copyAutogeneratedClass(buffer, *templateScope.autogeneratedTree);
//
//   autoGenerateExtensions(templateScope);
//
//   return templateScope.reference;
//}
//
//ref_t DerivationTransformer :: generateTemplate(SyntaxWriter& writer, _CompilerScope& scope, ref_t reference, List<ref_t>& parameters)
//{
//   SyntaxTree autogeneratedTree;
//
//   DerivationScope templateScope(&scope, NULL, NULL, NULL);
//   templateScope.templateRef = reference;
//   templateScope.autogeneratedTree = &autogeneratedTree;
//   templateScope.sourcePath = "compiling template...";
//
//   for (auto it = parameters.start(); !it.Eof(); it++) {
//      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
//   }
//
//   ref_t generatedReference = generateNewTemplate(templateScope);
//
//   SyntaxTree::moveNodes(writer, autogeneratedTree, lxClass);
//
//   return generatedReference;
//}
//
//ref_t DerivationTransformer :: generateNewTemplate(DerivationScope& scope, ref_t attrRef, SNode node)
//{
//   DerivationScope templateScope(&scope, attrRef);
//   loadParameterValues(node, templateScope, &scope.parameterValues/*, true*/);
//
//   return generateNewTemplate(templateScope);
//}
//
//ref_t DerivationTransformer :: mapNewTemplate(SNode node, DerivationScope& scope, bool operationMode, bool& arrayMode, int& paramIndex, bool templateMode, List<int>* templateAttributes)
//{
//   IdentifierString attrName;
//
//   SNode attr = /*node == lxIdleAttribute ? node.findChild(lxAttributeValue).findChild(lxIdentifier, lxPrivate) : */node.firstChild(lxTerminalMask);
//   if (attr == lxNone)
//      attr = node.findChild(lxObject).firstChild(lxTerminalMask);
//
////   if (attr == lxNone)
////      scope.raiseError(errInvalidSyntax, node);
//
//   ref_t typeRef = 0;
//   bool classMode = false;
////   bool paramMode = false;   
//   int prefixCounter = 0;
//   SNode operatorNode;
//   if (operationMode) {
//      operatorNode = node.nextNode();
//      prefixCounter = SyntaxTree::countNode(operatorNode, /*lxIdleAttribute, */lxAttributeValue, lxClassRefAttr);
//   }
//   else {
//      operatorNode = node.findChild(lxAttributeValue);
//      prefixCounter = SyntaxTree::countChild(node, lxAttributeValue);
//   }
//
//   ref_t attrRef = scope.mapAttribute(attr, paramIndex);
//
////   if (operatorNode.existChild(lxSize) && prefixCounter == 0 && (!isPrimitiveRef(attrRef) || (templateMode && attrRef == INVALID_REF))) {
////      //expr = operatorNode.findChild(lxObject);
////      arrayMode = true;
////   }
//   /*else */if (attrRef == V_OBJARRAY && prefixCounter == 1) {
////      //expr = goToNode(operatorNode, lxOperator).findChild(lxObject);
//      arrayMode = true;
//      attr = operatorNode;
////      attrRef = scope.mapTypeTerminal(operatorNode.findChild(lxAttributeValue).findChild(lxIdentifier, lxReference), true);
//   }
//   else if (attrRef == V_TYPETEMPL && prefixCounter == 1) {
//      attr = operatorNode;
//   }
//   else {
//      attrName.copy(attr.identifier());
//      attrName.append('#');
//      attrName.appendInt(prefixCounter);
//
//      attrRef = scope.mapIdentifier(attrName.c_str(), attr == lxReference);
//      if (!attrRef)
//         scope.raiseError(errUnknownSubject, node);
//
//      //expr = goToNode(operatorNode, lxOperator).findChild(lxObject);
//      classMode = true;
//   }
//
//   if (!attrRef)
//      scope.raiseError(errInvalidHint, node);
//
//   if (templateMode) {
//      if (classMode && templateAttributes != NULL) {
//         DerivationScope templateScope(&scope, attrRef);
//         loadParameterValues(operatorNode, templateScope, &scope.parameterValues/*, true*/);
//
//         auto sub_attr = templateScope.parameterValues.start();
//         while (!sub_attr.Eof()) {
//            templateAttributes->add(*sub_attr);
//
//            sub_attr++;
//         }
//      }
//
//      return attrRef;
//   }
//   else if (attrRef == V_TYPETEMPL || attrRef == V_OBJARRAY) {
//      typeRef = scope.mapReference(attr.firstChild(lxTerminalMask));
//      if (typeRef == V_PARAMETER) {
//         paramIndex = scope.mapParameter(attr.firstChild(lxTerminalMask).identifier());
//      }
//      else if (isPrimitiveRef(typeRef)) {
//         // HOTFIX : all attributes are treated as class references
//         typeRef = scope.mapIdentifier(attr.firstChild(lxTerminalMask).identifier(), false);
//      }
//   }
//   else if (classMode) {
//      typeRef = generateNewTemplate(scope, attrRef, operatorNode);
//   }
//
//   return typeRef;
//}
//
////inline void modifyTemplateOperand(SNode current)
////{
////   current = lxFalseAttribute;
////   SNode objectNode = current.findChild(lxObject);
////   objectNode = lxAttributeValue;
////}
//
//inline void moveUpTerminal(SNode& current)
//{
//   SNode targetNode = current.firstChild();
//   SNode sourceNode = current.findChild(lxObject).firstChild(lxTerminalMask);
//
//   // HOTFIX : the reserve copy of the string should be done before inserting
//   IdentifierString copy(sourceNode.identifier());
//   targetNode.set(sourceNode.type, copy.c_str());
//}
//
//void DerivationTransformer :: generateTemplateParameters(SNode& current, DerivationScope& scope, bool templateMode)
//{
//   SNode lastNode;
//   
//   bool starting = true;
//   while (true) {
//      if (current == lxOperator) {
//         if (!isClosingOperator(current)) {
//            if (starting) {
//               current = /*lxIdleAttribute*/lxAttributeValue;
//
//               // HOTFIX:copy the identifier to upper level
//               moveUpTerminal(current);
//
//               //objectNode = lxIdle
//               //objectNode = lxAttributeValue;
//
//               starting = false;
//            }
//            else generateSubTemplate(current, scope, templateMode);
//         }
//         else break;
//      }
//      else if (current == lxObject) {
//         current = lxAttributeValue;
//      }
//      else scope.raiseError(errInvalidSyntax, current);
//   
//      lastNode = current;
//      current = current.nextNode();
//      if (current == lxNone)
//         scope.raiseError(errInvalidSyntax, lastNode);
//   }
//}
//
//void DerivationTransformer :: generateSubTemplate(SNode& node, DerivationScope& scope, bool templateMode)
//{
//   SNode current = node;
//   node = node.prevNode();
//
//   generateTemplateParameters(current, scope, templateMode);
//
//   bool invalid = false;
//   int paramIndex = 0;
//   List<int> templateAttributes;
//   ref_t paramRef = mapNewTemplate(node, scope, true, invalid, paramIndex, templateMode, &templateAttributes);
//   if (invalid)
//      scope.raiseError(errInvalidSyntax, node);
//
//   node = lxIdle;
//   do {
//      node = node.nextNode();
//      node = lxIdle;
//   } while (node != current);
//
//   if (templateAttributes.Count() > 0) {
//      ident_t attrName = scope.compilerScope->module->resolveReference(paramRef);
//      if (isWeakReference(attrName)) {
//         IdentifierString fullName(scope.compilerScope->module->Name(), attrName);
//
//         node.set(lxClassRefAttr, fullName.c_str());
//      }
//      else node.set(lxClassRefAttr, attrName);
//
//      for (auto it = templateAttributes.start(); !it.Eof(); it++) {
//         node.appendNode(lxTemplateAttribute, *it);
//      }
//   }
//   else {
//      node = lxAttributeValue;
//
//      SNode terminalNode = node.firstChild(lxTerminalMask);
//      if (terminalNode == lxNone)
//         terminalNode = node.firstChild();
//
//      terminalNode.set(lxReference, scope.compilerScope->module->resolveReference(paramRef));
//   }
//}
//
//void DerivationTransformer :: generateNewTemplate(SyntaxWriter& writer, SNode& node, DerivationScope& scope, bool templateMode)
//{
//   SNode current = findTemplateEnd(node.nextNode());
//
//   // recognize the template attributes
////   if (!current.existChild(lxSize))
//      SNode paramNode = node.nextNode();
//      generateTemplateParameters(paramNode, scope, templateMode);
//
//   SNode expr = current.findChild(lxObject, lxNestedClass);
//
//   bool arrayMode = false;
//   int paramIndex = 0;
//   ref_t typeRef = mapNewTemplate(node, scope, true, arrayMode, paramIndex, templateMode, NULL);
//   if (!typeRef)
//      scope.raiseError(errUnknownSubject, node);
//
//   if (templateMode) {
//      // template in template should be copied "as is" (resolving all references)
//      writer.newNode(lxTemplateBoxing, -1);
//
//      //if (templateAttributes.Count() > 0) {
//      //   ident_t attrName = retrieveKey(scope.compilerScope->attributes.start(), paramRef, DEFAULT_STR);
//      //   if (isWeakReference(attrName)) {
//      //      IdentifierString fullName(scope.compilerScope->module->Name(), attrName);
//
//      //      node.set(lxClassRefAttr, fullName.c_str());
//      //   }
//      //   else node.set(lxClassRefAttr, attrName);
//
//      //   for (auto it = templateAttributes.start(); !it.Eof(); it++) {
//      //      node.appendNode(lxTemplateAttribute, *it);
//      //   }
//      //}
//
//      if (arrayMode) {
//         writer.appendNode(lxSize, -1);
//         if (paramIndex) {
//            writer.appendNode(lxTemplateAttribute, paramIndex);
//         }
//         else if (isPrimitiveRef(typeRef)) {
//            writer.appendNode(lxAttribute, typeRef);
//         }
//         else writeFullReference(writer, scope.compilerScope->module, typeRef);
//      }
//      else {
//         if (isPrimitiveRef(typeRef)) {
//            writer.appendNode(lxAttribute, typeRef);
//         }
//         else writeFullReference(writer, scope.compilerScope->module, typeRef);
//
//      }
//      SNode operatorNode = node.nextNode();
//      copyTemplateAttributeTree(writer, operatorNode, scope);
//
//      SNode attr = /*node == lxIdleAttribute ? node.findChild(lxAttributeValue).findChild(lxIdentifier, lxPrivate) : */node.firstChild(lxTerminalMask);
//      copyIdentifier(writer, attr);
//
//      if (expr == lxNestedClass) {
//         if (arrayMode)
//            scope.raiseError(errIllegalOperation, current);
//
//         writer.newBookmark();
//         recognizeScopeMembers(expr, scope, MODE_ROOT);
//         generateClassTree(writer, expr, scope, -1);
//         writer.removeBookmark();
//
//         writer.insert(lxExpression);
//         writer.closeNode();
//      }
//      else if (expr == lxNone || expr.existChild(lxIdleMsgParameter)) {
//         // do nothing for idle parameter
//      }
//      else {         
//         writer.newNode(lxReturning);
//         if (expr == lxObject) {
//            generateExpressionTree(writer, expr, scope, MODE_MESSAGE_BODY);
//         }
//         else generateExpressionTree(writer, expr, scope);
//         writer.closeNode();
//      }
//      writer.closeNode();
//
//      node = goToNode(node.nextNode(), lxOperator);
//   }
//   else {
//      writeFullReference(writer, scope.compilerScope->module, typeRef, node);
//
//      if (expr == lxNestedClass) {
//         if (!arrayMode) {
//            generateObjectTree(writer, expr, scope);
//         }
//         else scope.raiseError(errIllegalOperation, current);
//      }
//      else {
//         SNode exprOpNode;
//
//         if (arrayMode) {
//            writer.appendNode(lxOperator, -1);
//         }
//         if (expr.existChild(lxIdleMsgParameter)) {
//            // do nothing for idle parameter
//         }
//         else if (expr != lxNone) {
//            if (expr == lxObject) {
//               exprOpNode = expr.nextNode();
//
//               generateExpressionTree(writer, expr, scope, MODE_MESSAGE_BODY);
//            }
//            else generateExpressionTree(writer, expr, scope);
//         }
//         else scope.raiseError(errIllegalOperation, current);
//
//         writer.insert(lxBoxing);
//         writer.closeNode();
//
//         while (exprOpNode != lxNone) {
//            if (exprOpNode == lxMessage) {
//               generateMessageTree(writer, exprOpNode, scope);
//               writer.insert(lxExpression);
//               writer.closeNode();
//            }
//            exprOpNode = exprOpNode.nextNode();
//         }
//      }
//
//      node = current;
//   }
//}
//
//void DerivationTransformer :: generateMessageTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   bool invokeWithNoParamMode = node == lxIdleMsgParameter;
//   bool invokeMode = invokeWithNoParamMode || (node == lxMessageParameter);
//
//   SNode current;
//   if (invokeMode) {
//      writer.newNode(lxMessage);
//      writer.appendNode(lxIdentifier, INVOKE_MESSAGE);
//      writer.closeNode();
//
//      current = node;
//      if (invokeWithNoParamMode)
//         return;
//   }
//   else current = node.firstChild();
//
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxMessageParameter:
//            generateExpressionTree(writer, current, scope);
//            current = lxIdle; // HOTFIX : to prevent duble compilation of closure parameters
//            break;
//         case lxExpression:
//            generateExpressionTree(writer, current, scope/*, EXPRESSION_MESSAGE_MODE*/);
//            break;
//         case lxInlineClosure:
//            // COMPILER MAGIC : advanced closure syntax
//            generateClosureTree(writer, current, scope);
//            break;
//         case lxMessage:
//         case lxCatchOperation:
//         case lxAltOperation:
//         {
//            if (invokeMode/* || invokeWithNoParamMode*/) {
//               // message should be considered as a new operation if followed after closure invoke
//               return;
//            }
//            generateMessage(writer, current, scope/*, scope.reference == INVALID_REF*/);
//            break;
//         }
//         case lxIdentifier:
////         case lxPrivate:
////         case lxReference:
//            writer.newNode(lxMessage);
//            scope.copyName(writer, current);
//            writer.closeNode();
//            break;
//         case lxOperator:
//         case lxObject:
//            if (invokeMode/* || invokeWithNoParamMode*/) {
//               // operator should be considered as a new operation if followed after closure invoke
//               return;
//            }
//         default:
//            scope.raiseError(errInvalidSyntax, current);
//            break;
//      }
//      current = current.nextNode();
//   }
//}
//
//void DerivationTransformer :: generateCodeExpression(SyntaxWriter& writer, SNode current, DerivationScope& scope)
//{
//   generateCodeTree(writer, current, scope);
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
//   writer.insert(lxExpression);
//   writer.closeNode();
//}
//
//void DerivationTransformer :: generateObjectTree(SyntaxWriter& writer, SNode current, DerivationScope& scope, int mode)
//{
//   SNode nextNode = current.nextNode();
////   bool rootMode = test(mode, MODE_ROOT);
//   bool objectMode = test(mode, MODE_OBJECTEXPR);
//   bool identMode = false;
////   if (rootMode)
////      writer.newBookmark();
//
//   switch (current.type) {
//      case lxReferenceExpr:
//         writer.newNode(lxReferenceExpr);
//         generateExpressionTree(writer, current, scope, EXPRESSION_IMPLICIT_MODE);
//         writer.closeNode();
//         break;
//      case lxExpression:         
//         if (objectMode) {
//            writer.newNode(lxExpression);
//            generateExpressionTree(writer, current, scope);
//            writer.closeNode();
//         }
//         else generateExpressionTree(writer, current, scope);
//         break;
//      case lxMessageReference:
////         writer.newNode(lxExpression);
//         writer.newNode(current.type);
//         if (scope.type == DerivationScope::ttFieldTemplate) {
//            scope.copyName(writer, current.findChild(lxIdentifier, lxLiteral));
//         }
//         else copyIdentifier(writer, current.findChild(lxIdentifier, lxLiteral));
//         writer.closeNode();
////         writer.closeNode();
//         break;
//      case lxLazyExpression:
//         writer.newNode(current.type);
//         generateExpressionTree(writer, current, scope);
//         writer.closeNode();
//         break;
//      case lxNestedClass:
//         if (scope.type == DerivationScope::ttCodeTemplate && test(scope.mode, daNestedBlock)) {
//            writer.insert(lxTemplateParam, 2);
//            writer.closeNode();
//         }
//         else {
//            recognizeScopeMembers(current, scope, MODE_ROOT);
//
//            generateClassTree(writer, current, scope, -1);
//         }
//         writer.insert(lxExpression);
//         writer.closeNode();
//         break;
//      case lxReturning:
//         writer.newNode(lxCode);
//      case lxCode:
//         generateCodeExpression(writer, current, scope);
//         break;
//      case lxMethodParameter:
//      {
//         writer.newNode(lxMethodParameter);
//         copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
//         writer.closeNode();
//         break;
//      }
//      case lxAttributeValue:
//         generateTypeAttribute(writer, current, scope, scope.reference == INVALID_REF);
//         break;
//      default:
//      {
//         if (isTerminal(current.type)) {
//            identMode = true;
//            if (scope.type == DerivationScope::ttFieldTemplate) {
//               int index = scope.fields.get(current.identifier());
//               if (index != 0) {
//                  writer.newNode(lxTemplateField, index);
//                  copyIdentifier(writer, current);
//                  writer.closeNode();
//               }
//               else copyIdentifier(writer, current);
//            }
//            else if (scope.type == DerivationScope::ttCodeTemplate && scope.mapParameter(current.identifier())) {
//               writer.newNode(lxTemplateParam, 1);
//               copyIdentifier(writer, current);
//               writer.closeNode();
//            }
//            else if (nextNode == lxNestedClass && /*scope.mapParameter(current.identifier())*/scope.reference == INVALID_REF) {
//               int paramIndex = scope.mapParameter(current.identifier());
//               if (paramIndex) {
//                  writer.newNode(lxTemplateParam, paramIndex);
//                  copyIdentifier(writer, current);
//                  writer.closeNode();
//               }
//               else {
//                  ref_t reference = scope.mapReference(current);
//                  if (!reference) {
//                     scope.raiseError(errUnknownSubject, current);
//                  }
//                  else if (isPrimitiveRef(reference))
//                     scope.raiseError(errInvalidHint, current);
//
//                  writeFullReference(writer, scope.compilerScope->module, reference);
//               }
//            }
//            else copyIdentifier(writer, current);
//         }
//         else scope.raiseError(errInvalidSyntax, current);
//         break;
//      }
//   }
//
//   if (nextNode != lxNone) {
//      if (nextNode == lxExpression) {
//         generateExpressionTree(writer, nextNode, scope);
//
//         if (identMode) {
//            // HOTFIX : to parse a strong collection with operations
//            writer.insert(lxExpression);
//            writer.closeNode();
//         }
//      }
//      else generateObjectTree(writer, nextNode, scope);
////
////      if (rootMode && singleMode) {
////         writer.insert(lxExpression);
////         writer.closeNode();
////      }
//   }
////
////   if (rootMode)
////      writer.removeBookmark();
//}
//
//inline int defineOperatorLevel(SNode node)
//{
//   SNode ident = node.firstChild();
//   if (emptystr(ident.identifier())) {
//      return ident.type - nsL0Operator + 1;
//   }
//   return 10;
//}
//
//void DerivationTransformer :: copyOperator(SyntaxWriter& writer, SNode& node)
//{
//   int operator_id = node.argument;
//
//   SNode ident = node.firstChild();
//   if (emptystr(ident.identifier())) {
//
//      ident = ident.firstChild();
//   }
//
//   if (operator_id != 0) {
//      writer.newNode(lxOperator, operator_id);
//   }
//   else if (isClosingOperator(node)) {
//      SNode nextNode = node.nextNode();
//
//      if (nextNode == lxOperator && isClosingOperator(nextNode) && !node.existChild(lxObject)) {
//         node = lxIdle;
//         node = nextNode;
//         writer.newNode(lxOperator, ">>");
//      }
//      else writer.newNode(lxOperator, ">");
//   }
//   //else if (emptystr(ident.identifier())) {
//   //   writer.newNode(ident.type);
//   //}
//   else writer.newNode(lxOperator, ident.identifier());
//
//   SyntaxTree::copyNode(writer, lxRow, ident);
//   SyntaxTree::copyNode(writer, lxCol, ident);
//   SyntaxTree::copyNode(writer, lxLength, ident);
//   writer.closeNode();
//}
//
//inline void insertBookmarks(SyntaxWriter& writer, Stack<int>& bookmarks)
//{
//   bool skipFirst = true;
//   while (bookmarks.Count() > 0) {
//      if (!skipFirst) {
//         // the last bookmark can be ignored
//         writer.insert(bookmarks.pop() >> 3, lxExpression, 0);
//         writer.closeNode();
//      }
//      else {
//         bookmarks.pop();
//         skipFirst = false;
//      }
//      writer.removeBookmark();
//   }
//}
//
//void DerivationTransformer:: generateAssignmentOperator(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   writer.newNode(lxExpression);
//
//   SNode loperand = node.findChild(lxObject);
//   SNode operatorNode = node.findChild(lxAssignOperator);
//
//   if (loperand.nextNode() == lxArrOperator) {
//      // HOTFIX : if it is an assign operator with array brackets
//      SNode loperatorNode = loperand.nextNode();
//
//      writer.newBookmark();
//      writer.newNode(lxExpression);
//      generateObjectTree(writer, loperand.firstChild(), scope);
//      copyOperator(writer, loperatorNode);
//      generateExpressionTree(writer, loperatorNode, scope, EXPRESSION_OPERATOR_MODE);
//      writer.closeNode();
//      while (loperatorNode.nextNode() == lxArrOperator) {
//         loperatorNode = loperatorNode.nextNode();
//         generateObjectTree(writer, loperatorNode, scope);
//      }      
//      writer.removeBookmark();      
//
//      loperatorNode = loperand.nextNode();
//      writer.appendNode(lxAssign);
//      writer.newBookmark();
//      writer.newNode(lxExpression);
//      writer.newNode(lxExpression);
//      generateObjectTree(writer, loperand.firstChild(), scope);
//      copyOperator(writer, loperatorNode);
//      generateExpressionTree(writer, loperatorNode, scope, EXPRESSION_OPERATOR_MODE);
//      writer.closeNode();
//      while (loperatorNode.nextNode() == lxArrOperator) {
//         loperatorNode = loperatorNode.nextNode();
//         generateObjectTree(writer, loperatorNode, scope);
//      }
//      writer.removeBookmark();
//   }
//   else {
//      generateObjectTree(writer, loperand.firstChild(), scope);
//      writer.appendNode(lxAssign);
//      writer.newNode(lxExpression);
//      generateObjectTree(writer, loperand.firstChild(), scope);
//   }
//
//   IdentifierString operatorName(operatorNode.firstChild().identifier(), 1);
//   writer.appendNode(lxOperator, operatorName.c_str());
//
//   generateExpressionTree(writer, operatorNode, scope, EXPRESSION_OPERATOR_MODE);
//   writer.closeNode();
//
//   writer.closeNode();
//}
//
//bool DerivationTransformer :: checkPatternDeclaration(SNode node, DerivationScope&)
//{
//   return node.existChild(lxCode, lxNestedClass);
//}
//
//inline bool checkVarTemplateBrackets(SNode node)
//{
//   if (node.firstChild().firstChild().identifier().compare("<")) {
//      int level = 0;
//      SNode current = node;
//      while (current != lxNone) {
//         if (current == lxOperator) {
//            if (!isSingleStatement(current))
//               return false;
//
//            if (isClosingOperator(current)) {
//               level--;
//               if (level == 0) {
//                  SNode objNode = current.findChild(lxObject);
//                  if (objNode != lxNone) {
//                     SNode childNode = objNode.firstChild();
//                     if (childNode == lxIdentifier) 
//                        return current.nextNode().compare(lxAssigning, lxNone);
//                  }
//               }
//            }
//            else level++;
//         }
//         current = current.nextNode();
//      }
//   }
//
//   return false;
//}
//
////inline void setTemplateAttributes(SNode current)
////{
////   while (current != lxAssigning) {
////      if (current == lxOperator) {
////         current = lxIdleAttribute;
////
////         SNode objectNode = current.findChild(lxObject);
////         objectNode = lxAttributeValue;
////      }
////      else if (current == lxObject) {
////         current = lxAttributeValue;
////      }
////
////      current = current.nextNode();
////   }
////}
//
//bool DerivationTransformer :: checkVariableDeclaration(SNode node, DerivationScope& scope)
//{
//   SNode current = node.firstChild();
//   if (current != lxObject || !current.existChild(lxIdentifier, lxReference, lxGlobalReference))
//      return false;
//
//   SNode nextNode = current.nextNode();
//   if (nextNode == lxMessage && node.existChild(lxAssigning)) {
//      ref_t attrRef = scope.mapAttribute(current/*, true*/);
//      if (attrRef != 0) {
//         // HOTFIX : set already recognized attribute value if it is not a template parameter
////            if (attrRef != INVALID_REF) {
////               current.setArgument(attrRef);
////            }
////
//         current = lxAttribute;
//         nextNode = lxAttribute;
//
//         return true;
//      }
//   }
//   else if (nextNode == lxOperator) {
//      return checkVarTemplateBrackets(nextNode);
//   }
//
//   return false;
//}
//
//void DerivationTransformer :: generateVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   bool templatedBased = false;
//   bool idleMode = false; 
//
//   SNode current = node.findChild(lxAssigning);
//   if (current == lxNone) {
//      current = node.lastChild();
//
//      templatedBased = current == lxOperator;
//
//      idleMode = true;
//   }
//   else templatedBased = current.prevNode() == lxOperator;
//
//   if (templatedBased) {
//      SNode paramNode = node.firstChild().nextNode();
//
//      generateTemplateParameters(paramNode, scope, scope.reference == INVALID_REF);
//
//      paramNode = lxAttribute;
//
//      // HOTFIX:copy the identifier to upper level
//      moveUpTerminal(paramNode);
//
//      // mark the first node as an attribute
//      paramNode = node.firstChild();
//      paramNode = lxAttribute;
//
//      current.refresh();
//   }   
//
//   if (idleMode && current == lxAttribute) {
//      current = lxNameAttr;
//   }
//   else setIdentifier(current);
//
//   writer.newNode(lxVariable);
//
////   SNode ident = goToNode(attributes, lxNameAttr);
////   if (attributes == lxAttribute && attributes.argument == INVALID_REF) {
////      ident = ident.findChild(lxObject);
////   }
////   if (ident == lxNone)
////      scope.raiseError(errInvalidSyntax, node);
//
//   if (idleMode) {
//      generateAttributes(writer, current, scope, false, scope.reference == INVALID_REF, templatedBased);
//   }
//   else generateAttributes(writer, current.prevNode(), scope, false, scope.reference == INVALID_REF, templatedBased);
//
//   writer.closeNode();
//
//   if (!idleMode) {
//      writer.newNode(lxExpression);
//
//      copyIdentifier(writer, current.prevNode().firstChild(lxTerminalMask));
//
//      writer.appendNode(lxAssign);
//      generateExpressionTree(writer, current, scope/*, EXPRESSION_IMPLICIT_MODE*/);
//
//      writer.closeNode();
//   }
//}
//
////void DerivationReader :: generateArrayVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
////{
////   SNode current = node.firstChild();
////   SNode next = current.nextNode();
////
////   SNode attributes = current;
////
////   writer.newNode(lxVariable);
////
////   setIdentifier(attributes);
////
////   SNode ident = goToNode(attributes, lxNameAttr);
////   if (ident.existChild(lxObject))
////      ident = ident.findChild(lxObject);
////
////   generateAttributes(writer, SNode(), scope, current, scope.reference == INVALID_REF);
////
////   copyIdentifier(writer, ident.findChild(lxIdentifier, lxPrivate));
////
////   SNode size = next.findChild(lxAttributeValue).firstChild(lxTerminalMask);
////   if (size == lxInteger) {
////      writer.appendNode(lxAttribute, size.findChild(lxTerminal).identifier().toInt());
////   }
////   else if (size == lxHexInteger) {
////      writer.appendNode(lxAttribute, (ref_t)size.findChild(lxTerminal).identifier().toLong(16));
////   }
////
////   writer.closeNode();
////}
////
//////ref_t DerivationReader :: mapAttributeType(SNode attr, DerivationScope& scope)
//////{
//////   if (attr == lxIdleAttribute)
//////      attr = attr.findChild(lxAttributeValue);
//////
//////   ref_t typeRef = scope.mapTerminal(attr.findChild(lxIdentifier, lxPrivate), true);
//////   if (typeRef == 0)
//////      typeRef = scope.mapTerminal(attr.findChild(lxIdentifier, lxPrivate));
//////
//////   return typeRef;
//////}
//
//inline bool insertSize(SyntaxWriter& writer, SNode attr, bool dynamicMode)
//{
//   if (!dynamicMode) {
//      SNode sizeNode = goToNode(attr.nextNode(), lxAttributeValue).firstChild(lxTerminalMask);
//      if (sizeNode.compare(lxInteger, lxHexInteger)) {
//         writer.appendNode(lxSize, readSizeValue(sizeNode, sizeNode == lxHexInteger ? 16 : 10));
//      }
//      else return false;
//   }
//   else writer.appendNode(lxSize, -1);
//
//   return true;
//}
//
//ref_t DerivationTransformer :: mapCodeTemplateName(SNode node, int codeCounter, int nestedCounter, int parameterCounter, DerivationScope& scope)
//{
//   IdentifierString attrName;
//   bool globalOne = false;
//   if (node == lxGlobalReference) {
//      attrName.copy(node.identifier() + 1);
//      globalOne = true;
//   }
//   else attrName.copy(node.identifier());
//
//   attrName.append("##");
//   attrName.appendInt(codeCounter);
//   attrName.append('#');
//   attrName.appendInt(nestedCounter);
//   attrName.append('#');
//   attrName.appendInt(parameterCounter);
//
//   ref_t reference = 0;
//   if (!globalOne) {
//      reference = scope.mapIdentifier(attrName, node == lxReference);
//      if (!reference && node == lxReference)
//         reference = scope.compilerScope->mapFullReference(attrName.c_str(), true);
//   }
//   else reference = scope.compilerScope->mapFullReference(attrName.c_str(), true);
//
//   return reference;
//}
//
//ref_t DerivationTransformer :: mapTemplateName(SNode node, int prefixCounter, DerivationScope& scope, int postfixCounter)
//{
//   IdentifierString className;
//   //      if (!emptystr(node.identifier())) {
//   //         identNode = node;
//   //      }
//   //      else identNode = node.findChild(lxIdentifier, lxPrivate);
//
//   bool globalOne = false;
//   if (node == lxGlobalReference) {
//      className.copy(node.identifier() + 1);
//      globalOne = true;
//   }
//   else className.copy(node.identifier());
//
//   if (postfixCounter != 0) {
//      className.append('#');
//      className.appendInt(postfixCounter);
//   }
//
//   className.append('#');
//   className.appendInt(prefixCounter);
//
//   ref_t reference = 0;
//   if (!globalOne) {
//      reference = scope.mapIdentifier(className, node == lxReference);
//      if (!reference && node == lxReference) {
//         reference = scope.mapIdentifier(className, false);
//         if (!reference)
//            reference = scope.compilerScope->mapFullReference(className.c_str(), true);
//      }         
//   }
//   else reference = scope.compilerScope->mapFullReference(className.c_str(), true);
//
//   return reference;
//}
//
//void DerivationTransformer :: generateAttributeTemplate(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode, bool expressionMode)
//{
//   ref_t classRef = 0;
//
//   bool arrayMode = false;
//   bool dynamicMode = false;
//   bool newTemplateMode = false;
//
//   int prefixCounter = 0;
//   if (expressionMode) {
//      prefixCounter = SyntaxTree::countNode(node.nextNode(), lxAttributeValue);
//   }
//   else prefixCounter = SyntaxTree::countChild(node, lxAttributeValue);
//
//   ref_t attrRef = scope.mapAttribute(node/*, true*/);
//   if (attrRef == V_TYPETEMPL && prefixCounter == 1) {
//      // if it is a type atrribute
//      if (!expressionMode) {
//         if (node.findChild(lxAttributeValue).existChild(lxAttributeValue))
//            scope.raiseError(errInvalidHint, node);
//      }     
//      else if (goToNode(node, lxAttributeValue).existChild(lxAttributeValue))
//         scope.raiseError(errInvalidHint, node);
//   }
//   else if (attrRef == V_TYPETEMPL && prefixCounter == 2) {
//      // OBSOLETE : if it is an array atrribute
//      arrayMode = true;
//   }
//   else if (attrRef == V_OBJARRAY && prefixCounter == 2) {
//      // if it is an array atrribute
//      arrayMode = true;
//   }
//   else if (attrRef == V_OBJARRAY && prefixCounter == 1) {
//      // if it is an array atrribute
//      dynamicMode = arrayMode = true;
//   }
//   else {
//      SNode identNode = node.firstChild(lxTerminalMask);
//
//      attrRef = mapTemplateName(identNode, prefixCounter, scope);
//      if (!attrRef)
//         scope.raiseError(errUnknownTemplate, node);
//
//      newTemplateMode = true;
//   }
//
//   SNode attr;
//   if (expressionMode) {
//      attr = node.nextNode()/*.findChild(lxAttributeValue)*/;
//   }
//   else attr = node.findChild(lxAttributeValue);
//
//   if (templateMode) {
//      writer.newBookmark();
//
//      if (expressionMode) {
//         copyTemplateAttributeTree(writer, attr, scope);
//      }
//      else copyTemplateAttributeTree(writer, node.firstChild(), scope);
//
//      // template in template should be copied "as is" (resolving all references)
//      if (attrRef == V_TYPETEMPL || attrRef == V_OBJARRAY) {
//         if (arrayMode) {
//            if (!insertSize(writer, attr, dynamicMode))
//               scope.raiseError(errInvalidSubject, node);
//         }
//      }
//      else {
//         writeFullReference(writer, scope.compilerScope->module, attrRef);
//         writer.insert(lxTemplateParam, INVALID_REF);
//         writer.closeNode();
//      }
//
//      writer.removeBookmark();
//
//      //writer.newNode(lxTemplateParam, -1);
//
//      //if (attrRef == V_TYPETEMPL || attrRef == V_OBJARRAY) {
//      //   writer.appendNode(lxTemplate, attrRef);
//      //}
//      //else {
//      //   scope.raiseError(errUnknownSubject, node); // !! temporal
//
//      //   //writer.appendNode(lxTemplate, attrName.c_str());
//
//      //   //classRef = scope.mapReference(attr.firstChild(lxTerminalMask));
//      //   //      className.copy(attr.firstChild(lxTerminalMask).identifier());
//      //}
//
//      //copyIdentifier(writer, attr.firstChild(lxTerminalMask));
//
//      //writer.closeNode();
//
//      if (newTemplateMode && !expressionMode && scope.type == DerivationScope::ttExtTemplate) {
//         // hotfix : if it is a template extension
//         // save it to be auto-generated on demand
//         // NOTE : they should be declared in the same order
//         if (compareAttributes(node.firstChild(), scope)) {
//            scope.compilerScope->saveAutogerenatedExtension(attrRef, scope.extensionTemplateRef);
//         }
//      }
//
//      return;
//   }
//   else if (attrRef == V_TYPETEMPL || attrRef == V_OBJARRAY) {
//      classRef = scope.mapReference(attr.firstChild(lxTerminalMask));
////      className.copy(attr.firstChild(lxTerminalMask).identifier());
//   }
//   else {
//      classRef = generateNewTemplate(scope, attrRef, attr);
//   }
//
//   if (isPrimitiveRef(classRef)) {
//      if (arrayMode || classRef == V_ARGARRAY) {
//         // HOTFIX : recognize the open argument extension
//         writer.newNode(lxAttribute, classRef);
//      }
//      else scope.raiseError(errInvalidSubject, node);
//      copyIdentifier(writer, node);
//      writer.closeNode();
//   }
//   else if (classRef) {
//      writeFullReference(writer, scope.compilerScope->module, classRef, node);
//   }
//   else scope.raiseError(errUnknownClass, attr);
//
//   if (arrayMode) {
//      if (!insertSize(writer, attr, dynamicMode))
//         scope.raiseError(errInvalidSubject, node);
//   }
//}
//
//bool DerivationTransformer :: generateCodeTemplate(SyntaxWriter& writer, DerivationScope& scope, SubjectMap* parentAttributes)
//{
//   _Memory* body = scope.loadTemplateTree();
//   if (body == NULL)
//      return false;
//
//   SyntaxTree templateTree(body);
//
//   SNode root = templateTree.readRoot();
//
//   loadParameterValues(root.firstChild(), scope, parentAttributes/*, false*/);
//
//   SNode current = root.findChild(lxCode).firstChild();
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxLoop:
//         case lxExpression:
//         case lxExtern:
//            copyExpressionTree(writer, current, scope);
//            break;
//      }
//
//      current = current.nextNode();
//   }
//
//   return true;
//}
//
//void DerivationTransformer :: generateCodeTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   // check if the first token is attribute
//   SNode loperand = node.firstChild();
//   SNode attr;
//   if (loperand == lxObject) {
//      attr = loperand.findChild(lxIdentifier);
//   }
//   else if (loperand == lxIdentifier)
//      attr = loperand;
//
//   if (attr != lxNone) {
//      ref_t attrRef = mapCodeTemplateName(attr, 
//         SyntaxTree::countChild(node, lxCode),
//         SyntaxTree::countChild(node, lxNestedClass),
//         SyntaxTree::countChild(node, lxMessageParameter), scope);
//
//      if (attrRef != 0) {
//         DerivationScope templateScope(&scope, attrRef);
//         templateScope.exprNode = node.findChild(lxMessageParameter);
//         templateScope.codeNode = node.findChild(lxCode);
//         templateScope.nestedNode = node.findChild(lxNestedClass);
//         if (templateScope.nestedNode == lxNone || templateScope.codeNode != lxNone) {
//            // if there is else code block
//            templateScope.elseNode = templateScope.codeNode.nextNode();
//         }
//
//         templateScope.type = DerivationScope::ttCodeTemplate;
//
//         if (!generateCodeTemplate(writer, templateScope, &scope.parameterValues))
//            scope.raiseError(errInvalidHint, node);
//
//         return;
//      }
//      else scope.raiseError(errInvalidHint, node);
//   }
//
//   generateExpressionTree(writer, node, scope);
//}
//
//bool DerivationTransformer :: generateFieldTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer, bool/* templateMode*/)
//{
//   // if it is field / method template
//   int prefixCounter = 1;
//   SNode propNode;
//   if(setIdentifier(node))
//      propNode = node.prevNode().prevNode();
//
//   SNode attrNode = propNode.prevNode();
//   if (propNode == lxAttribute) {
//      if (attrNode == lxAttribute) {
//         //if the type attribute provoded
//         attrNode = lxAttributeValue;
//         //attributes.refresh();
//
//         prefixCounter++;
//      }
//      else attrNode = propNode;
//   }
//   else scope.raiseError(errInvalidHint, node);
//
//   ref_t attrRef = mapTemplateName(propNode.firstChild(lxTerminalMask), prefixCounter, scope, SyntaxTree::countChild(node, lxBaseParent));
//   if (!attrRef)
//      scope.raiseError(errInvalidHint, node);
//
//   DerivationScope templateScope(&scope, attrRef);
//   templateScope.type = DerivationScope::ttFieldTemplate;
//   templateScope.loadFields(node.findChild(lxBaseParent));
//
//   copyTemplateTree(writer, node, templateScope, attrNode, &scope.parameterValues);
//
//   // copy class initializers
//   SyntaxWriter initFieldWriter(buffer);
//   return SyntaxTree::moveNodes(initFieldWriter, *scope.autogeneratedTree, lxFieldInit);
//}
//
//bool DerivationTransformer :: generateFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer, bool templateMode)
//{
//   if (node == lxClassField) {
//      writer.newNode(lxClassField, templateMode ? -1 : 0);
//
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
//      else generateAttributes(writer, node.prevNode(), scope, false, templateMode, false);
//
//      writer.closeNode();
//   }
//   else if (node == lxFieldInit && !templateMode) {
//      SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
//
//      writer.newNode(lxFieldInit);
//      ::copyIdentifier(writer, nameNode);
//      writer.closeNode();
//   }
//
//   // copy inplace initialization
//   SNode bodyNode = node.findChild(lxAssigning);
//   if (bodyNode != lxNone) {
//      SyntaxWriter bufferWriter(buffer);
//
//      SNode nameNode = node.prevNode();
//
//      bufferWriter.newNode(lxFieldInit);
//      ::copyIdentifier(bufferWriter, nameNode.firstChild(lxTerminalMask));
//      bufferWriter.appendNode(lxAssign);
//      generateExpressionTree(bufferWriter, bodyNode.findChild(lxExpression), scope);
//      bufferWriter.closeNode();
//
//      return true;
//   }
//   else return false;
//}
//
//void DerivationTransformer :: declareType(/*SyntaxWriter& writer, */SNode node, DerivationScope& scope)
//{
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
//
//   SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
//   SNode referenceNode;
//
//   SNode current = node.firstChild();
//   bool invalid = true;
//   if (nameNode == lxIdentifier && isSingleStatement(current)) {
//      referenceNode = current.firstChild().firstChild(lxTerminalMask);
//
//      invalid = !referenceNode.compare(lxIdentifier, lxReference, lxGlobalReference);
//   }
//
//   if (invalid)
//      scope.raiseError(errInvalidSyntax, current);
//
//   // map a full type name
//   ident_t name = nameNode.identifier();
//   if (scope.compilerScope->attributes.exist(name))
//      scope.raiseError(errDuplicatedDefinition, nameNode);
//
//   ref_t classRef = scope.mapReference(referenceNode);
//   if (!classRef)
//      scope.raiseError(errInvalidHint, referenceNode);
//
//   scope.compilerScope->attributes.add(name, classRef);
//   scope.compilerScope->saveAttribute(name, classRef);
//
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
//}
//
//void DerivationTransformer :: includeModule(SNode ns, DerivationScope& scope)
//{
//   ident_t name = ns.findChild(lxIdentifier, lxReference).identifier();
//   if (name.compare(STANDARD_MODULE))
//      // system module is included automatically - nothing to do in this case
//      return;
//
//   bool duplicateInclusion = false;
//   if (scope.compilerScope->includeNamespace(*scope.imports, name, duplicateInclusion)) {
//      //if (duplicateExtensions)
//      //   scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, ns);
//   }
//   else if (duplicateInclusion) {
//      scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateInclude, ns);
//   }
//   else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, ns);
//}
//
//bool DerivationTransformer :: recognizeDeclaration(SNode node, DerivationScope& scope)
//{
//   // recognize the declaration type
//   DeclarationAttr declType = daNone;
//   SNode nameNode = node.prevNode();
//   if (nameNode != lxNameAttr)
//      scope.raiseError(errInvalidSyntax, node);
//
//   SNode current = nameNode.prevNode();
//   bool privateOne = true;
//   while (current == lxAttribute/* || current == lxAttributeDecl*/) {
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
//
//      current = current.prevNode();
//   }
//
//   if (nameNode.existChild(lxAttributeValue)) {
//      declType = (DeclarationAttr)(declType | daTemplate);
//   }      
//
////   attributes.refresh();
//
//   if (declType == daType) {
//      node = lxType;
//
////      SyntaxTree buffer;
////      SyntaxWriter bufferWriter(buffer);
////
////      bool retVal = declareType(bufferWriter, node, scope, attributes);
////
////      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);
////
////      return retVal;
//      return true;
//   }
//   else if (declType == daImport) {
//      node = lxIdle;
//
//      SNode name = node.prevNode();
//      if (name == lxNameAttr) {
//         includeModule(name, scope);
//
//         return true;
//      }
//      else return false;
//   }
//   else if (test(declType, daTemplate)) {
//      if (testany(declType, daImport | daType))
//         scope.raiseError(errInvalidSyntax, node);
//
//      node = lxTemplate;
//
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
//
//      return true;
//   }
//   else if (test(declType, daClass)) {
//      return false;
//   }
//   else return false;
//}
//
//void DerivationTransformer :: generateTemplateScope(SNode node, DerivationScope& scope)
//{
//   ref_t templateRef = node.argument;
//
//   // check for duplicate declaration
//   if (scope.compilerScope->module->mapSection(templateRef | mskSyntaxTreeRef, true))
//      scope.raiseError(errDuplicatedSymbol, node);
//
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
////      scope.moduleScope->saveAttribute(templateName, templateRef, false);
//}
//
//void DerivationTransformer :: generateTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
//{
//   SyntaxTree buffer((pos_t)0);
//
//   generateAttributes(writer, node.prevNode(), scope, false, true, false);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxAttribute|| current == lxNameAttr) {
//      }
//      else if (current == lxClassMethod) {
//         generateMethodTree(writer, current, scope, true, false);
////         subAttributes = SNode();
//      }
//      else if (current == lxClassField || current == lxFieldInit) {
//         if (generateFieldTree(writer, current, scope, buffer, true)) {
//            SyntaxTree::moveNodes(writer, buffer, lxFieldInit);
//         }
//      }
//      else if (current == lxFieldTemplate) {
//         generateFieldTemplateTree(writer, current, scope, buffer, true);
//      }
//      else if (current == lxCode) {
//         scope.type = DerivationScope::ttCodeTemplate;
//
//         generateCodeTree(writer, current, scope);
//      }
//      else if (current == lxBaseParent) {
//         //HOTFIX : check if it is template based on the class
//         writer.newNode(lxTemplateParent);
//
//         generateTypeAttribute(writer, current, scope, true);
//
//         //SNode terminalNode = current.firstChild(lxTerminalMask);
//         //ref_t reference = scope.mapReference(terminalNode);
//         //if (!reference)
//         //   scope.raiseError(errUnknownSubject, terminalNode);
//
//         //writeFullReference(writer, scope.compilerScope->module, reference);
//
//         writer.closeNode();
//      }
//
//      current = current.nextNode();
//   }
//}
////bool DerivationTransformer :: isImplicitAttribute(SNode node, DerivationScope& scope)
////{
////   ident_t name = node.identifier();
////   if (emptystr(name))
////      name = node.firstChild(lxTerminalMask).identifier();
////
////   ref_t attr = scope.attributes->get(name);
////
////   return /*attr == V_GENERIC || */attr == V_CONVERSION/* || attr == V_ACTION*/;
////}
//
//bool DerivationTransformer :: recognizeMethodScope(SNode node)
//{
//   SNode current = node.findChild(lxCode, lxExpression, lxDispatchCode/*, lxReturning*/, lxResendExpression);
//   if (current != lxNone) {
//      // try to resolve the message name
//      setIdentifier(node);
//
//      SNode nameAttr = node.prevNode();
//      if (nameAttr == lxNameAttr && nameAttr.existChild(lxAttributeValue)) {
//         // HOTFIX : recognize explicit conversion
//         nameAttr = lxAttribute;
//      }
//
//      // convert attributes into message or attribute values
//      SNode args = node.firstChild();
//      while (args != lxNone) {
//         if (args == lxAttribute) {
//            if (args.existChild(lxAttributeValue)) {
//               args = lxAttributeValue;
//            }
//            else args = lxMessage;
//         }
//         else if (args.compare(lxAttributeValue, lxMethodParameter)) {
//         }
//         else break;
//
//         args = args.nextNode();
//      }
//
//      node = lxClassMethod;
//
////      // !! HOTFIX : the node should be once again found
////      current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
//
//      if (current == lxExpression)
//         current = lxReturning;
//
//      return true;
//   }
//   return false;
//}
//
//void DerivationTransformer :: recognizeScopeMembers(SNode& node, DerivationScope& scope, int mode)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxAttribute) {
//      }
//      else if (current == lxScope) {
//         if (!recognizeMethodScope(current)) {
//            // recognize the field template if available
//            SNode fieldTemplate = current.findChild(lxBaseParent);
//            if (fieldTemplate != lxNone) {
//               current = lxFieldTemplate;
//            }
//            else if (setIdentifier(current)) {
//               if (current.prevNode().firstChild(lxTerminalMask) == lxMemberIdentifier) {
//                  current = lxFieldInit;
//               }
//               else {
//                  current = lxClassField;
//
////                  if (scope.type == DerivationScope::ttMethodTemplate)
////                     current.setArgument(V_METHOD);
//               }
//            }
//            else scope.raiseError(errInvalidSyntax, current);
//         }
//      }
//      else if (current == lxExpression && mode == MODE_ROOT) {
//         node = lxSymbol;
//      }
//      else if (current == lxAttributeDecl && test(mode, MODE_ROOT)) {
//         node.set(lxAttributeDecl, scope.mapAttribute(current));
//      }
//      else if (current == lxBaseParent && test(mode, MODE_ROOT)) {
//      }
//      else if (current == lxCode && test(mode, MODE_CODETEMPLATE)) {
//      }
//      else if (current.compare(lxMethodParameter, lxAttributeValue, lxCode) && mode == MODE_ROOT) {
//         // HOTFIX : recognize returning expression
//         SNode body = node.findChild(lxCode, lxExpression, lxDispatchCode/*, lxReturning*/, lxResendExpression);
//         if (body == lxExpression)
//            body = lxReturning;
//
//         // one method class declaration
//         node.injectNode(lxClassMethod, V_ACTION);
//
//         node.set(lxClass, (ref_t)-2);
//
//
//
//         current.refresh();
//      }
//      else scope.raiseError(errInvalidSyntax, current);
//
//      current = current.nextNode();
//   }
//   
//   if (node == lxScope) {
//      // otherwise it will be compiled as a class
//      node = lxClass;
//   }   
//}
//
//void DerivationTransformer :: saveTemplate(SNode node, DerivationScope& scope, DerivationScope::Type type, SyntaxTree& autogenerated, ref_t templateRef)
//{
//   _Module* module = scope.compilerScope->module;
//
//   _Memory* target = module->mapSection(templateRef | mskSyntaxTreeRef, false);
//
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
//
//   SyntaxTree::saveNode(tree.readRoot(), target);
//}
//
//void DerivationTransformer :: generate(SyntaxWriter& writer, SNode node, _CompilerScope& scope, ident_t sourcePath, ident_t ns,
//   IdentifierList* imports, SyntaxTree& autogenerated)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      switch (current) {
//         case lxClass:
//         {
//            DerivationScope rootScope(&scope, sourcePath, ns, imports);
//            rootScope.autogeneratedTree = &autogenerated;
//
//            generateClassTree(writer, current, rootScope);
//            break;
//         }
//         case lxSymbol:
//         {
//            DerivationScope rootScope(&scope, sourcePath, ns, imports);
//            rootScope.autogeneratedTree = &autogenerated;
//
//            if (!generateSingletonScope(writer, current, rootScope)) {
//               generateSymbolTree(writer, current, rootScope);
//            }            
//            break;
//         }
//         case lxTemplate:
//         {
//            DerivationScope rootScope(&scope, sourcePath, ns, imports);
//            rootScope.autogeneratedTree = &autogenerated;
//
//            generateTemplateScope(current, rootScope);
//            break;
//         }
//         case lxType:
//         {
//            DerivationScope rootScope(&scope, sourcePath, ns, imports);
//            rootScope.autogeneratedTree = &autogenerated;
//            
//            declareType(current, rootScope);
//            break;
//         }      
//      }
//      current = current.nextNode();
//   }
//}
//
//void DerivationTransformer :: generate(SyntaxWriter& writer, _CompilerScope& scope, ident_t sourcePath, ident_t ns, IdentifierList* imports)
//{
//   SyntaxTree autogeneratedTree;
//
//   generate(writer, _root.firstChild(), scope, sourcePath, ns, imports, autogeneratedTree);
//
//   SyntaxTree::moveNodes(writer, autogeneratedTree, lxClass);
//}
//
//void DerivationTransformer :: recognizeRootScope(SNode node, DerivationScope& scope)
//{
//   // try to recognize general declaration
//   if (!recognizeDeclaration(node, scope)) {
//      recognizeScopeMembers(node, scope, MODE_ROOT);
//
//      if (node == lxAttributeDecl) {
//         declareAttribute(node, scope);
//      }
//      else recognizeRootAttributes(node, scope);
//   }
//}
//
//void DerivationTransformer :: recognize(_CompilerScope& scope, SNode node, ident_t sourcePath, ident_t ns, IdentifierList* imports)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxScope) {
//         DerivationScope rootScope(&scope, sourcePath, ns, imports);
//         setIdentifier(current);
//
//         recognizeRootScope(current, rootScope);
//      }
//      current = current.nextNode();
//   }
//}
//
//void DerivationTransformer :: recognize(_CompilerScope& scope, ident_t sourcePath, ident_t ns, IdentifierList* imports)
//{
//   recognize(scope, _root.firstChild(), sourcePath, ns, imports);
//}
