//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Derivation Tree class implementation
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "derivation.h"
#include "errors.h"

using namespace _ELENA_;

// --- DerivationWriter ---

void DerivationWriter :: writeNode(Symbol symbol)
{
   switch (symbol)
   {
      case nsToken:
         _writer.newNode(lxAttribute);
         break;
      case nsExpression:
      case nsRootExpression:
         _writer.newNode(lxExpression);
         break;
      case nsCodeEnd:
         _writer.newNode(lxEOF);
         break;
      case nsMethodParameter:
         _writer.newNode(lxMethodParameter);
         break;
      case nsMessageParameter:
         _writer.newNode(lxMessageParameter);
         break;
      case nsNestedClass:
         _writer.newNode(lxNestedClass);
         break;
      case nsAssigning:
         _writer.newNode(lxAssigning);
         break;
      case nsResendExpression:
         _writer.newNode(lxResendExpression);
         break;
      case nsObject:
         _writer.newNode(lxObject);
         break;
      case nsBaseClass:
         _writer.newNode(lxBaseParent);
         break;
      case nsL0Operation:
      case nsL3Operation:
      case nsL4Operation:
      case nsL5Operation:
      case nsL6Operation:
      case nsL7Operation:
         _writer.newNode(lxOperator);
         break;
      case nsMessageOperation:
         _writer.newNode(lxMessage);
         break;
      case nsSubjectArg:
         _writer.newNode(lxMessage, -1);
         break;
      case nsRootMessage:
         _writer.newNode(lxMessage, -2);
         break;
      case nsLazyExpression:
         _writer.newNode(lxLazyExpression);
         break;
      case nsRetStatement:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask | lxExprMask));
         break;
      case nsMessageReference:
         _writer.newNode(lxMessageReference);
         break;
      case nsSizeValue:
         _writer.newNode(lxSize);
         break;
      case nsSwitching:
         _writer.newNode(lxSwitching);
         break;
      case nsSubCode:
      case nsScope:
      case nsTemplate:
      case nsTokenParam:
//      case nsSubject:
      case nsDispatchExpression:
      case nsExtension:
      case nsCatchMessageOperation:
      case nsAltMessageOperation:
      case nsSwitchOption:
      case nsLastSwitchOption:
//      case nsBiggerSwitchOption:
//      case nsLessSwitchOption:
//      case nsInlineClosure:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask));
         break;
      case nsAttribute:
         _writer.newNode(lxAttributeDecl);
         break;
      default:
         _writer.newNode((LexicalType)symbol);
         break;
   }   
}

void DerivationWriter :: writeSymbol(Symbol symbol)
{
   if (symbol != nsNone) {
      writeNode(symbol);
   }
   else _writer.closeNode();
}

void DerivationWriter :: writeTerminal(TerminalInfo& terminal)
{
   // HOT FIX : if there are several constants e.g. $10$13, it should be treated like literal terminal
   if (terminal == tsCharacter && terminal.value.findSubStr(1, '$', terminal.length, NOTFOUND_POS) != NOTFOUND_POS) {
      terminal.symbol = tsLiteral;
   }

//   if (terminal.symbol == tsAttribute) {
//      _writer.newNode(lxExplicitAttr);
//   }
   /*else */_writer.newNode((LexicalType)(terminal.symbol & ~mskAnySymbolMask | lxTerminalMask | lxObjectMask));

   if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
      // try to use local storage if the quote is not too big
      if (getlength(terminal.value) < 0x100) {
         QuoteTemplate<IdentifierString> quote(terminal.value);
      
         _writer.appendNode(lxTerminal, quote.ident());
      }
      else {
         QuoteTemplate<DynamicString<char> > quote(terminal.value);
      
         _writer.appendNode(lxTerminal, quote.ident());
      }
   }
   else _writer.appendNode(lxTerminal, terminal.value);

   _writer.appendNode(lxCol, terminal.col);
   _writer.appendNode(lxRow, terminal.row);
   _writer.appendNode(lxLength, terminal.length);
   //   _writer->writeDWord(terminal.disp);

   _writer.closeNode();
}

// --- DerivationReader ---

inline void copyIdentifier(SyntaxWriter& writer, SNode ident)
{
   if (emptystr(ident.identifier())) {
      SNode terminal = ident.findChild(lxTerminal);
      if (terminal != lxNone) {
         writer.newNode(ident.type, terminal.identifier());
      }
      else writer.newNode(ident.type);
   }
   else writer.newNode(ident.type, ident.identifier());

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);
   writer.closeNode();
}

inline SNode findLastAttribute(SNode current)
{
   SNode lastAttribute;
   while (current == lxAttribute) {
      lastAttribute = current;
      current = current.nextNode();
   }

   return lastAttribute;
}

inline bool setIdentifier(SNode current)
{
   SNode lastAttribute = findLastAttribute(current);

   if (lastAttribute == lxAttribute) {
      lastAttribute = lxNameAttr;

      current.refresh();

      return true;
   }
   else return false;
}

inline bool isTerminal(LexicalType type)
{
   switch (type)
   {
      case lxIdentifier:
      case lxPrivate:
      case lxInteger:
      case lxHexInteger:
      case lxLong:
      case lxReal:
      case lxLiteral:
      case lxReference:
      case lxCharacter:
      case lxWide:
      case lxExplicitConst:
         return true;
      default:
         return false;
   }
}

inline SNode goToNode(SNode current, LexicalType type)
{
   while (current != lxNone && current != type)
      current = current.nextNode();

   return current;
}

// --- DerivationReader::DerivationScope ---

ref_t DerivationReader::DerivationScope :: mapAttribute(SNode attribute/*, int& attrValue*/)
{   
   SNode terminal = attribute.firstChild(lxTerminalMask);
   if (terminal == lxNone)
      terminal = attribute;

//   int index = mapAttribute(terminal);
//   if (index) {
//      attrValue = index;
//
//      return INVALID_REF;
//   }
   /*else */return moduleScope->mapAttribute(attribute/*, attrValue*/);
}

//bool Compiler::TemplateScope :: isAttribute(SNode terminal)
//{
//   int dummy = 0;
//   ref_t ref = mapAttribute(terminal, dummy);
//
//   return (ref != 0 && isPrimitiveRef(moduleScope->subjectHints.get(ref)));
//}

bool DerivationReader::DerivationScope :: isTypeAttribute(SNode terminal)
{
   ident_t name = terminal.identifier();
   if (emptystr(name))
      name = terminal.findChild(lxTerminal).identifier();

   if (moduleScope->typeHints.exist(name)) {
      return true;
   }
   else return false;
}

ref_t DerivationReader::DerivationScope :: mapTypeAttribute(SNode terminal)
{
   ident_t name = terminal.identifier();
   if (emptystr(name))
      name = terminal.findChild(lxTerminal).identifier();

   return moduleScope->typeHints.get(name);
}

//ref_t Compiler::TemplateScope :: mapTemplate(SNode terminal, int prefixCounter)
//{
//   int paramCounter = SyntaxTree::countChild(terminal, lxAttributeValue);
//   IdentifierString attrName(terminal.findChild(lxIdentifier).findChild(lxTerminal).identifier());
//   if (prefixCounter != 0) {
//      attrName.append('#');
//   }
//   attrName.append('#');
//   attrName.appendInt(paramCounter + prefixCounter);
//   
//   return moduleScope->resolveAttributeRef(attrName, false);
//}
//
//void Compiler::TemplateScope :: loadAttributeValues(SNode attributes, bool prefixMode)
//{
//   SNode current = attributes;
//   // load template parameters
//   while (current != lxNone) {
//      if (current == lxAttributeValue) {
//         SNode terminalNode = current.firstChild(lxObjectMask);
//         ref_t subject = mapSubject(terminalNode);
//         if (subject == 0) {
//            ident_t identifier = terminalNode.identifier();
//            if (emptystr(identifier))
//               identifier = terminalNode.findChild(lxTerminal).identifier();
//
//            subject = moduleScope->module->mapSubject(identifier, false);
//         }
//
//         subjects.add(subjects.Count() + 1, subject);
//      }
//      else if (current == lxTypeAttr) {
//         ref_t subject = subject = moduleScope->module->mapSubject(current.identifier(), false);
//
//         subjects.add(subjects.Count() + 1, subject);
//      }
//      else if (current == lxTemplateType) {
//         TemplateScope* parentTemplate = (TemplateScope*)parent;
//         ref_t subject = parentTemplate->subjects.get(current.argument);
//
//         subjects.add(subjects.Count() + 1, subject);
//      }
//      else if (prefixMode && current == lxNameAttr)
//         break;
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler::TemplateScope :: loadParameters(SNode node)
//{
//   SNode current = node.firstChild();
//   // load template parameters
//   while (current != lxNone) {
//      if (current == lxBaseParent) {               
//         ident_t name = current.firstChild(lxTerminalMask).findChild(lxTerminal).identifier();
//
//         parameters.add(name, parameters.Count() + 1);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler::TemplateScope :: loadFields(SNode node)
//{
//   SNode current = node.firstChild();
//   // load template parameters
//   while (current != lxNone) {
//      if (current == lxAttributeValue) {
//         ident_t name = current.firstChild(lxTerminalMask).findChild(lxTerminal).identifier();
//
//         fields.add(name, parameters.Count() + 1);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//int Compiler::TemplateScope :: mapAttribute(SNode terminal)
//{
//   ident_t identifier = terminal.identifier();
//   if (emptystr(identifier))
//      identifier = terminal.findChild(lxTerminal).identifier();
//
//   int index = parameters.get(identifier);
//   if (!index) {
//      if (parent != NULL) {
//         return ((TemplateScope*)parent)->mapAttribute(terminal);
//      }
//      else return 0;
//   }
//   else return index;
//}
//
//int Compiler::TemplateScope :: mapIdentifier(SNode terminal)
//{
//   ident_t identifier = terminal.identifier();
//   if (emptystr(identifier))
//      identifier = terminal.findChild(lxTerminal).identifier();
//
//   if (type == TemplateScope::ttFieldTemplate) {
//      return fields.get(identifier);
//   }
//   else if (type == TemplateScope::ttCodeTemplate) {
//      return parameters.get(identifier);
//   }
//   else return 0;
//}

void DerivationReader::DerivationScope :: copySubject(SyntaxWriter& writer, SNode terminal)
{
   /*int index = mapAttribute(terminal);
   if (index) {
      writer.newNode(lxTemplateParam, index);
      copyIdentifier(writer, terminal);
      writer.closeNode();
   }
   else */copyIdentifier(writer, terminal);
}

void DerivationReader::DerivationScope :: copyIdentifier(SyntaxWriter& writer, SNode terminal)
{
   ::copyIdentifier(writer, terminal);
}

//bool Compiler::TemplateScope :: generateClassName()
//{
//   ReferenceNs name;
//   name.copy(moduleScope->module->Name());
//   name.combine(moduleScope->module->resolveSubject(templateRef));
//
//   SubjectMap::Iterator it = subjects.start();
//   while (!it.Eof()) {
//      name.append('@');
//      name.append(moduleScope->module->resolveSubject(*it));
//
//      it++;
//   }
//
//   reference = moduleScope->module->mapReference(name, true);
//   if (!reference) {
//      reference = moduleScope->module->mapReference(name, false);
//
//      return true;
//   }
//   else return false;
//}

void DerivationReader :: generateAttributes(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   SNode current = attributes;
   while (current == lxAttribute || current == lxAttributeDecl) {
      ref_t attrRef = current.argument;
      if (!attrRef)
         attrRef = scope.mapAttribute(current);

      if (attrRef != 0) {
         writer.newNode(lxAttribute, attrRef);
         copyIdentifier(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      else if (scope.isTypeAttribute(current.findChild(lxIdentifier, lxPrivate))) {
         ref_t classRef = scope.mapTypeAttribute(current.findChild(lxIdentifier, lxPrivate));

         writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(classRef));
         copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }
      //   else if (attrRef == INVALID_REF) {
   //      writer.appendNode(lxTemplateType, attrValue);
   //   }
   //   else if (attrRef != 0) {
   //      ref_t classRef = scope.moduleScope->subjectHints.get(attrRef);
   //      if (classRef == INVALID_REF) {
   //            TemplateScope templateScope(&scope, attrRef);
   //            if (scope.type == TemplateScope::ttAttrTemplate)
   //               templateScope.type = scope.type;

   //            copyTemplateTree(writer, current, templateScope, current.firstChild());
   //      }
   //      else if (_logic->isPrimitiveRef(classRef)) {
   //         writer.appendNode(lxAttribute, classRef);
   //      }
   //      else {
   //         writer.appendNode(lxTypeAttr, scope.moduleScope->module->resolveSubject(attrRef));
   //      }            
   //   }
      else scope.raiseError(errInvalidHint, current);

      current = current.nextNode();
   }

   //if (current == lxNameAttr && current.existChild(lxSize)) {
   //   SNode sizeNode = current.findChild(lxSize).findChild(lxInteger, lxHexInteger);
   //   if (sizeNode != lxNone) {
   //      writer.appendNode(lxAttribute, readSizeValue(sizeNode, sizeNode == lxHexInteger ? 16 : 10));
   //   }
   //   else scope.raiseError(errInvalidHint, current);
   //}

   if (node != lxNone) {
      SNode nameNode = current == lxNameAttr ? current.findChild(lxIdentifier, lxPrivate) : node.findChild(lxIdentifier, lxPrivate);
      if (nameNode != lxNone)
         scope.copySubject(writer, nameNode);
   }
}

void DerivationReader:: generateMessageTree(SyntaxWriter& writer, SNode node, DerivationScope& scope/*, bool operationMode*/)
{
//   if (operationMode)
//      writer.newBookmark();

   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         //case lxObject:
         case lxMessageParameter:
            generateExpressionTree(writer, current, scope, EXPRESSION_MESSAGE_MODE);
            break;
         case lxExpression:
            generateExpressionTree(writer, current, scope, EXPRESSION_EXPLICIT_MODE | EXPRESSION_MESSAGE_MODE);
            break;
         //case lxExtern:
         //   generateCodeTree(writer, current, scope);
         //   break;
         //case lxCode:
         //   generateCodeTree(writer, current, scope);
         //   if (scope.type == TemplateScope::ttCodeTemplate) {
         //      if (scope.parameters.Count() == 2) {
         //         if (scope.codeNode == lxNone) {
         //            writer.insert(lxTemplateParam);
         //            scope.codeNode = current;
         //         }
         //         else {
         //            writer.insert(lxTemplateParam, 3);
         //            scope.codeNode = SNode();
         //         }
         //      }
         //      else writer.insert(lxTemplateParam);
         //      writer.closeNode();
         //   }

         //   writer.insert(lxExpression);
         //   writer.closeNode();
         //   break;
         case lxMessage:
            writer.newNode(lxMessage);
            scope.copySubject(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
            break;
         case lxIdentifier:
         case lxPrivate:
         case lxReference:
            writer.newNode(lxMessage);
            scope.copySubject(writer, current);
            writer.closeNode();
            break;
      }
      current = current.nextNode();
   }
   
//   if (operationMode) {
//      writer.removeBookmark();
//   }
}

void DerivationReader :: generateObjectTree(SyntaxWriter& writer, SNode current, DerivationScope& scope/*, int mode*/)
{
   switch (current.type) {
//      case lxAssigning:
//         writer.appendNode(lxAssign);
//         generateExpressionTree(writer, current, scope, 0);
//         break;
//      case lxSwitching:
//         generateSwitchTree(writer, current, scope);
//         writer.insert(lxSwitching);
//         writer.closeNode();
//         writer.insert(lxExpression);
//         writer.closeNode();
//         break;
//      case lxOperator:
//         copyOperator(writer, current.firstChild());
//         generateExpressionTree(writer, current, scope, 0);
//         writer.insert(lxExpression);
//         writer.closeNode();
//         break;
//      case lxCatchOperation:
//      case lxAltOperation:
//         if (scope.type == TemplateScope::ttCodeTemplate && scope.templateRef == 0) {
//            // HOTFIX : for try-catch template
//            scope.codeNode = SNode();
//         }
//         writer.newBookmark();
      case lxMessage:
//         if (current.argument == -1 && current.nextNode() == lxMethodParameter) {
//            writer.newNode(lxClosureMessage);
//            copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
//            writer.closeNode();
//         }
//         else {
            generateMessageTree(writer, current, scope/*, false*/);

            writer.insert(lxExpression);
            writer.closeNode();
//         }
//         if (current == lxCatchOperation) {
//            writer.removeBookmark();
//            writer.insert(lxTrying);
//            writer.closeNode();
//         }
//         else if (current == lxAltOperation) {
//            writer.removeBookmark();
//            writer.insert(lxAlt);
//            writer.closeNode();
//         }
         break;
//      case lxExtension:
//         writer.newNode(current.type, current.argument);
//         generateExpressionTree(writer, current, scope, 0);
//         writer.closeNode();
//         break;
      case lxExpression:
         generateExpressionTree(writer, current, scope, 0);
         break;
      case lxMessageReference:
      case lxLazyExpression:
         writer.newNode(lxExpression);
         writer.newNode(current.type);
         if (current == lxLazyExpression) {
            generateExpressionTree(writer, current, scope, 0);
         }
         else copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate, lxLiteral));
         writer.closeNode();
         writer.closeNode();
         break;
      case lxObject:
         generateExpressionTree(writer, current, scope, 0);
         break;
//      case lxNestedClass:
//         if (scope.type == TemplateScope::ttCodeTemplate) {
//            writer.insert(lxTemplateParam, 2);
//            writer.closeNode();
//         }
//         else {
//            generateScopeMembers(current, scope);
//
//            generateClassTree(writer, current, scope, SNode(), -1);
//         }
//         writer.insert(lxExpression);
//         writer.closeNode();
//         break;
      case lxCode:
         generateCodeTree(writer, current, scope);
         /*if (scope.type == TemplateScope::ttCodeTemplate) {
            if (scope.parameters.Count() == 2) {
               if (scope.codeNode == lxNone) {
                  writer.insert(lxTemplateParam);
                  scope.codeNode = current;
               }
               else {
                  writer.insert(lxTemplateParam, 3);
                  scope.codeNode = SNode();
               }
            }
            else writer.insert(lxTemplateParam);
            writer.closeNode();
         }*/
         writer.insert(lxExpression);
         writer.closeNode();
         break;
//      case lxMethodParameter:
//         writer.newNode(lxMethodParameter);
//         copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
//         writer.closeNode();
//         break;
      default:
      {
         if (isTerminal(current.type)) {
//            if (scope.type == TemplateScope::ttFieldTemplate) {
//               int index = scope.mapIdentifier(current);
//               if (index != 0) {
//                  writer.newNode(lxTemplateField, index);
//                  copyIdentifier(writer, current);
//                  writer.closeNode();
//               }
//               else copyIdentifier(writer, current);
//            }
//            else if (scope.type == TemplateScope::ttCodeTemplate && scope.mapIdentifier(current)) {
//               writer.newNode(lxTemplateParam, 1);
//               copyIdentifier(writer, current);
//               writer.closeNode();
//            }
            /*else */copyIdentifier(writer, current);
         }               
         break;
      }
   }
   //
   //   if (mode == 1 && exprCounter > 1) {
   //      _writer.insert(lxExpression);
   //      _writer.closeNode();
   //   }
}

void DerivationReader :: generateExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, int mode)
{
   writer.newBookmark();

   SNode current = node.firstChild();
//   bool identifierMode = current.type == lxIdentifier;
//   bool listMode = false;
//   // check if it is new operator
//   if (identifierMode && current.nextNode() == lxExpression && current.nextNode().nextNode() != lxExpression) {
//      scope.copySubject(writer, current);
//
//      writer.appendNode(lxOperator, -1);
//      generateExpressionTree(writer, current.nextNode(), scope, 0);
//      writer.insert(lxExpression);
//      writer.closeNode();
//   }
//   else {
      while (current != lxNone) {
         if (current == lxExpression) {
//            if (current.nextNode() == lxExpression && !test(mode, EXPRESSION_MESSAGE_MODE))
//               listMode = true;
//
//            if (identifierMode) {
//               generateExpressionTree(writer, current, scope, listMode ? EXPRESSION_EXPLICIT_MODE : 0);
//            }
//            else if (listMode) {
//               generateExpressionTree(writer, current, scope, EXPRESSION_EXPLICIT_MODE);
//            }
            /*else */generateObjectTree(writer, current, scope);
         }
//         else if (listMode && (current == lxMessage || current == lxOperator)) {
//            // HOTFIX : if it is an operation with a collection
//            listMode = false;
//            writer.insert(lxExpression);
//            writer.closeNode();
//
//            generateObjectTree(writer, current, scope);
//         }
         else generateObjectTree(writer, current, scope);

         current = current.nextNode();
      }
//   }
//
//   if (listMode) {
//      writer.insert(lxExpression);
//      writer.closeNode();
//   }

   if (test(mode, EXPRESSION_EXPLICIT_MODE)) {
      writer.insert(node.type);
      writer.closeNode();
   }

   writer.removeBookmark();
}

void DerivationReader:: generateSymbolTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   writer.newNode(lxSymbol);

   generateAttributes(writer, node, scope, attributes);

   generateExpressionTree(writer, node.findChild(lxExpression), scope);

   writer.closeNode();
}

void DerivationReader :: generateCodeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   writer.newNode(node.type, node.argument);

   bool withBreakpoint = (node == lxReturning || node == lxResendExpression);
   if (withBreakpoint)
      writer.newBookmark();

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression || current == lxReturning) {
         //if (current.existChild(lxAssigning)) {
         //   generateVariableTree(writer, current, scope);
         //}
         //else if (isArrayDeclaration(current)) {
         //   generateArrayVariableTree(writer, current, scope);
         //}
         //else if (isTemplateDeclaration(current)) {
         //   generateTemplateVariableTree(writer, current, scope, scope.reference == INVALID_REF);
         //}
         //else if (current.existChild(lxCode) || current.existChild(lxNestedClass)) {
         //   generateCodeTemplateTree(writer, current, scope);
         //}
         /*else */generateExpressionTree(writer, current, scope);
      }
      else if (current == lxEOF) {
         writer.newNode(lxEOF);

         SNode terminal = current.firstChild();
         SyntaxTree::copyNode(writer, lxRow, terminal);
         SyntaxTree::copyNode(writer, lxCol, terminal);
         SyntaxTree::copyNode(writer, lxLength, terminal);
         writer.closeNode();
      }
      else if (current == lxLoop || current == lxCode || current == lxExtern) {
         generateCodeTree(writer, current, scope);
      }
      else generateObjectTree(writer, current, scope);

      current = current.nextNode();
   }

   if (withBreakpoint)
      writer.removeBookmark();

   writer.closeNode();
}

void DerivationReader :: generateMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode)
{
   SyntaxWriter bufferWriter(buffer);

   writer.newNode(lxClassMethod);
   //if (templateMode) {
   //   writer.appendNode(lxSourcePath, scope.sourcePath);
   //   writer.appendNode(lxTemplate, scope.templateRef);
   //}      

   /*if (node == lxDefaultGeneric) {
      if (node.existChild(lxMethodParameter)) {
         writer.appendNode(lxIdentifier, EVAL_MESSAGE);
      }
      else writer.appendNode(lxIdentifier, GET_MESSAGE);

      writer.appendNode(lxAttribute, V_SEALED);
      writer.appendNode(lxAttribute, V_GENERIC);
   }
   else */generateAttributes(bufferWriter, node, scope, attributes/*, false, true*/);

   // copy attributes
   SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxTypeAttr, lxClassRefAttr, lxTemplateType);

   // copy method signature
   SNode current = goToNode(attributes, lxNameAttr);
   while (current == lxNameAttr || current == lxMessage) {
      if (current == lxMessage) {
         writer.newNode(lxMessage);
         scope.copySubject(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }

      current = current.nextNode();
   }

   // copy method arguments
   current = node.firstChild();
   while (current != lxNone) {
      if (current == lxMethodParameter || current == lxMessage) {
         writer.newNode(current.type, current.argument);
         if (current == lxMessage) {
            scope.copySubject(writer, current.firstChild(lxTerminalMask));
         }
         else copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }
      else if (current == lxAttributeValue) {
         // if it is an explicit type declaration
         ref_t ref = scope.moduleScope->mapTerminal(current.findChild(lxIdentifier, lxReference));
         writer.newNode(lxParamRefAttr, scope.moduleScope->module->resolveReference(ref));
         copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }

      current = current.nextNode();
   }

   SNode bodyNode = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (bodyNode != lxNone) {
      //if (templateMode)
      //   scope.reference = INVALID_REF;

      generateCodeTree(writer, bodyNode, scope);
   }

   writer.closeNode();

   // copy methods
   SyntaxTree::moveNodes(writer, buffer, lxClassMethod);
}


bool DerivationReader :: declareType(SNode node, DerivationScope& scope, SNode attributes)
{
   SNode expr = node.findChild(lxExpression);
   if (expr == lxExpression) {
      expr = expr.findChild(lxObject);
      if (expr != lxObject || expr.nextNode() != lxNone)
         return false;
   }

   SNode nameNode = goToNode(attributes, lxNameAttr).findChild(lxIdentifier, lxPrivate);

   bool internalSubject = nameNode == lxPrivate;
   bool invalid = true;

   // map a full type name
   ident_t typeName = nameNode.findChild(lxTerminal).identifier();
   ref_t classRef = 0;

   SNode classNode = expr.findChild(lxIdentifier, lxPrivate, lxReference);
   if (classNode != lxNone && classNode.nextNode(lxObjectMask) == lxNone) {
      //      SNode option = classNode.findChild(lxAttributeValue);
      //      if (option != lxNone) {
      //         TemplateScope templateScope(&scope);
      //         templateScope.templateRef = templateScope.mapTemplate(classNode);
      //         if (templateScope.templateRef != 0) {
      //            classRef = scope.subjectHints.get(templateScope.templateRef);
      //            if (classRef == INVALID_REF) {
      //               templateScope.loadAttributeValues(classNode.firstChild()/*, false*/);
      //               templateScope.autogeneratedTree = &autogenerated;
      //
      //               SyntaxTree buffer;
      //               SyntaxWriter bufferWriter(buffer);
      //               if (generateTemplate(bufferWriter, templateScope, true/*, true*/)) {
      //                  //SyntaxWriter writer(autogenerated);
      //      
      //                  SyntaxTree::moveNodes(writer, buffer);
      //      
      //                  classRef = templateScope.reference;
      //      
      //                  invalid = false;
      //               }
      //            }
      //         }
      //      }
      //      else {
      classRef = scope.moduleScope->mapTerminal(classNode);

      invalid = false;
      //      }
   }

   if (!invalid) {
      scope.moduleScope->saveType(typeName, classRef, internalSubject);
   }

   return !invalid;
}

bool DerivationReader :: generateDeclaration(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
//   ModuleScope* moduleScope = scope.moduleScope;
//
//   SyntaxTree tree;
//   SyntaxWriter bufferWriter(tree);
//
//   bufferWriter.newNode(lxTemplate);
//
//   scope.type = TemplateScope::ttAttrTemplate;
//   generateAttributes(bufferWriter, node, scope, attributes/*, false*/);
//
//   bufferWriter.closeNode();
//
   // recognize the declaration type
   DeclarationAttr declType = daNone;
   SNode current = attributes;
   while (current == lxAttribute || current == lxAttributeDecl) {
      ref_t attrRef = current.argument;
      if (!attrRef) {
         attrRef = scope.mapAttribute(current);

         current.setArgument(attrRef);

         DeclarationAttr attr = daNone;
         switch (attrRef) {
            case V_TYPETEMPL:
               attr = daType;
               break;
            //case V_CLASS:
            //case V_STRUCT:
            //case V_STRING:
            //   attr = daClass;
            //   break;
            //case V_TEMPLATE:
            //   attr = daTemplate;
            //   break;
            //case V_FIELD:
            //   attr = daField;
            //   break;
            //case V_METHOD:
            //   attr = daMethod;
            //   break;
            //case V_LOOP:
            //   attr = daLoop;
            //   break;
            //case V_IMPORT:
            //   attr = daImport;
            //   break;
            //case V_EXTERN:
            //   attr = daExtern;
            //   break;
            default:
               break;
         }         
         declType = (DeclarationAttr)(declType | attr);
      }

      current = current.nextNode();
   }

   if (declType == daType) {
      return declareType(node, scope, attributes);
   }
//   if (declType == daImport) {
//      SNode name = goToNode(attributes, lxNameAttr);
//
//      compileIncludeModule(name, *scope.moduleScope);
//
//      return true;
//   }
//   else if (test(declType, daTemplate)) {
//      node = lxTemplate;
//
//      SNode name = goToNode(attributes, lxNameAttr);
//
//      int count = SyntaxTree::countChild(node, lxBaseParent);
//
//      IdentifierString templateName(name.findChild(lxIdentifier).findChild(lxTerminal).identifier());
//      if (test(declType, daField)) {
//         templateName.append("#");
//
//         scope.type = TemplateScope::Type::ttFieldTemplate;
//         count++; // HOTFIX : to include the field itself
//      }
//      else if (test(declType, daMethod)) {
//         templateName.append("#");
//
//         scope.type = TemplateScope::ttMethodTemplate;
//      }
//      else if (node.existChild(lxExpression)) {
//         scope.type = TemplateScope::ttCodeTemplate;
//
//         // HOTFIX : mark the expression as a code
//         SNode exprNode = node.findChild(lxExpression);
//         exprNode.injectNode(lxExpression);
//         if (test(declType, daLoop)) {
//            exprNode.injectNode(lxLoop);
//         }
//         else if (test(declType, daExtern)) {
//            exprNode.injectNode(lxExtern);
//         }
//
//         exprNode = lxCode;
//      }
//
//      templateName.append('#');
//      templateName.appendInt(count);
//
//      ref_t templateRef = moduleScope->mapNewSubject(templateName);
//
//      // check for duplicate declaration
//      if (moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, true))
//         scope.raiseError(errDuplicatedSymbol, name);
//
//      saveTemplate(moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, false), 
//         node, *moduleScope, attributes, scope.type/*, autogenerated*/);
//
//      moduleScope->saveSubject(templateRef, INVALID_REF, false);
//
//      return true;
//   }
//   else if (test(declType, daClass)) {
//      return false;
//   }
   /*else */return false;
}

bool DerivationReader :: generateSingletonScope(SyntaxWriter& writer, SNode node, /*TemplateScope& scope, */SNode attributes)
{
//   SNode expr = node.findChild(lxExpression);
//   SNode object = expr.findChild(lxObject);
//   SNode closureNode = object.findChild(lxNestedClass);
//   if (closureNode != lxNone && isSingleStatement(expr)) {
//      generateScopeMembers(closureNode, scope);
//
//      SNode terminal = object.firstChild(lxTerminalMask);
//
//      writer.newNode(lxClass);
//      writer.appendNode(lxAttribute, V_SINGLETON);
//
//      if (terminal != lxNone) {
//         writer.newNode(lxBaseParent);
//         copyIdentifier(writer, terminal);
//         writer.closeNode();
//      }
//
//      generateAttributes(writer, node, scope, attributes/*, false*/);
//
//      // NOTE : generateClassTree closes the class node and copies auto generated classes after it
//      generateClassTree(writer, closureNode, scope, SNode(), -2);
//
//      return true;
//   }
   /*else */return false;
}

bool DerivationReader :: generateMethodScope(SNode node, DerivationScope& scope, SNode attributes)
{
   SNode current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (current != lxNone) {
      // try to resolve the message name
      SNode lastAttr = findLastAttribute(attributes);

      if (node.firstChild(lxExprMask) == lxMethodParameter) {
         // HOTFIX : recognize type
         current = lastAttr.prevNode();
         if (current == lxAttribute && scope.isTypeAttribute(lastAttr.findChild(lxIdentifier, lxPrivate))) {
            lastAttr = lxMessage;
            lastAttr = current;
         }

      }

      //// HOTFIX : recognize generic attribute      
      //if (!scope.isAttribute(lastAttr)) {
      //   bool isVerb = _verbs.exist(lastAttr.findChild(lxIdentifier).findChild(lxTerminal).identifier());

      //   

         // mark the last message as a name
         lastAttr = lxNameAttr;
      //}
      
      node = lxClassMethod;

      // !! HOTFIX : the node should be once again found
      current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);

      if (current == lxExpression)
         current = lxReturning;

      return true;
   }
   return false;
}

void DerivationReader :: generateClassTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, int nested)
{
   SyntaxTree buffer;

   if (!nested) {
      writer.newNode(lxClass);

      generateAttributes(writer, node, scope, attributes/*, false*/);
   }

   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute || current == lxNameAttr) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
//      else if (current == lxBaseParent) {
//         if (current.existChild(lxAttributeValue)) {
//            ref_t attrRef = scope.mapTemplate(current);
//
//            TemplateScope templateScope(&scope, attrRef);
//            copyTemplateTree(writer, current, templateScope, current.firstChild());
//         }
//         else {
//            writer.newNode(lxBaseParent);
//            copyIdentifier(writer, current.firstChild(lxTerminalMask));
//            writer.closeNode();
//         }
//      }
      else if (current == lxClassMethod) {
         generateMethodTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }
//      else if (current == lxClassField) {
//         if (current.argument == INVALID_REF) {
//            generateFieldTemplateTree(writer, current, scope, subAttributes, buffer);
//         }
//         else generateFieldTree(writer, current, scope, subAttributes, buffer);
//         subAttributes = SNode();
//      }
//      else if (current == lxFieldTemplate) {
//         generateFieldTemplateTree(writer, current, scope, subAttributes, buffer);
//         subAttributes = SNode();
//      }

      current = current.nextNode();
   }

   if (nested == -1)
      writer.insert(lxNestedClass);

   writer.closeNode();

//   SyntaxTree::moveNodes(writer, buffer, lxClass);
}

void DerivationReader :: generateScopeMembers(SNode node, DerivationScope& scope)
{
   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxScope) {
         if (!generateMethodScope(current, scope, subAttributes)) {
//            // recognize the field template if available
//            SNode fieldTemplate = current.findChild(lxBaseParent);
//            if (fieldTemplate != lxNone) {
//               current = lxFieldTemplate;
//            }
//            else if (setIdentifier(subAttributes)) {
//               current = lxClassField;
//               
//               if (current.existChild(lxAttributeValue)) {
//                  current.setArgument(-1);
//               }
//            }
         }
         subAttributes = SNode();
      }

      current = current.nextNode();
   }
}

void DerivationReader :: generateScope(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   SNode body = node.findChild(lxExpression, lxAttributeDecl);
   if (body == lxExpression) {
      // if it could be compiled as a symbol
      if (setIdentifier(attributes)) {
         attributes.refresh();

         // check if it is a code template
         if (!generateDeclaration(writer, node, scope, attributes)) {
            // check if it could be compiled as a singleton
            if (!generateSingletonScope(writer, node, /*scope, */attributes)) {
               node = lxSymbol;
               attributes.refresh();

               generateSymbolTree(writer, node, scope, attributes);
            }
         }
      }
   }
   else if (body == lxAttributeDecl) {
      if (setIdentifier(attributes)) {
         attributes.refresh();

         SNode nameAttr = goToNode(attributes, lxNameAttr);
         ident_t name = nameAttr.findChild(lxIdentifier).findChild(lxTerminal).identifier();

         ref_t attrRef = scope.mapAttribute(body);

         scope.moduleScope->saveAttribute(name, attrRef);
      }
   }
   else {
//      // it is is a template
//      if (node == lxTemplate) {
//         generateScopeMembers(node, scope);
//
//         generateTemplateTree(writer, node, scope, attributes);
//      }
//      else {
         setIdentifier(attributes);
//         // try to recognize general declaration
//         if (!generateDeclaration(writer, node, scope, attributes)) {
            // otherwise it will be compiled as a class
            node = lxClass;
            attributes.refresh();

            generateScopeMembers(node, scope);

            generateClassTree(writer, node, scope, attributes);
//         }
//      }
   }
}

void DerivationReader :: generateSyntaxTree(SyntaxWriter& writer, SNode node, _CompilerScope& scope/*, SyntaxTree& autogenerated*/)
{
   SNode attributes;
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxAttribute:
         case lxAttributeDecl:
            if (attributes == lxNone) {
               attributes = current;
            }
            break;
         case lxScope:
         {
            DerivationScope rootScope(&scope);
////            rootScope.templateRef = INVALID_REF;
//            rootScope.autogeneratedTree = &autogenerated;
            generateScope(writer, current, rootScope, attributes);
            attributes = SNode();
            break;
         }
//         case lxTemplate:
//            generateNewAttribute(current, scope, attributes);
//            attributes = SNode();
//            break;
      }
      current = current.nextNode();
   }
}

void DerivationReader :: generateSyntaxTree(SyntaxWriter& writer, _CompilerScope& scope/*, SyntaxTree& autogenerated*/)
{
   //   SyntaxTree autogeneratedTree;
   writer.newNode(lxRoot);
   generateSyntaxTree(writer, _root, scope/*, autogeneratedTree*/);

//   SyntaxTree::moveNodes(writer, autogeneratedTree, lxClass);

   writer.closeNode();
}