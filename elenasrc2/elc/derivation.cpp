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
//#include "bytecode.h"

using namespace _ELENA_;

constexpr auto MODE_ROOT                  = 0x01;
//constexpr auto MODE_PROPERTYALLOWED = 0x40;

constexpr auto MODE_FUNCTION              = -2;
//constexpr auto MODE_COMPLEXMESSAGE  = -3;
constexpr auto MODE_PROPERTYMETHOD        = -4;

constexpr auto EXPRESSION_IMPLICIT_MODE   = 0x1;

// --- DerivationWriter ---

//inline SNode goToLastNode(SNode current)
//{
//   SNode lastOne;
//   while (current != lxNone) {
//      lastOne = current;
//      current = current.nextNode();
//   }      
//
//   return lastOne;
//}

inline SNode goToFirstNode(SNode current)
{
   SNode firstOne = current;
   while (current != lxNone) {
      firstOne = current;
      current = current.prevNode();
   }

   return firstOne;
}

inline SNode goToFirstNode(SNode current, LexicalType type)
{
   SNode firstOne = current;
   while (current != lxNone && current == type) {
      firstOne = current;
      current = current.prevNode();
   }

   return firstOne;
}

inline SNode goToFirstNode(SNode current, LexicalType type1, LexicalType type2)
{
   SNode firstOne = current;
   while (current != lxNone && current.compare(type1, type2)) {
      firstOne = current;
      current = current.prevNode();
   }

   return firstOne;
}

inline SNode goToFirstNode(SNode current, LexicalType type1, LexicalType type2, LexicalType type3)
{
   SNode firstOne = current;
   while (current != lxNone && current.compare(type1, type2, type3)) {
      firstOne = current;
      current = current.prevNode();
   }

   return firstOne;
}

//inline SNode goToNode(SNode current, LexicalType type)
//{
//   while (current != lxNone && current != type)
//      current = current.nextNode();
//
//   return current;
//}
//
//inline SNode goToNode(SNode current, LexicalType type1, LexicalType type2)
//{
//   while (current != lxNone && !current.compare(type1, type2))
//      current = current.nextNode();
//
//   return current;
//}
//
//inline SNode goToNode(SNode current, LexicalType type1, LexicalType type2, LexicalType type3)
//{
//   while (current != lxNone && !current.compare(type1, type2, type3))
//      current = current.nextNode();
//
//   return current;
//}
//
//inline bool isTerminal(LexicalType type)
//{
//   return test(int(type), lxTerminalMask);
//}

inline void copyIdentifier(SyntaxWriter& writer, SNode ident, bool ignoreTerminalInfo)
{
   ident_t s = ident.identifier();
   if (!emptystr(s)) {
      writer.newNode(ident.type, s);
   }
   else writer.newNode(ident.type);

   if (!ignoreTerminalInfo) {
      SyntaxTree::copyNode(writer, lxRow, ident);
      SyntaxTree::copyNode(writer, lxCol, ident);
      SyntaxTree::copyNode(writer, lxLength, ident);
   }

   writer.closeNode();
}

void DerivationWriter :: newNodeDirectly(LexicalType symbol, ident_t arg)
{
   _output.newNode(symbol, arg);
}

void DerivationWriter :: newNodeDirectly(LexicalType symbol)
{
   _output.newNode(symbol);
}

void DerivationWriter :: closeNodeDirectly()
{
   _output.closeNode();
}

DerivationWriter::MetaScope DerivationWriter :: recognizeMetaScope(SNode node)
{
   recognizeScopeAttributes(node.lastChild(), 0);

   MetaScope scopeType = MetaScope::None;

   SNode current = node.firstChild();
   while (current == lxAttribute) {
      switch (current.argument) {
         case V_NAMESPACE:
            scopeType = MetaScope::Namespace;
            break;
         //         case V_TYPETEMPL:
         //            declType = (DeclarationAttr)(declType | daType);
         //            break;
         //         case V_INLINE:
         //            declType = (DeclarationAttr)(declType | daInline);
         //            break;
         //         case V_PROPERTY:
         //            declType = (DeclarationAttr)(declType | daProperty);
         //            break;
         case V_IMPORT:
            scopeType = MetaScope::Import;
            break;
         //         case V_EXTENSION:
         //            declType = (DeclarationAttr)(declType | daExtension);
         //            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   return scopeType;
}

void DerivationWriter :: newNode(LexicalType symbol)
{
   _level++;

   if (symbol == lxScope && _level == 1) {
      _cachingLevel = _level;

      SNode node = _cache.readRoot();
      MetaScope scopeType = recognizeMetaScope(node);
      // the namespace node should be copied directly
      if (scopeType == MetaScope::Namespace) {
         Scope scope;
         declareNestedNamespace(node, scope);
         
         _cachingLevel = _level = 0;

         return;
      }
      else if (scopeType == MetaScope::Import) {
         symbol = lxImport;
      }
      // otherwise it should be cached
   }
   else if (symbol == lxAttributeDecl || symbol == lxStatementDecl) {
      _cachingLevel = _level;
   }

   _cacheWriter.newNode(symbol);
}

void DerivationWriter :: closeNode()
{
   if (_level == 0) {
      _output.closeNode();
   }
   else {
      _level--;

      _cacheWriter.closeNode();
      if (_level < _cachingLevel) {
         _cachingLevel = 0;

         saveScope(_output);

         _cacheWriter.newNode(lxRoot);
      }
   }
}

inline void saveTerminal(SyntaxWriter& writer, TerminalInfo& terminal)
{
   if (terminal.symbol == lxGlobalReference) {
      writer.newNode(terminal.symbol, terminal.value + 1);
   }
   else writer.newNode(terminal.symbol, terminal.value);

   writer.appendNode(lxCol, terminal.col);
   writer.appendNode(lxRow, terminal.row);
   writer.appendNode(lxLength, terminal.length);
   //   _writer->writeDWord(terminal.disp);
   
   writer.closeNode();
}

void DerivationWriter :: appendTerminal(TerminalInfo& terminal)
{
//   // HOT FIX : if there are several constants e.g. $10$13, it should be treated like literal terminal
//   if (terminal == tsCharacter && terminal.value.findSubStr(1, '$', terminal.length, NOTFOUND_POS) != NOTFOUND_POS) {
//      terminal.symbol = tsLiteral;
//   }

//   if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
//      // try to use local storage if the quote is not too big
//      if (getlength(terminal.value) < 0x100) {
//         QuoteTemplate<IdentifierString> quote(terminal.value);
//
//         _cacheWriter.newNode(type, quote.ident());
//      }
//      else {
//         QuoteTemplate<DynamicString<char> > quote(terminal.value);
//
//         _cacheWriter.newNode(type, quote.ident());
//      }
//   }
//   else if (terminal == tsGlobal) {
//      // HOTFIX : skip the leading symbol for the global reference
//      _cacheWriter.newNode(type, terminal.value + 1);
//   }
//   else _cacheWriter.newNode(type, terminal.value);
//
   saveTerminal(_cacheWriter, terminal);

//   _cacheWriter.closeNode();
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
         raiseError(errInvalidSyntax, current);
      
      ident_t name = current.firstChild(lxTerminalMask).identifier();
      
      if(!scope.parameters.add(name, scope.parameters.Count() + 1, true))
         raiseError(errDuplicatedDefinition, current);

      current = current.nextNode();
   }
}

void DerivationWriter :: loadTemplateExprParameters(Scope& scope, SNode node)
{
   SNode current = node.findChild(lxParameter);
   while (current == lxParameter) {
      SNode tokenNode = current.findChild(lxToken);

      if (tokenNode.existChild(lxToken) || tokenNode.nextNode() != lxNone)
         raiseError(errInvalidSyntax, current);

      ident_t name = tokenNode.firstChild(lxTerminalMask).identifier();

      if (!scope.parameters.add(name, scope.parameters.Count() + 1, true))
         raiseError(errDuplicatedDefinition, current);

      current = current.nextNode();
   }
}

void DerivationWriter :: generateTemplateTree(SNode node/*, ScopeType templateType*/)
{
   Scope templateScope;
//   templateScope.templateMode = templateType;
//   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
//      loadTemplateExprParameters(templateScope, node);
//      templateScope.ignoreTerminalInfo = true;
//   }
//
//   loadTemplateParameters(templateScope, nameNode);
   
   //SyntaxTree templateTree;
   //SyntaxWriter templateWriter(templateTree);

   //templateWriter.newNode(lxRoot);
   //
   //templateWriter.closeNode();

   SNode identNode = node.prevNode().firstChild(lxTerminalMask);
   IdentifierString name(identNode.identifier());
   int paramCounter = SyntaxTree::countChild(node.firstChild(), lxToken);
   //switch (templateMode) {
   //case V_INLINE:
   //   name.append("#inline#");
   //   break;
   //case V_PROPERTY:
   //   name.append("#prop#");
   //   break;
   //default:
      name.append('#');
   //   break;
   //}

   name.appendInt(paramCounter);
   //if (codeTemplate) {
   //   int subParamCounter = SyntaxTree::countChild(nameNode.nextNode(), lxParameter);
   //   name.append('#');
   //   name.appendInt(subParamCounter);
   //}

   identNode.setStrArgument(name.c_str());

   _output.newNode(lxTemplate);
   generateScope(_output, _cache.readRoot(), templateScope);
   _output.closeNode();

   //SNode root = templateTree.readRoot();

}

//bool DerivationWriter :: recognizeMetaScope(SNode node)
//{
//   // recognize the declaration type
//   DeclarationAttr declType = daNone;
//   SNode nameNode;
//   SNode current = node.prevNode();
//   if (current == lxNameAttr) {
//      nameNode = current;
//      current = current.prevNode();
//   }      
//   
//   while (current == lxAttribute) {
//      switch (current.argument) {
////         case V_TYPETEMPL:
////            declType = (DeclarationAttr)(declType | daType);
////            break;
////         case V_INLINE:
////            declType = (DeclarationAttr)(declType | daInline);
////            break;
////         case V_PROPERTY:
////            declType = (DeclarationAttr)(declType | daProperty);
////            break;
//         case V_IMPORT:
//            declType = (DeclarationAttr)(declType | daImport);
//            break;
////         case V_EXTENSION:
////            declType = (DeclarationAttr)(declType | daExtension);
////            break;
//         default:
//            break;
//      }
//   
//      current = current.prevNode();
//   }
//   
////   if (nameNode.existChild(lxToken)) {
////      declType = (DeclarationAttr)(declType | daTemplate);
////   }
////
////   if (declType == daType) {
////      node = lxForward;
////
////      return true;
////   }
//   /*else */if (declType == daImport) {
//      SNode name = node.prevNode();
//      if (name == lxNameAttr) {
//         node = lxImport;
//
//         return true;
//      }
//      else return false;
//   }
////   else if (test(declType, daTemplate)) {
////      if (testany(declType, daImport | daType))
////         _scope->raiseError(errInvalidSyntax, _filePath, node);
////   
////      recognizeDefinition(node);
////
////      ScopeType type = ScopeType::stClassTemplate;
////      if (node.existChild(lxCode)) {
////         type = ScopeType::stCodeTemplate;
////      }
////      else if (test(declType, daProperty)) {
////         type = ScopeType::stPropertyTemplate;
////      }
////      else if (test(declType, daInline)) {
////         type = ScopeType::stInlineTemplate;
////      }
////      else if (test(declType, daExtension)) {
////         type = ScopeType::stExtensionTemplate;
////      }
////
////      generateTemplateTree(node, nameNode, type);
////
////      node = lxIdle;
////
////      return true;
////   }
//
//   return false;
//}

void DerivationWriter :: recognizeDefinition(SNode scopeNode)
{
   SNode bodyNode = scopeNode.firstChild();
   if (scopeNode.existChild(lxCode, lxReturning)) {
      // mark one method class declaration
      scopeNode.set(lxClass, MODE_FUNCTION);
   }
   else if (bodyNode == lxExpression) {
      scopeNode = lxSymbol;
   }
//   else if (bodyNode.compare(lxSizeDecl, lxFieldInit)) {
//      _scope->raiseError(errInvalidSyntax, _filePath, bodyNode);
//   }
   else if (bodyNode == lxTemplateArgs) {
      scopeNode = lxClass;

      recognizeClassMebers(scopeNode);

      generateTemplateTree(scopeNode/*, type*/);

      scopeNode = lxIdle;
   }
   else {
      scopeNode = lxClass;

      recognizeClassMebers(scopeNode);
   }
}

void DerivationWriter :: recognizeScope()
{
   SNode node = _cache.readRoot().lastChild();
   if (node == lxScope) {
      recognizeDefinition(node);
   }
   else if (node == lxAttributeDecl) {
      declareAttribute(node);
   }
   else if (node == lxStatementDecl) {
      recognizeDefinition(node);

      declareStatement(node, ScopeType::stCodeTemplate);

      // should be later skipped 
      node = lxIdle;
   }
}

ref_t DerivationWriter :: mapAttribute(SNode node, bool allowType, /*bool& allowPropertyTemplate, */ref_t& previusCategory)
{
   ref_t attrRef = 0;
////
////      bool tokenNode = node.existChild(lxToken);
////      bool sizeNode = node.existChild(lxDynamicSizeDecl);
////      if (!allowType && (tokenNode || sizeNode))
////         _scope->raiseError(errInvalidHint, _filePath, node);
////
////      if (tokenNode)
////         return V_TEMPLATE;

   SNode terminal = node.firstChild(lxTerminalMask);
   ident_t token = terminal.identifier();

//      if (allowPropertyTemplate) {
//         allowPropertyTemplate = false;
//
//         // COMPILER MAGIC : recognize property template
//         IdentifierString templateName(token);
//         int paramCount = allowType ? 1 : 2;
//
//         templateName.append("#prop#");
//         templateName.appendInt(paramCount);
//
//         ref_t templateRef = _scope->attributes.get(templateName.ident());
//         if (templateRef) {
//            return V_PROPERTY;
//         }
//      }
//
//      allowPropertyTemplate = false;

   ref_t ref = _scope->attributes.get(token);      
   if (isPrimitiveRef(ref)) {
      // Compiler magic : check if the attribute have correct order
      if ((ref & V_CATEGORY_MASK) < previusCategory) {
         previusCategory = ref & V_CATEGORY_MASK;
      }
      else ref = 0u;
   }

   if (allowType) {
      if (!isPrimitiveRef(ref))
         attrRef = ref;

      if (attrRef || !ref)
         return attrRef;
   }

   if (!isPrimitiveRef(ref) && !allowType)
      raiseError(errInvalidHint, node);

   return ref;
}

void DerivationWriter :: raiseError(ident_t err, SNode node)
{
   SNode parentNode = _output.CurrentNode();
   while (parentNode != lxNone) {
      if (parentNode == lxNamespace && parentNode.existChild(lxSourcePath)) {
         _scope->raiseError(err, parentNode.findChild(lxSourcePath).identifier(), node);

         return; // !! dummy return
      }

      parentNode = parentNode.parentNode();
   }

   _scope->raiseError(err, "<not defined>", node);
}

void DerivationWriter :: raiseWarning(int level, ident_t msg, SNode node)
{
   SNode parentNode = node.parentNode();
   while (parentNode != lxNone) {
      if (parentNode == lxNamespace && parentNode.existChild(lxSourcePath)) {
         _scope->raiseWarning(level, msg, parentNode.findChild(lxSourcePath).identifier(), node);

         return; // !! dummy return
      }

      parentNode = parentNode.parentNode();
   }

   _scope->raiseWarning(level, msg, "<not defined>", node);
}

void DerivationWriter :: declareAttribute(SNode node)
{
   ident_t name = node.findChild(lxIdentifier).identifier();
   SNode attrNode = node.findChild(lxSizeDecl);
   ref_t attrRef = 0;
   SNode valNode = attrNode.firstChild(lxTerminalMask);
   ident_t value = valNode.identifier();
   if (valNode == lxHexInteger) {
      attrRef = value.toULong(16);
   }
   //else if (valNode == lxInteger) {
   //   attrRef = value.toULong(10);
   //}
   else raiseError(errInvalidSyntax, attrNode);

   if (attrRef && _scope->attributes.add(name, attrRef, true)) {
      _scope->saveAttribute(name, attrRef);
   }
   else raiseError(errDuplicatedDefinition, node);
}

void DerivationWriter :: declareStatement(SNode node, ScopeType templateType)
{
   Scope templateScope;
   templateScope.templateMode = templateType;
   loadTemplateParameters(templateScope, node.findChild(lxTemplateArgs));

   int exprCounter = templateScope.parameters.Count();
   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
      loadTemplateExprParameters(templateScope, node);
      templateScope.ignoreTerminalInfo = true;
   }

   SyntaxTree templateTree;
   SyntaxWriter templateWriter(templateTree);

   templateWriter.newNode(lxRoot);

   generateScope(templateWriter, _cache.readRoot(), templateScope);

   templateWriter.closeNode();

   SNode nameTerminal = node.firstChild(lxTerminalMask);
   IdentifierString name(nameTerminal.identifier().c_str());
   // COMPILER MAGIC : if it is complex code template
   SNode subNameNode = node.findChild(lxBaseDecl);
   while (subNameNode == lxBaseDecl) {
      name.append(':');
      name.append(subNameNode.findChild(lxToken).firstChild(lxTerminalMask).identifier());

      subNameNode = subNameNode.nextNode();
   }
   name.append('#');
   name.appendInt(exprCounter);
   name.append('#');
   name.appendInt(templateScope.parameters.Count() - exprCounter);

   // verify if there is an attribute with the same name
   if (_scope->attributes.exist(name))
      raiseWarning(WARNING_LEVEL_2, wrnAmbiguousIdentifier, nameTerminal);

   ReferenceNs fullName("'", name.ident());
   ref_t reference = _scope->module->mapReference(fullName.c_str());

   // check for duplicate declaration
   if (_scope->module->mapSection(reference | mskSyntaxTreeRef, true))
      raiseError(errDuplicatedSymbol, node);

   _Memory* target = _scope->module->mapSection(reference | mskSyntaxTreeRef, false);

   SNode root = templateTree.readRoot();
   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
      // COMPILER MAGIC : code template : find the method body and ignore the rest, save as the attribute
      root = root.findChild(lxClass).findChild(lxClassMethod).findChild(lxCode);

      _scope->attributes.add(name.ident(), reference);
      _scope->saveAttribute(name.ident(), reference);
   }

   SyntaxTree::saveNode(root, target);
}

//ref_t DerivationWriter :: mapInlineAttribute(SNode terminal)
//{
//   ident_t token = terminal.findChild(lxIdentifier).identifier();
//
//   // COMPILER MAGIC : recognize attribute template
//   IdentifierString templateName(token);
//   templateName.append("#inline#");
//   templateName.appendInt(SyntaxTree::countChild(terminal, lxExpression) + 1);
//
//   ref_t templateRef = _scope->attributes.get(templateName.ident());
//   if (templateRef) {
//      return templateRef;
//   }
//   else _scope->raiseError(errInvalidHint, _filePath, terminal);
//
//   return 0;
//}

void DerivationWriter :: recognizeAttributes(SNode current, int mode, LexicalType nameNodeType)
{
   //   while (current == lxInlineAttribute) {
   //      ref_t inlineAttrRef = mapInlineAttribute(current);
   //
   //      current.setArgument(inlineAttrRef);
   //
   //      current = current.nextNode();
   //   }
   //   int templateMode = 0;
   //   bool privateOne = true;
   //   bool visibilitySet = false;
   //   bool allowPropertyTemplate = test(mode, MODE_PROPERTYALLOWED);
   //   bool withoutMapping = false;
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current == lxToken) {
      bool allowType = current.nextNode()/*.compare(lxNameAttr)*/ == nameNodeType;

      ref_t attrRef = mapAttribute(current, allowType, /*allowPropertyTemplate, */attributeCategory);
      if (isPrimitiveRef(attrRef)) {
         current.set(lxAttribute, attrRef);
         //         if (test(mode, MODE_ROOT)) {
         //            if ((attrRef == V_PUBLIC || attrRef == V_INTERNAL)) {
         //               // the symbol visibility should be provided only once
         //               if (!visibilitySet) {
         //                  privateOne = attrRef == V_INTERNAL;
         //                  visibilitySet = true;
         //               }
         //               else _scope->raiseError(errInvalidHint, _filePath, current);
         //            }
         //            else if(attrRef == V_INLINE || attrRef == V_PROPERTY) {
         //               templateMode = attrRef;
         //            }
         //            else if (attrRef == V_TYPETEMPL || attrRef == V_META) {
         //               // NOTE : the type alias should not be mapped in the module
         //               withoutMapping = true;
         //            }
         //         }
      }
      else if (attrRef != 0 || allowType) {
         current.set(lxType, attrRef);
         allowType = false;
      }
      else raiseWarning(WARNING_LEVEL_2, wrnUnknownHint, current);

      current = current.nextNode();
   }
}

void DerivationWriter :: recognizeScopeAttributes(SNode current, int mode)
{
   // set name
   SNode nameNode = current;
   nameNode = lxNameAttr;

   recognizeAttributes(goToFirstNode(nameNode.prevNode(), lxToken/*, lxInlineAttribute*/), mode, lxNameAttr);

   SNode nameTerminal = nameNode.firstChild(lxTerminalMask);
   IdentifierString name(nameTerminal.identifier().c_str());
//   if (nameNode.existChild(lxToken)) {
//      // if it is a template identifier      
//      bool codeTemplate = nameNode.nextNode().findChild(lxCode) == lxCode;
//      if (codeTemplate && nameNode.nextNode().existChild(lxParent)) {
//         // COMPILER MAGIC : if it is complex code template
//         SNode subNameNode = nameNode.nextNode().findChild(lxParent);
//         while (subNameNode == lxParent) {
//            name.append(':');
//            name.append(subNameNode.findChild(lxToken).firstChild(lxTerminalMask).identifier());
//
//            subNameNode = subNameNode.nextNode();
//         }
//      }
//
//      int paramCounter = SyntaxTree::countChild(nameNode, lxToken);
//      switch (templateMode) {
//         case V_INLINE:
//            name.append("#inline#");
//            break;
//         case V_PROPERTY:
//            name.append("#prop#");
//            break;
//         default:
//            name.append('#');
//            break;
//      }
//
//      name.appendInt(paramCounter);
//      if (codeTemplate) {
//         int subParamCounter = SyntaxTree::countChild(nameNode.nextNode(), lxParameter);
//         name.append('#');
//         name.appendInt(subParamCounter);
//      }
//   }
   
   // verify if there is an attribute with the same name
   if (test(mode, MODE_ROOT) && _scope->attributes.exist(name))
      raiseWarning(WARNING_LEVEL_2, wrnAmbiguousIdentifier, nameNode);
}

void DerivationWriter :: recognizeMethodMebers(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxParameter:
         {
            SNode paramNode = current.lastChild();
            recognizeScopeAttributes(paramNode, 0);
            paramNode.refresh();
            break;
         }
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: recognizeClassMebers(SNode node/*, DerivationScope& scope*/)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxScope) {
         SNode bodyNode = current.findChild(lxCode, lxDispatchCode, lxReturning, lxExpression/*, lxResendExpression*/);

         int mode = 0;
         if (bodyNode == lxExpression) {
            // if it is a property, mark it as a get-property
            current.set(lxClassMethod, MODE_PROPERTYMETHOD);

            recognizeMethodMebers(current);
         }
         else if (bodyNode != lxNone) {
            // if it is a method
            current = lxClassMethod;

            recognizeMethodMebers(current);
         }
         else if (current.firstChild()/*.compare(lxSizeDecl, lxFieldInit, lxFieldAccum, */ == lxNone/*)*/) {
            // if it is a field
            current = lxClassField;
//            mode = MODE_PROPERTYALLOWED;
         }
//         else if (current.existChild(lxScope)) {
//            // if it is a property
//            current = lxClassProperty;
//         }
//         else _scope->raiseError(errInvalidSyntax, _filePath, current);

         recognizeScopeAttributes(current.prevNode(), mode);
      }
      else if (current == lxBaseDecl) {
         // HOTFIX : passing none as a name node, because the type should be last token
         recognizeAttributes(current.firstChild(), 0, lxNone);
      }

      current = current.nextNode();
   }
}

////void DerivationWriter :: generateMetaTree(SyntaxWriter& writer, SNode node, Scope& scope)
////{
////   writer.newNode(lxMeta);
////   //writer.appendNode(lxSourcePath, scope.sourcePath);
////
////   generateAttributes(writer, node.prevNode(), scope);
////
////   SNode current = node.firstChild();
////   while (current != lxNone) {
////      if (current == lxFieldInit) {
////         generateExpressionTree(writer, current, scope);
////      }
////
////      current = current.nextNode();
////   }
////
////   writer.closeNode();
////}

void DerivationWriter :: generateScope(SyntaxWriter& writer, SNode node, Scope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxSymbol:
         {
////            DerivationScope rootScope(&scope, sourcePath, ns, imports);
////            rootScope.autogeneratedTree = &autogenerated;
////
////            if (!generateSingletonScope(writer, current, rootScope)) {
               generateSymbolTree(writer, current, scope);
////            }            
            break;
         }
         case lxClass:
            generateClassTree(writer, current, scope);
            break;
//         case lxForward:
//            declareType(current);
//            break;
//         //case lxMeta:
//         //   generateMetaTree(writer, current, scope);
//         //   break;
         case lxImport:
            generateImport(writer, current);
            break;
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: declareNestedNamespace(SNode node, Scope& derivationScope)
{
   SyntaxTree buffer((pos_t)0);

   _output.newNode(lxNamespace);

   generateAttributes(_output, node.lastChild(), derivationScope, buffer);

   if (!buffer.isEmpty())
      SyntaxTree::copyNode(_output, buffer.readRoot());

   _cache.clear();
   _cacheWriter.clear();
   _cacheWriter.newNode(lxRoot);

   // NOTE : the node should not be closed 
}

void DerivationWriter :: generateSymbolTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SyntaxTree buffer((pos_t)0);

   writer.newNode(lxSymbol);
   //writer.appendNode(lxSourcePath, scope.sourcePath);

   generateAttributes(writer, node.prevNode(), derivationScope, buffer);

   generateExpressionTree(writer, node.findChild(lxExpression), derivationScope);

   if (!buffer.isEmpty())
      SyntaxTree::copyNode(writer, buffer.readRoot());

   writer.closeNode();
}

void DerivationWriter :: generateClassTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool nested)
{
   SyntaxTree buffer((pos_t)0);

   bool functionMode = false;
   if (!nested) {
      writer.newNode(lxClass);

      generateAttributes(writer, node.prevNode(), derivationScope, buffer);
      if (node.argument == MODE_FUNCTION) {
         // if it is a single method singleton
         writer.appendNode(lxAttribute, V_SINGLETON);

         functionMode = true;
      }
   }

   SNode current = node.firstChild();
   if (functionMode) {
      // HOTFIX : recognize method parameters
      recognizeMethodMebers(node);

      generateMethodTree(writer, node, derivationScope, true, current.argument == MODE_PROPERTYMETHOD, buffer);
   }
   else {
      bool firstParent = true;
      while (current != lxNone) {
         if (current == lxBaseDecl) {
//            SNode baseNameNode = current.findChild(lxNameAttr);
            if (firstParent) {
               writer.newNode(lxParent);               
               generateAttributes(writer, current.firstChild(), derivationScope, buffer);
//               if (baseNameNode.existChild(lxToken)) {
//                  generateTemplateAttributes(writer, baseNameNode.findChild(lxToken), derivationScope);
//               }
               writer.closeNode();

               firstParent = false;
            }
            else {
//               if (baseNameNode.existChild(lxToken)) {
//                  generateClassTemplateTree(writer, current, derivationScope);
//               }
               /*else */raiseError(errInvalidHint, current);
            }
         }
         else if (current == lxClassMethod) {
            generateMethodTree(writer, current, derivationScope, false, current.argument == MODE_PROPERTYMETHOD, buffer);
         }
         else if (current == lxClassField/* || current == lxFieldInit*/) {
            generateFieldTree(writer, current, derivationScope, buffer);
         }
//         else if (current == lxClassProperty) {
//            generatePropertyTree(writer, current, derivationScope, buffer);
//         }
//         else if (current == lxInlineAttribute) {
//            generateInlineTemplateTree(writer, current, goToNode(current, lxNameAttr), derivationScope, buffer);
//         }
//         //      else if (current == lxFieldTemplate) {
//         //         withInPlaceInit |= generateFieldTemplateTree(writer, current, scope, buffer);
//         //      }
//         ////      else if (current == lxMessage) {
//         ////      }
//         //      else scope.raiseError(errInvalidSyntax, node);
//
         current = current.nextNode();
      }

      if (!buffer.isEmpty()) {
         SyntaxTree::copyNode(writer, buffer.readRoot());
      }
   }

   if (nested)
      writer.inject(lxNestedClass);

   writer.closeNode();
}

void DerivationWriter :: generateTemplateAttributes(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
   ref_t attributeCategory = 0u;
   while (current != lxNone) {
      if (current == lxToken) {
         generateExpressionAttribute(writer, current, derivationScope, attributeCategory, true);
      }
      current = current.nextNode();
   }
}

void DerivationWriter :: generateTypeAttribute(SyntaxWriter& writer, SNode node/*, size_t dimensionCounter*/, 
   ref_t typeRef, Scope& derivationScope)
{
   SNode terminal = node.firstChild(lxTerminalMask);

//   writer.newNode(lxTypeAttribute);
//   for (size_t i = 0; i < dimensionCounter; i++)
//      writer.newNode(lxArrayType);

   if (typeRef == V_TEMPLATE) {
      writer.newNode(lxType, V_TEMPLATE);
      copyIdentifier(writer, terminal, derivationScope.ignoreTerminalInfo);
      generateTemplateAttributes(writer, node.nextNode().firstChild(), derivationScope);
      writer.closeNode();
   }
   else {
      LexicalType targetType = lxType;
      int targetArgument = typeRef;
//      if (derivationScope.withTypeParameters()) {
//         // check template parameter if required
//         int index = derivationScope.parameters.get(terminal.identifier());
//         if (index != 0) {
//            targetType = lxTemplateParam;
//            targetArgument = index + derivationScope.nestedLevel;
//         }
//      }

      writer.newNode(targetType, targetArgument);
      copyIdentifier(writer, terminal, derivationScope.ignoreTerminalInfo);
      writer.closeNode();
   }

//   for (size_t i = 0; i < dimensionCounter; i++)
//      writer.closeNode();

//   writer.closeNode();
}

void DerivationWriter :: generateAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{
   SNode current = node;

   SNode nameNode;
   if (current == lxNameAttr) {
      nameNode = current;

      current = goToFirstNode(nameNode.prevNode(), lxAttribute, lxType/*, lxInlineAttribute */);
   }

   while (true) {
      if (current == lxType/* || (current.argument == V_TEMPLATE && current == lxAttribute)*/) {
//         size_t dimensionCounter = SyntaxTree::countChild(current, lxDynamicSizeDecl);
//
         generateTypeAttribute(writer, current/*, dimensionCounter*/, current.argument, derivationScope);
      }
//      //else if (current == lxInlineAttribute) {
//      //   // COMPILER MAGIC : inject an attribute template
//      //   generateAttributeTemplateTree(writer, current, nameNode, derivationScope, buffer);
//      //}
      else if (current == lxAttribute) {
         writer.newNode(lxAttribute, current.argument);
         copyIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);

//         if (current.existChild(lxDynamicSizeDecl))
//            _scope->raiseError(errInvalidSyntax, _filePath, current.findChild(lxDynamicSizeDecl));

         writer.closeNode();
      }
      else break;

      current = current.nextNode();
   }
   if (nameNode != lxNone) {
//      if (nameNode.existChild(lxDynamicSizeDecl))
//         _scope->raiseError(errInvalidSyntax, _filePath, nameNode.findChild(lxDynamicSizeDecl));

      SNode terminal = nameNode.firstChild(lxTerminalMask);

      LexicalType nameType = lxNameAttr;
//      ref_t nameArgument = nameNode.argument;
//      if (derivationScope.isNameParameter(terminal.identifier(), nameArgument)) {
//         nameType = lxTemplateNameParam;
//      }

      writer.newNode(nameType/*, nameArgument*/);
      copyIdentifier(writer, terminal, derivationScope.ignoreTerminalInfo);
      writer.closeNode();

//      if (nameArgument == MODE_COMPLEXMESSAGE) {
//         // COMPILER MAGIC : if it is a complex name
//         SNode parentNode = node.parentNode().prevNode();
//
//         writer.newNode(lxMessage);
//         copyIdentifier(writer, parentNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
//         writer.closeNode();
//
//         // copy the property attributes
//         SNode parentAttrNode = parentNode.prevNode();
//         while (parentAttrNode == lxAttribute) {
//            writer.newNode(parentAttrNode.type, parentAttrNode.argument);
//            SyntaxTree::copyNode(writer, parentAttrNode);
//            writer.closeNode();
//
//            parentAttrNode = parentAttrNode.prevNode();
//         }
//      }
   }
}

//inline void checkFieldPropAttributes(SNode node, bool& isPropertyTemplate, bool& isInitializer)
//{
//   SNode current = node.prevNode();
//   while (current.compare(lxAttribute, lxNameAttr, lxTarget)) {
//      if (current == lxAttribute && current.argument == V_PROPERTY) {
//         isPropertyTemplate = true;
//      }
//      else if (current == lxAttribute && current.argument == V_MEMBER) {
//         isInitializer = true;
//      }
//
//      current = current.prevNode();
//   }
//}
//
//void DerivationWriter :: generatePropertyTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
//{
//   //// COMPILER MAGIC : property declaration
//   //bool withPropertyTemplate = false;
//   //bool withAttributes = false;
//   //bool withInitializer = false;
//   //checkFieldPropAttributes(node, withPropertyTemplate, withInitializer, withAttributes);
//
//   recognizeClassMebers(node);
//
//   //if (withPropertyTemplate) {
//   //   // COMPILER MAGIC : inject an attribute template
//   //   generatePropertyTemplateTree(writer, node, derivationScope);
//   //}
//   //if (withAttributes) {
//   //   // COMPILER MAGIC : inject an attribute template
//   //   generateAttributeTemplateTree(writer, node, derivationScope);
//   //}
//   //else if (withInitializer) {
//   //   SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
//
//   //   writer.newNode(lxFieldInit);
//   //   ::copyIdentifier(writer, nameNode);
//   //   writer.closeNode();
//   //}
//   
//   generatePropertyBody(writer, node, derivationScope, nullptr, buffer);
//}

void DerivationWriter :: generateFieldTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{   
//   // COMPILER MAGIC : property template declaration
//   bool isPropertyTemplate = false;
//   bool isInitializer = false;
//   checkFieldPropAttributes(node, isPropertyTemplate, isInitializer);
//
//   if (isPropertyTemplate) {
//      // COMPILER MAGIC : inject a property template
//      generatePropertyTemplateTree(writer, node, derivationScope, buffer);
//   }
//   else if (!isInitializer) {
      writer.newNode(lxClassField/*, templateMode ? -1 : 0*/);
//      SNode sizeNode = node.findChild(lxSizeDecl);
//      if (sizeNode != lxNone) {
//         writer.newNode(lxSize);
//         copyIdentifier(writer, sizeNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
//         writer.closeNode();
//      }

      generateAttributes(writer, node.prevNode(), derivationScope, buffer);

      writer.closeNode();
//   }
//
//   // copy inplace initialization
//   SNode bodyNode = node.findChild(lxFieldInit, lxFieldAccum);
//   if (bodyNode != lxNone) {
//      SyntaxWriter bufferWriter(buffer);
//      if (buffer.isEmpty()) {
//         // HOTFIX : create a root node
//         bufferWriter.newNode(lxRoot);
//      }
//      else bufferWriter.findRoot();
//
//      SNode nameNode = node.prevNode();
//
//      bufferWriter.newNode(lxFieldInit);
//
//      if (derivationScope.templateMode != stNormal) {
//         // HOTFIX : save the template source path
//         IdentifierString fullPath(_scope->module->Name());
//         fullPath.append('\'');
//         fullPath.append(_filePath);
//
//         writer.appendNode(lxSourcePath, fullPath.c_str());
//         //writer.appendNode(lxTemplate, scope.templateRef);
//      }
//
//      SNode attrNode = nameNode.prevNode();
//      if (attrNode == lxAttribute && attrNode.argument == V_MEMBER) {
//         // HOTFIX : if the field has scope prefix - copy it as well
//         bufferWriter.newNode(lxAttribute, attrNode.argument);
//         copyIdentifier(bufferWriter, attrNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
//         bufferWriter.closeNode();
//      }
//
//      ::copyIdentifier(bufferWriter, nameNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
//      
//      bufferWriter.appendNode(lxAssign, bodyNode == lxFieldAccum ? -1 : 0);
//
//      generateExpressionTree(bufferWriter, bodyNode.findChild(lxExpression), derivationScope);
//      bufferWriter.closeNode();
//   }
}

void DerivationWriter :: generateMethodTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool functionMode, 
   bool propertyMode, SyntaxTree& buffer)
{
//   //// recognize template attributes
//   //SNode current = node.prevNode();
//   //while (current.compare(lxAttribute, lxNameAttr, lxTarget)) {
//   //   if (current == lxAttribute && current.argument == fMPLATTRIBUTE) {
//   //      // COMPILER MAGIC : inject an attribute template
//   //      generateAttributeTemplateTree(writer, node, derivationScope);
//   //   }
//
//   //   current = current.prevNode();
//   //}

   writer.newNode(lxClassMethod);
//   if (derivationScope.templateMode != stNormal) {
//      // HOTFIX : save the template source path
//      IdentifierString fullPath(_scope->module->Name());
//      fullPath.append('\'');
//      fullPath.append(_filePath);
//
//      writer.appendNode(lxSourcePath, fullPath.c_str());
//      //writer.appendNode(lxTemplate, scope.templateRef);
//   }

   if (propertyMode) {
      writer.appendNode(lxAttribute, V_GETACCESSOR);
   }

   if (functionMode) {
      writer.appendNode(lxAttribute, V_FUNCTION);
   }
   else generateAttributes(writer, node.prevNode(), derivationScope, buffer);

   // copy method arguments
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxParameter:
         {
            writer.newNode(lxMethodParameter, current.argument);

            SNode paramNode = current.lastChild();
            generateAttributes(writer, paramNode, derivationScope, buffer);

            writer.closeNode();
            break;
         }
//         case lxParent:
//         {
//            // COMPILER MAGIC : if it is a complex name
//            writer.newNode(lxMessage);
//            SNode identNode = current.findChild(lxToken).firstChild(lxTerminalMask);
//            copyIdentifier(writer, identNode, derivationScope.ignoreTerminalInfo);
//            writer.closeNode();
//            break;
//         }
         default:
            // otherwise break the loop
            current = SNode();
            break;
      }

      current = current.nextNode();
   }

   if (propertyMode) {
      writer.newNode(lxReturning);
      generateExpressionTree(writer, node.findChild(lxExpression), derivationScope, 0);
      writer.closeNode();
   }
   else {
      SNode bodyNode = node.findChild(lxCode, lxDispatchCode, lxReturning/*, lxResendExpression*/);
      if (bodyNode.compare(lxReturning, lxDispatchCode)) {
         writer.newNode(bodyNode.type);
         generateExpressionTree(writer, bodyNode.firstChild(), derivationScope, EXPRESSION_IMPLICIT_MODE);
         writer.closeNode();
      }
//      else if (bodyNode == lxResendExpression) {
//         writer.newNode(bodyNode.type);
//         generateExpressionTree(writer, bodyNode, derivationScope, EXPRESSION_IMPLICIT_MODE);
//         SNode block = bodyNode.nextNode();
//
//         if (block == lxCode)
//            generateCodeTree(writer, block, derivationScope);
//
//         writer.closeNode();
//      }
      else if (bodyNode == lxCode) {
         generateCodeTree(writer, bodyNode, derivationScope);
      }
   }

   writer.closeNode();
}

void DerivationWriter :: generateCodeTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   writer.newNode(node.type, node.argument);

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
         case lxEOP:
         {
            writer.newNode(lxEOP);
            //if (!derivationScope.ignoreTerminalInfo)
            //{
               SNode terminal = current.firstChild();
               SyntaxTree::copyNode(writer, lxRow, terminal);
               SyntaxTree::copyNode(writer, lxCol, terminal);
               SyntaxTree::copyNode(writer, lxLength, terminal);
            //}

            writer.closeNode();
            break;
         }
      }
      current = current.nextNode();
   }

   writer.closeNode();
}

void DerivationWriter :: generateCodeExpression(SyntaxWriter& writer, SNode current, Scope& derivationScope, bool closureMode)
{
   /*if (closureMode) {
      generateCodeTree(writer, current, derivationScope);
      writer.inject(lxClosureExpr);
      writer.closeNode();
   }
   else {*/
      writer.newNode(lxExpression);
      generateCodeTree(writer, current, derivationScope);
      writer.closeNode();
   //}
}

//void DerivationWriter :: generateClassTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
//{
//   SNode nameNode = node.firstChild();
//
//   SyntaxTree bufferTree;
//   SyntaxWriter bufferWriter(bufferTree);
//   bufferWriter.newNode(lxRoot);
//   generateTemplateAttributes(bufferWriter, nameNode.firstChild(), derivationScope);
//   bufferWriter.closeNode();
//
//   List<SNode> parameters;
//   IdentifierString templateName;
//   templateName.copy(nameNode.firstChild(lxTerminalMask).identifier());
//
//   SNode current = bufferTree.readRoot().firstChild();
//   while (current == lxTypeAttribute) {
//      parameters.add(current.findChild(lxTarget));
//
//      current = current.nextNode();
//   }
//
//   templateName.append('#');
//   templateName.appendInt(parameters.Count());
//
//   ref_t templateRef = _scope->resolveImplicitIdentifier(_ns, templateName.c_str(), false, &_importedNs);
//   if (!templateRef)
//      _scope->raiseError(errInvalidSyntax, _filePath, node);
//
//   _scope->importClassTemplate(writer, templateRef, parameters);
//
//   node = lxIdle;
//}
//
//void DerivationWriter :: generatePropertyBody(SyntaxWriter& writer, SNode node, Scope& derivationScope, 
//   List<SNode>* parameters, SyntaxTree& buffer)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxClassMethod) {
//         SNode subNameNode = current.prevNode();
//
//         subNameNode.setArgument(MODE_COMPLEXMESSAGE);
//
//         generateMethodTree(writer, current, derivationScope, false, current.argument == MODE_PROPERTYMETHOD, buffer);
//
//         if (parameters)
//            parameters->add(subNameNode);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void DerivationWriter :: generateInlineTemplateTree(SyntaxWriter& writer, SNode node, SNode nameNode, Scope& derivationScope, SyntaxTree& buffer)
//{
//   List<SNode> parameters;
//   //IdentifierString templateName;
//
//   //SyntaxTree tempTree;
//   //SyntaxWriter tempWriter(tempTree);
//
//   //SNode current = node;
//   //templateName.copy(current.firstChild(lxTerminalMask).identifier());
//
//   //if (nameNode.nextNode() == lxClassMethod) {
//   //   tempWriter.newNode(lxNameAttr);
//   //   tempWriter.newNode(lxMessage);
//
//   //   SyntaxTree::copyNode(tempWriter, nameNode);
//   //   SyntaxTree::copyMatchedNodes(writer, lxParameter, nameNode.nextNode());
//
//   //   tempWriter.closeNode();
//   //   tempWriter.closeNode();
//
//   //   parameters.add(tempTree.readRoot());
//   //}
//
//   // name parameter is always the last parameter
//   parameters.add(nameNode);
//
//   //templateName.append("#inline#");
//   //templateName.appendInt(parameters.Count());
//
//   ref_t templateRef = /*_scope->attributes.get(templateName.c_str())*/node.argument;
//   if (!templateRef)
//      _scope->raiseError(errInvalidSyntax, _filePath, node.parentNode());
//
//   SyntaxWriter bufferWriter(buffer);
//   if (buffer.isEmpty()) {
//      // HOTFIX : create a root node
//      bufferWriter.newNode(lxRoot);
//   }
//   else bufferWriter.findRoot();
//
//   _scope->generateTemplateProperty(bufferWriter, templateRef, parameters);
//}
//
//void DerivationWriter :: generatePropertyTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
//{
//   List<SNode> parameters;
//   IdentifierString templateName;
//
//   SyntaxTree tempTree;
//   SyntaxWriter tempWriter(tempTree);
//
//   SNode nameNode = node.prevNode();
//   SNode current = nameNode.prevNode();
//   if (current == lxTarget || (current == lxAttribute && current.argument == V_TEMPLATE)) {
//      // generate property type
//      //derivationScope.nestedLevel += 0x100;
//      generateAttributes(tempWriter, current, derivationScope, buffer);
//      //derivationScope.nestedLevel -= 0x100;
//
//      current = current.prevNode();
//   }
//
//   while (current == lxAttribute) {
//      if (current.argument == V_PROPERTY) {
//         templateName.copy(current.firstChild(lxTerminalMask).identifier());
//         current.setArgument(0);
//         break;
//      }
//
//      current = current.prevNode();
//   }
//
//   current = tempTree.readRoot();
//   if (current == lxTypeAttribute /*.compare(lxTarget, lxTemplateParam)*/) {
//      parameters.add(current.findChild(lxTarget, lxTemplateParam));
//   }
//
//   SNode t = nameNode.firstChild(lxTerminalMask);
//   ident_t s = t.identifier();
//
//   // name parameter is always the last parameter
//   parameters.add(nameNode);
//
//   templateName.append("#prop#");
//   templateName.appendInt(parameters.Count());
//
//   ref_t templateRef = _scope->attributes.get(templateName.c_str());
//   if (!templateRef)
//      _scope->raiseError(errInvalidSyntax, _filePath, node.parentNode());
//
//   _scope->generateTemplateProperty(writer, templateRef, parameters);
//}
//
//void DerivationWriter :: generateClosureTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope)
//{
//   if (node == lxInlineClosure) {
//      node = node.firstChild();
//   }
//   else if (node != lxClosureExpr) {
//      writer.inject(lxMethodParameter);
//      writer.closeNode();
//
//      node = node.nextNode();
//      while (node == lxParameter) {
//         writer.newNode(lxMethodParameter);
//         writer.newBookmark();
//
//         SNode tokenNode = node.findChild(lxToken);
//         generateTokenExpression(writer, tokenNode, derivationScope, false);
//
//         writer.removeBookmark();
//         writer.closeNode();;
//
//         node = node.nextNode();
//      }
//   }
//
//   if (node == lxReturning) {
//      writer.newNode(lxReturning);
//      generateExpressionTree(writer, node, derivationScope);
//      writer.closeNode();
//   }
//   else if (node == lxClosureExpr) {
//      generateCodeTree(writer, node.findChild(lxCode), derivationScope);
//   }
//
//   writer.inject(lxClosureExpr);
//   writer.closeNode();
//
//   while (node.nextNode() != lxNone)
//      node = node.nextNode();
//}
//
//ref_t DerivationWriter :: resolveTemplate(ident_t templateName)
//{
//   for (auto it = _importedNs.start(); !it.Eof(); it++) {
//      IdentifierString fullName(*it);
//      fullName.append("'");
//      fullName.append(templateName);
//
//      ref_t templateRef = 0;
//      _Module* templateModule = _scope->loadReferenceModule(fullName.c_str(), templateRef);
//      if (templateModule != nullptr && templateModule->mapSection(templateRef | mskSyntaxTreeRef, true) != nullptr) {
//         if (_scope->module != templateModule) {
//            return importReference(templateModule, templateRef, _scope->module);
//         }
//         else return templateRef;
//      }
//   }
//
//   return 0;
//}

void DerivationWriter :: generateStatementTemplateTree(SyntaxWriter& writer, SNode node, SyntaxTree& tempTree, ident_t templateName, Scope&)
{
   ref_t templateRef = _scope->attributes.get(templateName.c_str());
//   ref_t templateRef = resolveTemplate(templateName);
   if (!templateRef)
      raiseError(errInvalidSyntax, node.parentNode());

   // load code template parameters
   List<SNode> parameters;
   SNode current = tempTree.readRoot().firstChild();
   while (current != lxNone) {
      if (current == lxExpression) {
         parameters.add(current);
      }

      current = current.nextNode();
   }

   _scope->generateStatementCode(writer, templateRef, parameters);
}

void DerivationWriter :: saveTemplateParameters(SyntaxWriter& tempWriter, SNode current, Scope& derivationScope)
{
   while (current != lxNone) {
      if (current == lxStatementArgs) {
         saveTemplateParameters(tempWriter, current.firstChild(), derivationScope);
      }
      else if (current == lxCode) {
         derivationScope.nestedLevel += 0x100;
         generateCodeExpression(tempWriter, current, derivationScope, false);
         derivationScope.nestedLevel -= 0x100;
      }
      else if (current == lxExpression) {
         derivationScope.nestedLevel += 0x100;
         generateExpressionTree(tempWriter, current, derivationScope, 0);
         derivationScope.nestedLevel -= 0x100;
      }

      current = current.nextNode();
   }
}

inline void parseStatement(SNode current, IdentifierString& templateName, int& exprCounters, int& blockCounters)
{
   while (current != lxNone) {
      if (current == lxExpression) {
         //if (blockCounters == 0) {
         exprCounters++;
         /*}
         else blockCounters++;*/
      }
      else if (current == lxStatementArgs) {
         parseStatement(current.firstChild(), templateName, exprCounters, blockCounters);
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
}

void DerivationWriter :: generateStatementTemplateTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope)
{
   IdentifierString templateName;
   templateName.copy(node.firstChild(lxTerminalMask).identifier());

   int exprCounters = 0;
   int blockCounters = 0;
   SNode current = node.nextNode();
   parseStatement(current, templateName, exprCounters, blockCounters);

   templateName.append('#');
   templateName.appendInt(exprCounters);
   templateName.append('#');
   templateName.appendInt(blockCounters);

   // generate members
   SyntaxTree tempTree;
   SyntaxWriter tempWriter(tempTree);
   tempWriter.newNode(lxRoot);
   saveTemplateParameters(tempWriter, node, derivationScope);
   tempWriter.closeNode();

   generateStatementTemplateTree(writer, node, tempTree, templateName.ident(), derivationScope);

   while (node.nextNode() != lxNone)
      node = node.nextNode();
}

//inline bool isTypeExpressionAttribute(SNode current)
//{
//   return current.nextNode() == lxToken && current.nextNode().nextNode() != lxToken;
//}

void DerivationWriter :: generateExpressionAttribute(SyntaxWriter& writer, SNode current, Scope& derivationScope, 
   ref_t& previousCategory, bool templateArgMode/*, bool onlyAttributes*/)
{
   bool allowType = /*!onlyAttributes && (*/templateArgMode || current.nextNode().nextNode() != lxToken/*)*/;
//   bool allowProperty = false;
   
//   size_t dimensionCounter = SyntaxTree::countChild(current, lxDynamicSizeDecl);
//   if (dimensionCounter && !allowType)
//      _scope->raiseError(errInvalidSyntax, _filePath, current.findChild(lxDynamicSizeDecl));

   if (current.nextNode() == lxTemplateArgs) {
      // if it is a template based type
      generateTypeAttribute(writer, current, /*dimensionCounter, */V_TEMPLATE, derivationScope);
   }
   else {
      ref_t attrRef = mapAttribute(current, allowType, /*allowProperty, */previousCategory);
      if (isPrimitiveRef(attrRef)) {
         SNode identNode = current.firstChild(lxTerminalMask);

         writer.newNode(lxAttribute, attrRef);
         copyIdentifier(writer, identNode, derivationScope.ignoreTerminalInfo);
         writer.closeNode();
      }
      else generateTypeAttribute(writer, current, /*dimensionCounter, */attrRef, derivationScope);
   }
}

void DerivationWriter :: generateIdentifier(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
//   ref_t argument = 0;
//   // COMPILER MAGIC : if it is a class template declaration
//   if (current.nextNode() == lxToken) {
//      writer.newNode(lxTemplate);
//      if (current == lxToken) {
//         copyIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
//      }
//      else copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
//      
//      SNode argNode = current.nextNode();
//      ref_t attributeCategory = 0u;
//      while (argNode == lxToken) {
//         //writer.newBookmark();
//         generateExpressionAttribute(writer, argNode, derivationScope, attributeCategory, true);
//         //writer.removeBookmark();
//
//         argNode = argNode.nextNode();
//      }
//      
//      writer.closeNode();
//   }
   /*else */if (derivationScope.templateMode == ScopeType::stCodeTemplate) {
      int paramIndex = derivationScope.parameters.get(current.identifier());
      if (paramIndex != 0) {
         writer.newNode(lxTemplateParam, paramIndex + derivationScope.nestedLevel);
         copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
         writer.closeNode();
      }
      else copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
   }
//   else if (derivationScope.isNameParameter(current.identifier(), argument)) {
//      writer.newNode(lxTemplateNameParam, argument);
//      copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
//      writer.closeNode();
//   }
//   else if (derivationScope.isIdentifierParameter(current.identifier(), argument)) {
//      writer.newNode(lxTemplateIdentParam, argument);
//      copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
//      writer.closeNode();
//   }
   else copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
}

void DerivationWriter :: generateMesage(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
//   ref_t argument = 0;

   SNode identNode = current.firstChild(lxTerminalMask);
//   if (current == lxMessage && derivationScope.isMessageParameter(identNode.identifier(), argument)) {
//      writer.newNode(lxTemplateMsgParam, argument);
//      copyIdentifier(writer, identNode, derivationScope.ignoreTerminalInfo);
//      writer.closeNode();
//   }
//   else {
      writer.newNode(lxMessage);
//      if (current.compare(lxMessage, lxSubMessage)) {
         copyIdentifier(writer, identNode, derivationScope.ignoreTerminalInfo);
//      }
      writer.closeNode();
//   }
}

void DerivationWriter :: generateTokenExpression(SyntaxWriter& writer, SNode& node, Scope& derivationScope/*, bool rootMode*/)
{
   ref_t attributeCategory = V_CATEGORY_MAX;

   // save the token expression start
   SNode current = node;

   // find the last token
   SNode lastNode = node;
   node = node.nextNode();   
   while (node.compare(lxToken, lxTemplateArgs)) {
      lastNode = node;
      node = node.nextNode();
   }

   // NOTE : set the node back to the last one due to implementation
   if (node == lxNestedClass) {
      // NOTE : nested class is considered as a last token
      node = lastNode;
      lastNode = node.nextNode();
   }
   else if (node == lxMessage && SyntaxTree::existSibling(node, lxStatementArgs)) {
      // COMPILER MAGIC : recognize the statemnt template
      generateStatementTemplateTree(writer, current, derivationScope);
      lastNode = current;
      node = current;
   }
   else node = lastNode;

//   if (node.nextNode().compare(lxCollection, lxNestedClass, lxAttrExpression)) {
//      generateExpressionAttribute(writer, node, derivationScope, attributeCategory, false, true);
//   }
//   else {
      while (current != lastNode) {
         if (current == lxToken)
            // skip template args
            generateExpressionAttribute(writer, current, derivationScope, attributeCategory, false);

         current = current.nextNode();
      }
//      if (rootMode) {
//         if (goToNode(node, lxCode/*, lxClosureExpr*/, lxOperator) == lxCode) {
//            // COMPILER MAGIC : recognize the code template
//            generateCodeTemplateTree(writer, node, derivationScope);
//            return;
//         }
//      }
      if (lastNode == lxToken)
         generateIdentifier(writer, lastNode.firstChild(lxTerminalMask), derivationScope);
//
//      size_t dimensionCounter = SyntaxTree::countChild(node, lxDynamicSizeDecl);
//      if (dimensionCounter > 0) {
//         writer.appendNode(lxDimensionAttr, dimensionCounter);
//      }
//   }
}

//void DerivationWriter :: generateSwitchTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxSwitchOption:
////         case lxBiggerSwitchOption:
////         case lxLessSwitchOption:
////            if (current.type == lxBiggerSwitchOption) {
////               writer.newNode(lxOption, GREATER_MESSAGE_ID);
////            }
////            else if (current.type == lxLessSwitchOption) {
////               writer.newNode(lxOption, LESS_MESSAGE_ID);
////            }
//            /*else */writer.newNode(lxOption, EQUAL_OPERATOR_ID);
//            generateIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope);
//            generateCodeExpression(writer, current.firstChild(lxCode), derivationScope, false);
//            writer.closeNode();
//            break;
//         case lxLastSwitchOption:
//            writer.newNode(lxElse);
//            writer.newBookmark();
//            generateCodeExpression(writer, current.firstChild(lxCode), derivationScope, false);
//            writer.removeBookmark();
//            writer.closeNode();
//            break;
////         default:
////            scope.raiseError(errInvalidSyntax, current);
////            break;
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void DerivationWriter :: generateCollectionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
//{
//   writer.newNode(lxCollection);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxExpression) {
//         generateExpressionTree(writer, current, derivationScope);
//      }
//      current = current.nextNode();
//   }
//
//   writer.closeNode();
//}

//void DerivationWriter :: generateOperatorTemplateTree(SyntaxWriter& writer, SNode& current, Scope& derivationScope)
//{
////   // revert the first operand
////   writer.trim();
////
////   current = lxIdle;
////
////   SNode node = goToFirstNode(current);
//
//   IdentifierString templateName;
//   SNode identNode = current.firstChild(lxTerminalMask);
////   if (operatorNode.identifier().compare(IF_OPERATOR)) {
////      templateName.copy(DOIFNOTNIL_OPERATOR);
////   }
////   else if (operatorNode.identifier().compare(ALT_OPERATOR)) {
////      templateName.copy(TRYORRETURN_OPERATOR);
////   }
////
////   // generate members
////   SyntaxTree tempTree;
////   SyntaxWriter tempWriter(tempTree);
////
////   // generate loperand
////   derivationScope.nestedLevel += 0x100;
////   bool dummy1 = false, dummy2 = false;
////   tempWriter.newNode(lxRoot);
////
////   tempWriter.newNode(lxExpression);
////   generateExpressionNode(tempWriter, node, dummy1,dummy2, derivationScope);
////   tempWriter.closeNode();
////   derivationScope.nestedLevel -= 0x100;
////
////   // generate roperand
////   derivationScope.nestedLevel += 0x100;
////   generateExpressionTree(tempWriter, current.parentNode(), derivationScope);
////   derivationScope.nestedLevel -= 0x100;
////
////   tempWriter.closeNode();
////
////   generateCodeTemplateTree(writer, node, tempTree, templateName.ident(), derivationScope);
////
////   while (node.nextNode() != lxNone)
////      node = node.nextNode();
////
////   current = node;
//}

void DerivationWriter :: generateExpressionNode(SyntaxWriter& writer, SNode& current, bool& first/*, bool& expressionExpected*/, 
   Scope& derivationScope)
{
   switch (current.type) {
      case lxMessage:
         if (!first) {
            writer.inject(lxExpression);
            writer.closeNode();
         }
         else first = false;

         generateMesage(writer, current, derivationScope);
         break;
      case lxOperator:
      case lxAssign:
         if (!first) {
            writer.inject(lxExpression);
            writer.closeNode();
         }
         else first = false;
         writer.newNode(current.type, current.argument);
         copyIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
         writer.closeNode();
         break;
      case lxExpression:
         generateExpressionTree(writer, current, derivationScope);
         break;
//      case lxSubMessage:
//         generateMesage(writer, current, derivationScope);
//         break;
//      case lxAttrExpression:
//         generateExpressionTree(writer, current.findChild(lxExpression), derivationScope, 0);
//         break;
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
         generateTokenExpression(writer, current, derivationScope/*, true*/);
         break;
      case lxPropertyParam:
         // to indicate the get property call
         writer.appendNode(lxPropertyParam);
         break;
//      case lxSwitching:
//         generateSwitchTree(writer, current, derivationScope);
//         writer.inject(lxSwitching);
//         writer.closeNode();
//         expressionExpected = true;
//         break;
//      case lxCollection:
//         generateCollectionTree(writer, current, derivationScope);
//         first = false;
//         break;
//      case lxClosureExpr:
//      case lxInlineClosure:
//         // COMPILER MAGIC : recognize the closure without parameters, 
//         //                  the one with parameters should be handled in default case
//         generateClosureTree(writer, current, derivationScope);
//         break;
//      default:
//         if (isTerminal(current.type)) {
//            generateTokenExpression(writer, current, derivationScope, true);
//
//            if (current.nextNode().compare(lxClosureExpr, lxParameter, lxReturning)) {
//               // COMPILER MAGIC : recognize the closure
//               generateClosureTree(writer, current, derivationScope);
//            }
//         }
//         break;
   }
}

void DerivationWriter :: generateExpressionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, int mode)
{
   writer.newBookmark();
   
   bool first = true;
   bool expressionExpected = !test(mode, EXPRESSION_IMPLICIT_MODE);
   
   SNode current = node.firstChild();
   while (current != lxNone) {
      generateExpressionNode(writer, current, first/*, expressionExpected*/, derivationScope);

      current = current.nextNode();
   }

   if (expressionExpected) {
      writer.inject(lxExpression);
      writer.closeNode();
   }

//   if (first && test(mode, EXPRESSION_OBJECT_REQUIRED))
//      scope.raiseError(errInvalidSyntax, node);

   writer.removeBookmark();
}

//void DerivationWriter:: declareType(SNode node)
//{
//   SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
//   SNode typeNameNode;
//
//   SNode current = node.firstChild();
//   bool invalid = true;
//   if (nameNode == lxIdentifier && isSingleStatement(current)) {
//      typeNameNode = current.firstChild(lxTerminalMask);
//
//      invalid = typeNameNode != lxIdentifier;
//   }
//
//   if (invalid)
//      _scope->raiseError(errInvalidSyntax, _filePath, current);
//
//   ident_t shortcut = nameNode.identifier();
//
//   if (_scope->attributes.exist(shortcut))
//      _scope->raiseError(errDuplicatedDefinition, _ns, nameNode);
//
//   ref_t classRef = _scope->mapNewIdentifier(_ns, typeNameNode.identifier(), false);
//
//   _scope->attributes.add(shortcut, classRef);
//   _scope->saveAttribute(shortcut, classRef);
//}

void DerivationWriter :: generateImport(SyntaxWriter& writer, SNode ns)
{
   SNode nameNode = ns.prevNode().firstChild(lxTerminalMask);
   if (nameNode != lxNone) {
      ident_t name = nameNode.identifier();

//      if (name.compare(STANDARD_MODULE))
//         // system module is included automatically - nothing to do in this case
//         return;

      writer.newNode(lxImport, name);
      copyIdentifier(writer, nameNode, false);
      writer.closeNode();

//      _importedNs.add(name.clone());
   }
}

// --- TemplateGenerator::TemplateScope ---

bool TemplateGenerator::TemplateScope :: generateClassName()
{
   ident_t templateName = moduleScope->module->resolveReference(templateRef);
   IdentifierString name;
   if (isWeakReference(templateName)) {
      name.copy(moduleScope->module->Name());
      name.append(templateName);
   }
   else name.copy(templateName);

   auto it = parameterValues.start();
   while (!it.Eof()) {
      name.append('&');

      ident_t param = moduleScope->module->resolveReference((*it).argument);
      if (isWeakReference(param) && !isTemplateWeakReference(param)) {
         name.append(moduleScope->module->Name());
         name.append(param);
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

TemplateGenerator :: TemplateGenerator(SyntaxTree&)
{
//   _root = tree.readRoot();
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
   if (test(current.type, lxTerminalMask/* | lxObjectMask*/)) {
      copyIdentifier(writer, current, false);
   }
////   else if (current == lxTemplate) {
////      writer.appendNode(lxTemplate, scope.templateRef);
////   }
//   else if (current == lxTarget && current.argument != 0) {
//      if (!scope.importMode && scope.moduleScope->module != scope.templateModule)
//         current.setArgument(importReference(scope.templateModule, current.argument, scope.moduleScope->module));
//
//      copyExpressionTree(writer, current, scope);
//   }
   else if (current == lxTemplateParam) {
      if (scope.type == TemplateScope::ttCodeTemplate) {
         if (current.argument < 0x100) {
//            // HOTFIX : to prevent the targets declared in the main scope from importing
//            bool oldMode = scope.importMode;
//            scope.importMode = true;

            SNode nodeToInject = scope.parameterValues.get(current.argument);
            if (nodeToInject == lxCode) {
               writer.newNode(lxExpression);
               copyExpressionTree(writer, nodeToInject, scope);
               writer.closeNode();
            }
            else if (nodeToInject == lxExpression) {
               copyExpressionTree(writer, nodeToInject, scope);
            }

//            scope.importMode = oldMode;
         }
//         else {
//            // if it is a nested template
//            writer.newNode(current.type, current.argument - 0x100);
//            copyChildren(writer, current, scope);
//            writer.closeNode();
//         }
      }
//      else if (scope.type == TemplateScope::ttPropertyTemplate || scope.type == TemplateScope::ttClassTemplate) {
//         SNode sizeNode = current.findChild(lxDimensionAttr);
//         SNode nodeToInject = scope.parameterValues.get(current.argument);
//         bool oldMode = scope.importMode;
//         scope.importMode = true;
//
//         if (sizeNode == lxDimensionAttr) {
//            // HOTFIX : if it is a node with the size postfix
//            if (nodeToInject.strArgument != -1) {
//               writer.newNode(nodeToInject.type, nodeToInject.identifier());
//            }
//            else writer.newNode(nodeToInject.type, nodeToInject.argument);
//
//            copyChildren(writer, nodeToInject, scope);
//            writer.appendNode(sizeNode.type, sizeNode.argument);
//
//            writer.closeNode();
//         }
//         else copyExpressionTree(writer, nodeToInject, scope);
//
//         scope.importMode = oldMode;
//      }
//      else throw InternalError("Not yet supported");
   }
//   else if (current == lxTemplateNameParam) {
//      if (current.argument < 0x100) {
//         // name node is always the last parameter
//         SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());
//
//         copyChildren(writer, nodeToInject, scope);
//      }
//      else {
//         // if it is a nested template
//         writer.newNode(current.type, current.argument - 0x100);
//         copyChildren(writer, current, scope);
//         writer.closeNode();
//      }
//   }
//   else if (current == lxTemplateIdentParam) {
//      if (current.argument < 0x100) {
//         SNode nodeToInject = scope.parameterValues.get(current.argument);
//
//         copyChildren(writer, nodeToInject, scope);
//      }
//      else {
//         // if it is a nested template
//         writer.newNode(current.type, current.argument - 0x100);
//         copyChildren(writer, current, scope);
//         writer.closeNode();
//      }
//   }
//   else if (current == lxTemplateMsgParam) {
//      // name node is always the last parameter
//      SNode nodeToInject = scope.parameterValues.get(current.argument);
//
//      writer.newNode(lxMessage);
//      copyChildren(writer, nodeToInject, scope);
//      writer.closeNode();
//      if (nodeToInject.argument == MODE_COMPLEXMESSAGE) {
//         // COMPILER MAGIC : if it is a complex name
//         SNode parentNode = nodeToInject.parentNode().prevNode();
//
//         writer.newNode(lxMessage);
//         copyIdentifier(writer, parentNode.firstChild(lxTerminalMask), false);
//         writer.closeNode();
//      }
//   }
//   else if (current == lxIdle) {
//      // skip idle nodes
//   }
   else copyExpressionTree(writer, current, scope);
}

void TemplateGenerator :: copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
////      if (current == lxIdentifier || current == lxPrivate || current == lxReference) {
////         copyIdentifier(writer, current);
////      }
      /*else */if (current == lxNameAttr) {
         copyTreeNode(writer, current, scope);
      }
//      else if (current == lxTemplateNameParam) {
//         // name node is always the last parameter
//         SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());
//
//         copyTreeNode(writer, nodeToInject, scope);
//      }
//      else if(current == lxTemplateParam) {
//         SNode nodeToInject = scope.parameterValues.get(current.argument);
//
//         // NOTE : target should not be imported / exported
//         copyExpressionTree(writer, nodeToInject, scope);
//      }
//      //      else if (current == lxTemplateParam && current.argument == INVALID_REF) {
////         if (current.argument == INVALID_REF) {
////            ref_t templateRef = generateNewTemplate(current, scope, &scope.parameterValues);
////
////            writeFullReference(writer, scope.compilerScope->module, templateRef, current);
////         }
////         else {
////            // if it is a template parameter
////            ref_t attrRef = scope.parameterValues.get(current.argument);
////            if (attrRef == INVALID_REF && (scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate)) {
////               copyIdentifier(writer, scope.identNode.firstChild(lxTerminalMask));
////            }
////            //else if ((int)attrRef < -1) {
////            //   copyParamAttribute(writer, current, scope);
////            //}
////            else writeFullReference(writer, scope.compilerScope->module, attrRef, current);
////         }
////      }
////      else if (current == lxTemplateField && current.argument >= 0) {
////         ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();
////
////         writer.newNode(lxIdentifier, fieldName);
////
////         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
////         writer.closeNode();
////      }
////      else if (current == lxTemplateAttribute) {
////         copyParamAttribute(writer, current, scope);
////      }
//      else if (current == lxDimensionAttr) {
//         writer.appendNode(current.type, current.argument);
//      }
      else if (current == lxAttribute) {
         copyTreeNode(writer, current, scope);
      }
      else if (current == lxType) {
         copyTreeNode(writer, current, scope);
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

//void TemplateGenerator :: copyFieldInitTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
//{
//   writer.newNode(node.type, node.argument);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
////      if (current == lxMemberIdentifier) {
////         copyIdentifier(writer, current);
////      }
//      /*else */copyExpressionTree(writer, current, scope);
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
//      if (current == lxTemplateNameParam) {
//         // name node is always the last parameter
//         SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());
//         copyExpressionTree(writer, nodeToInject, scope);
//      }
      /*else */copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

void TemplateGenerator :: copyModuleInfo(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxImport) {
         copyTreeNode(writer, current, scope);
      }
      else if (current == lxSourcePath) {
         copyTreeNode(writer, current, scope);
      }
      else break;

      current = current.nextNode();
   }
}

//////void DerivationTransformer :: copyTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributeValues, SubjectMap* parentAttributes, int mode)
//////{
//////   loadParameterValues(attributeValues, scope, parentAttributes/*, true*/);
//////
//////   if (generateTemplate(writer, scope, false, mode)) {
//////      //if (/*variableMode && */scope.reference != 0)
//////      //   writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(scope.reference));
//////   }
//////   else scope.raiseError(errInvalidHint, node);
//////}

bool TemplateGenerator :: generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass, bool importModuleInfo)
{
   _Memory* body = scope.loadTemplateTree();
   if (body == NULL)
      return false;

   SyntaxTree templateTree(body);
   SNode root = templateTree.readRoot();
   if (importModuleInfo) {
      copyModuleInfo(writer, root, scope);
   }

   if (scope.type != TemplateScope::ttCodeTemplate) {
      // HOTFIX : the code template contains the expression directly
      root = root.findChild(lxClass);
   }

   if (declaringClass) {
      // HOTFIX : exiting if the class was already declared in this module
      if (!scope.generateClassName() && scope.moduleScope->isDeclared(scope.reference))
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
//         if (scope.type == TemplateScope::ttPropertyTemplate) {
//            // do not copy the property attributes
//         }
//         else /*if (!test(mode, MODE_IMPORTING))*/ {
//            // do not copy the class attributes in the import mode 
            writer.newNode(current.type, current.argument);
            SyntaxTree::copyNode(writer, current);
            writer.closeNode();
//         }
      }
////      else if (current == lxTemplateParent && !test(mode, MODE_IMPORTING)) {
////         // HOTFIX : class based template
////         writer.newNode(lxBaseParent, -1);
////         copyClassTree(writer, current.findChild(lxTypeAttr), scope);
////         writer.closeNode();
////      }
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
      //else if (current == lxFieldInit) {
      //   //writer.newNode(lxFieldInit);
      //   //copyIdentifier(writer, current.findChild(lxMemberIdentifier));
      //   //writer.closeNode();

      //   //SyntaxWriter initWriter(*scope.autogeneratedTree);
      //   copyFieldInitTree(writer, current, scope);
      //}
      else if (current == lxExpression) {
         /*if (current.nextNode() == lxExpression) {
            // HOTFIX : if the code template contains several expressions
            writer.newNode(lxCodeExpression);
            copyNodes(writer, current, scope);
            writer.closeNode();
            break;
         }
         else */copyExpressionTree(writer, current, scope);
      }
      current = current.nextNode();
   }

   if (declaringClass) {
      writer.closeNode();
   }

   return true;
}

//void TemplateGenerator :: importClass(SyntaxWriter& output, SNode classNode)
//{
//   SNode current = classNode.firstChild();
//   while (current != lxNone) {
//      if (current.compare(lxClassMethod, lxClassField)) {
//         output.newNode(current.type, current.argument);
//         output.appendNode(lxAutogenerated);
//         SyntaxTree::copyNode(output, current);
//         output.closeNode();
//      }      
//
//      current = current.nextNode();
//   }
//}

ref_t TemplateGenerator :: declareTemplate(SyntaxWriter&, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL);
   templateScope.sourcePath = "compiling template...";

   for (auto it = parameters.start(); !it.Eof(); it++) {
      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   templateScope.generateClassName();

   return templateScope.reference;
}

ref_t TemplateGenerator :: generateTemplate(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters, bool importModuleInfo, bool importMode)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL);
   templateScope.sourcePath = "compiling template...";

   for (auto it = parameters.start(); !it.Eof(); it++) {
      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   // NOTE : for the import mode, no need to declare a new class
   if (generateTemplate(writer, templateScope, !importMode, importModuleInfo)) {
      return templateScope.reference;
   }
   else return 0;   
}

void TemplateGenerator :: generateTemplateCode(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL);
   templateScope.type = TemplateScope::Type::ttCodeTemplate;

   for (auto it = parameters.start(); !it.Eof(); it++) {
      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   generateTemplate(writer, templateScope, false, false);
}

//void TemplateGenerator :: generateTemplateProperty(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
//{
//   TemplateScope templateScope(&scope, reference, NULL, NULL/*, NULL*/);
//   templateScope.type = TemplateScope::Type::ttPropertyTemplate;
//
//   for (auto it = parameters.start(); !it.Eof(); it++) {
//      templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
//   }
//
//   generateTemplate(writer, templateScope, false, false);
//}
