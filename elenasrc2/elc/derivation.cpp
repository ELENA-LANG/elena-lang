//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Derivation Tree class implementation
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "derivation.h"
#include "errors.h"

using namespace _ELENA_;

constexpr auto MODE_ROOT                  = 0x01;
constexpr auto MODE_PROPERTYALLOWED       = 0x40;

constexpr auto MODE_FUNCTION              = -2;
constexpr auto MODE_PROPERTYMETHOD        = -4;

constexpr auto EXPRESSION_IMPLICIT_MODE   = 0x1;

//inline void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

// --- DerivationWriter ---

//inline SNode goToFirstNode(SNode current)
//{
//   SNode firstOne = current;
//   while (current != lxNone) {
//      firstOne = current;
//      current = current.prevNode();
//   }
//
//   return firstOne;
//}

inline SNode goBacktoSibling(SNode current, LexicalType type)
{
   while (current != lxNone && current != type) {
      current = current.prevNode();
   }

   return current;
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

inline SNode goToFirstNode(SNode current, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4)
{
   SNode firstOne = current;
   while (current != lxNone && current.compare(type1, type2, type3, type4)) {
      firstOne = current;
      current = current.prevNode();
   }

   return firstOne;
}

inline SNode goToNextNotIdleNode(SNode current)
{
   current = current.nextNode();
   while (current != lxNone) {
      if (current == lxIdle) {

      }
      else break;

      current = current.nextNode();
   }

   return current;
}

inline SNode goToPrevNotIdleNode(SNode current)
{
   current = current.prevNode();
   while (current != lxNone) {
      if (current == lxIdle) {

      }
      else break;

      current = current.prevNode();
   }

   return current;
}

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
   recognizeScopeAttributes(node.lastChild(), MODE_ROOT);

   MetaScope scopeType = MetaScope::None;

   SNode current = node.firstChild();
   while (current == lxAttribute) {
      switch (current.argument) {
         case V_NAMESPACE:
            scopeType = MetaScope::Namespace;
            break;
         case V_TYPETEMPL:
            scopeType = MetaScope::Type;
            break;
         case V_IMPORT:
            scopeType = MetaScope::Import;
            break;
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
      else if (scopeType == MetaScope::Type) {
         symbol = lxForward;
      }
      // otherwise it should be cached
   }
   else if (symbol == lxAttributeDecl || symbol == lxStatementDecl || symbol == lxInlineDecl || symbol == lxPropertyDecl) {
      _cachingLevel = _level;
   }

   if (test(symbol, lxSubScopeEndMask)) {
      LexicalType injected = (LexicalType)((int)symbol & ~lxSubScopeEndMask);
      if (test(symbol, lxInjectMask)) {
         // DERIVATION MAGIC : inject sub scope:
         // - find a begining mark

         SNode target = _cacheWriter.CurrentNode();
         SNode current = goBacktoSibling(target.lastChild(), lxInjectMark);
         if (current != lxNone) {
            target = current.prevNode();
            while (target == lxIdle)
               target = target.prevNode();

            target = target.injectNode(injected);

            SyntaxTree::moveSiblingNodes(current, target);
         }
         else target.injectNode(injected);
      }
      else {
         SNode current = _cacheWriter.CurrentNode();

         current.injectNode(injected);
      }

      if (!_cacheWriter.moveToChild())
         _cacheWriter.newNode(lxIdle);
   }
   else _cacheWriter.newNode(symbol);
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
//   if (terminal.symbol == lxGlobalReference) {
//      writer.newNode(terminal.symbol, terminal.value + 1);
//   }
   /*else */if (terminal.symbol == lxLiteral || terminal == lxCharacter || terminal == lxWide) {
      // try to use local storage if the quote is not too big
      if (getlength(terminal.value) < 0x100) {
         QuoteTemplate<IdentifierString> quote(terminal.value);

         writer.newNode(terminal.symbol, quote.ident());
      }
      else {
         QuoteTemplate<DynamicString<char> > quote(terminal.value);

         writer.newNode(terminal.symbol, quote.ident());
      }
   }
   else writer.newNode(terminal.symbol, terminal.value);

   writer.appendNode(lxCol, terminal.col);
   writer.appendNode(lxRow, terminal.row);
   writer.appendNode(lxLength, terminal.length);

   writer.closeNode();
}

void DerivationWriter :: appendTerminal(TerminalInfo& terminal)
{
   // HOT FIX : if there are several constants e.g. $10$13, it should be treated like literal terminal
   if (terminal == lxCharacter && terminal.value.findSubStr(1, '$', terminal.length, NOTFOUND_POS) != NOTFOUND_POS) {
      terminal.symbol = lxLiteral;
   }

   saveTerminal(_cacheWriter, terminal);
}

void DerivationWriter :: saveScope(SyntaxWriter& writer)
{
   recognizeScope();

   Scope outputScope;
   flushScope(writer, _cache.readRoot(), outputScope);

   _cache.clear();
   _cacheWriter.clear();
}

void DerivationWriter :: loadTemplateParameters(Scope& scope, SNode node)
{
   SNode current = node.findChild(lxToken);
   while (current == lxToken) {
      if (current.existChild(lxTokenArgs, lxDynamicBrackets))
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

void DerivationWriter :: generateTemplateTree(SNode node, ScopeType templateType)
{
   Scope templateScope;
   templateScope.templateMode = templateType;

   SNode argsNode = node.prevNode().firstChild();
   loadTemplateParameters(templateScope, argsNode);

   SNode identNode = argsNode.firstChild(lxTerminalMask);
   IdentifierString name(identNode.identifier());
   int paramCounter = templateScope.parameters.Count();
   name.append('#');

   name.appendInt(paramCounter);

   identNode.setStrArgument(name.c_str());

   if (templateType == ScopeType::stExtensionTemplate) {
      _output.newNode(lxExtensionTemplate);
   }
   else _output.newNode(lxTemplate);
   flushScope(_output, _cache.readRoot(), templateScope);
   _output.closeNode();
}

void DerivationWriter :: recognizeDefinition(SNode scopeNode)
{
   SNode bodyNode = scopeNode.firstChild();
   if (scopeNode.existChild(lxCode, lxReturning)) {
      // mark one method class declaration
      scopeNode.set(lxClass, (ref_t)MODE_FUNCTION);
   }
   else if (bodyNode == lxExpression) {
      scopeNode = lxSymbol;
   }
   else if (scopeNode.prevNode().firstChild() == lxTokenArgs) {
      scopeNode = lxClass;

      // validate the template type
      ScopeType type = ScopeType::stClassTemplate;
      SNode prevNode = scopeNode.prevNode();
      while (prevNode != lxNone) {
         if (prevNode == lxAttribute && prevNode.argument == V_EXTENSION) {
            type = ScopeType::stExtensionTemplate;
         }
         prevNode = prevNode.prevNode();
      }

      recognizeClassMembers(scopeNode);

      generateTemplateTree(scopeNode, type);

      scopeNode = lxIdle;
   }
   else {
      scopeNode = lxClass;

      recognizeClassMembers(scopeNode);
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
   else if (node == lxInlineDecl) {
      node = lxClass;
      recognizeClassMembers(node);

      declareStatement(node, ScopeType::stInlineTemplate);

      // should be later skipped
      node = lxIdle;
   }
   else if (node == lxPropertyDecl) {
      // NOTE : property template is saved as is
      declareStatement(node, ScopeType::stPropertyTemplate);

      // should be later skipped
      node = lxIdle;
   }
}

ref_t DerivationWriter :: mapAttribute(SNode node, bool allowType, bool& allowPropertyTemplate, ref_t& previusCategory)
{
   ref_t attrRef = 0;

   SNode terminal = node.firstChild(lxTerminalMask);
   ident_t token = terminal.identifier();

   if (allowPropertyTemplate) {
      allowPropertyTemplate = false;

      // COMPILER MAGIC : recognize property template
      IdentifierString templateName(token);
      int paramCount = allowType ? 1 : 2;

      templateName.append("#prop#");
      templateName.appendInt(paramCount);

      ref_t templateRef = _scope->attributes.get(templateName.ident());
      if (templateRef) {
         return V_PROPERTY;
      }
   }

   allowPropertyTemplate = false;

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

void DerivationWriter :: appendFilePath(SNode node, IdentifierString& path)
{
   SNode parentNode = node.parentNode();
   while (parentNode != lxNone) {
      if (parentNode == lxNamespace && parentNode.existChild(lxSourcePath)) {
         path.append(parentNode.findChild(lxSourcePath).identifier());

         return;
      }

      parentNode = parentNode.parentNode();
   }

   path.append("<not defined>");
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
   else if (valNode == lxInteger) {
      attrRef = value.toULong(10);
   }
   else raiseError(errInvalidSyntax, attrNode);

   if (attrRef && _scope->attributes.add(name, attrRef, true)) {
      _scope->saveAttribute(name, attrRef);
   }
   else raiseError(errDuplicatedDefinition, node);
}

void DerivationWriter :: declareStatement(SNode node, ScopeType templateType)
{
   SNode token = node.prevNode();

   SNode args = token.findChild(lxTokenArgs);

   Scope templateScope;
   templateScope.templateMode = templateType;
   loadTemplateParameters(templateScope, args);

   int exprCounter = templateScope.parameters.Count();
   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
      loadTemplateExprParameters(templateScope, node);
      templateScope.ignoreTerminalInfo = true;
   }

   SyntaxTree templateTree;
   SyntaxWriter templateWriter(templateTree);

   templateWriter.newNode(lxRoot);

   if (templateScope.templateMode == ScopeType::stPropertyTemplate) {
      // NOTE : property template body is copied as is
      SNode current = node.firstChild();
      while (current != lxNone) {
         copyScope(templateWriter, current, templateScope);

         current = current.nextNode();
      }
   }
   else flushScope(templateWriter, _cache.readRoot(), templateScope);

   templateWriter.closeNode();

   SNode nameTerminal = args != lxNone ? args.firstChild(lxTerminalMask) : token.firstChild(lxTerminalMask);
   IdentifierString name(nameTerminal.identifier().c_str());
   // COMPILER MAGIC : if it is complex code template
   SNode subNameNode = node.findChild(lxBaseDecl);
   while (subNameNode == lxBaseDecl) {
      name.append(':');
      name.append(subNameNode.findChild(lxToken).firstChild(lxTerminalMask).identifier());

      subNameNode = subNameNode.nextNode();
   }
   if (templateScope.templateMode == ScopeType::stInlineTemplate) {
      name.append("#inline#");
      name.appendInt(templateScope.parameters.Count());
   }
   else if (templateScope.templateMode == ScopeType::stPropertyTemplate) {
      name.append("#prop#");
      name.appendInt(templateScope.parameters.Count());
   }
   else {
      name.append('#');
      name.appendInt(exprCounter);
      name.append('#');
      name.appendInt(templateScope.parameters.Count() - exprCounter);
   }

   // verify if there is an attribute with the same name
   if (_scope->attributes.exist(name))
      raiseWarning(WARNING_LEVEL_2, wrnAmbiguousIdentifier, nameTerminal);

   IdentifierString fullName("'", name.ident());
   ref_t reference = _scope->module->mapReference(fullName.c_str());

   // check for duplicate declaration
   if (_scope->module->mapSection(reference | mskSyntaxTreeRef, true))
      raiseError(errDuplicatedSymbol, node);

   _Memory* target = _scope->module->mapSection(reference | mskSyntaxTreeRef, false);

   SNode root = templateTree.readRoot();
   if (templateScope.templateMode == ScopeType::stCodeTemplate) {
      // COMPILER MAGIC : code template : find the method body and ignore the rest, save as the attribute
      root = root.findChild(lxClass).findChild(lxClassMethod).findChild(lxCode);
   }
   _scope->attributes.add(name.ident(), reference);
   _scope->saveAttribute(name.ident(), reference);

   SyntaxTree::saveNode(root, target);
}

ref_t DerivationWriter :: mapInlineAttribute(SNode terminal)
{
   ident_t token = terminal.findChild(lxIdentifier).identifier();

   // COMPILER MAGIC : recognize attribute template
   IdentifierString templateName(token);
   templateName.append("#inline#");
   templateName.appendInt(SyntaxTree::countChild(terminal, lxExpression));

   ref_t templateRef = _scope->attributes.get(templateName.ident());
   if (templateRef) {
      return templateRef;
   }
   else raiseError(errInvalidHint, terminal);

   return 0;
}

void DerivationWriter :: recognizeAttributes(SNode current, int mode, LexicalType nameNodeType)
{
   while (current == lxInlineAttribute) {
      ref_t inlineAttrRef = mapInlineAttribute(current);

      current.setArgument(inlineAttrRef);

      current = current.nextNode();
   }
   bool allowPropertyTemplate = test(mode, MODE_PROPERTYALLOWED);
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current == lxToken) {
      // HOTFIX : skip idle nodes
      bool allowType = goToNextNotIdleNode(current) == nameNodeType;
      SNode child = current.firstChild();
      if (child == lxTokenArgs) {
         if (allowType)
            current.set(lxType, V_TEMPLATE);
      }
      else if (child == lxDynamicBrackets) {
         if (allowType)
            current.set(lxArrayType, 0);
      }
      else {
         ref_t attrRef = mapAttribute(current, allowType, allowPropertyTemplate, attributeCategory);
         if (isPrimitiveRef(attrRef)) {
            current.set(lxAttribute, attrRef);
         }
         else if (attrRef != 0 || allowType) {
            current.set(lxType, attrRef);
            allowType = false;
         }
         else raiseWarning(WARNING_LEVEL_2, wrnUnknownHint, current);
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: recognizeScopeAttributes(SNode current, int mode)
{
   bool templateMode = false;
   if (current.firstChild() == lxTokenArgs) {
      if (test(mode, MODE_ROOT)) {
         templateMode = true;
      }
      else raiseError(errInvalidSyntax, current);
   }

   // set name if it is not a template
   SNode nameNode = current;
   nameNode = templateMode ? lxTemplateNameAttr : lxNameAttr;

   recognizeAttributes(goToFirstNode(nameNode.prevNode(), lxToken, lxInlineAttribute),
      mode, templateMode ? lxTemplateNameAttr : lxNameAttr);

   if (!templateMode && test(mode, MODE_ROOT)) {
      SNode nameTerminal = nameNode.firstChild(lxTerminalMask);
      IdentifierString name(nameTerminal.identifier().c_str());

      // verify if there is an attribute with the same name
      if (_scope->attributes.exist(name))
         raiseWarning(WARNING_LEVEL_2, wrnAmbiguousIdentifier, nameNode);
   }
}

void DerivationWriter :: recognizeMethodMembers(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxParameter:
         {
            SNode paramNode = current.lastChild();
            recognizeScopeAttributes(paramNode, 0);
            //paramNode.refresh();
            break;
         }
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: recognizeClassMembers(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxScope) {
         SNode bodyNode = current.findChild(lxCode, lxDispatchCode, lxReturning, lxExpression, lxResendExpression, lxNoBody);

         int mode = 0;
         if (bodyNode == lxExpression) {
            // if it is a property, mark it as a get-property
            current.set(lxClassMethod, (ref_t)MODE_PROPERTYMETHOD);

            recognizeMethodMembers(current);
         }
         else if (bodyNode != lxNone) {
            // if it is a method
            current = lxClassMethod;

            recognizeMethodMembers(current);
         }
         else if (current.firstChild().compare(lxSizeDecl, lxFieldInit, /*lxFieldAccum, */lxNone)) {
            // if it is a field
            current = lxClassField;
            mode = MODE_PROPERTYALLOWED;
         }
         else if (current.existChild(lxScope)) {
            // if it is a property
            current = lxClassProperty;
         }

         recognizeScopeAttributes(current.prevNode(), mode);
      }
      else if (current == lxBaseDecl) {
         // HOTFIX : passing none as a name node, because the type should be last token
         recognizeAttributes(current.firstChild(), 0, lxNone);
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: copyScope(SyntaxWriter& writer, SNode node, Scope& scope)
{
   if (node.strArgument != -1) {
      writer.newNode(node.type, node.identifier());
   }
   else writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxIdentifier && scope.templateMode == ScopeType::stPropertyTemplate) {
         ref_t index = 0;
         if (scope.isNameParameter(current.identifier(), index)) {
            writer.newNode(lxTemplateNameParam, index);
            copyIdentifier(writer, current, scope.ignoreTerminalInfo);
            writer.closeNode();
         }
         else if (scope.isIdentifierParameter(current.identifier(), index)) {
            writer.newNode(lxTemplateIdentParam, index);
            copyIdentifier(writer, current, scope.ignoreTerminalInfo);
            writer.closeNode();
         }
         else copyScope(writer, current, scope);
      }
      else copyScope(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

void DerivationWriter :: flushScope(SyntaxWriter& writer, SNode node, Scope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxSymbol:
            flushSymbolTree(writer, current, scope);
            break;
         case lxClass:
            flushClassTree(writer, current, scope);
            break;
         case lxForward:
            declareType(current);
            break;
         case lxImport:
            flushImport(writer, current);
            break;
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: declareNestedNamespace(SNode node, Scope& derivationScope)
{
   SyntaxTree buffer((pos_t)0);

   _output.newNode(lxNamespace);

   flushAttributes(_output, node.lastChild(), derivationScope, buffer);

   if (!buffer.isEmpty())
      SyntaxTree::copyNode(_output, buffer.readRoot());

   _cache.clear();
   _cacheWriter.clear();
   _cacheWriter.newNode(lxRoot);

   // NOTE : the node should not be closed
}

void DerivationWriter :: flushSymbolTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SyntaxTree buffer((pos_t)0);

   writer.newNode(lxSymbol);
//   //writer.appendNode(lxSourcePath, scope.sourcePath);

   flushAttributes(writer, node.prevNode(), derivationScope, buffer);

   flushExpressionTree(writer, node.findChild(lxExpression), derivationScope);

   if (!buffer.isEmpty())
      SyntaxTree::copyNode(writer, buffer.readRoot());

   writer.closeNode();
}

void DerivationWriter :: generateClassImport(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree&)
{
   SNode terminalNode = node.firstChild().firstChild(lxTerminalMask);

   writer.newNode(lxClassImport, 0);

   copyIdentifier(writer, terminalNode, derivationScope.ignoreTerminalInfo);

   flushTemplateAttributes(writer, terminalNode.nextNode(), derivationScope);

   writer.closeNode();
}

void DerivationWriter :: flushClassTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SyntaxTree buffer((pos_t)0);

   bool functionMode = false;
   writer.newNode(lxClass);

   // HOTFIX : skip idle nodes
   flushAttributes(writer, goToPrevNotIdleNode(node), derivationScope, buffer);
   if (node.argument == MODE_FUNCTION) {
      // if it is a single method singleton
      writer.appendNode(lxAttribute, V_SINGLETON);

      functionMode = true;
   }

   SNode current = node.firstChild();
   if (functionMode) {
      // HOTFIX : recognize method parameters
      recognizeMethodMembers(node);

      flushMethodTree(writer, node, derivationScope, true, current.argument == MODE_PROPERTYMETHOD, buffer);
   }
   else {
      bool firstParent = true;
      while (current != lxNone) {
         if (current == lxBaseDecl) {
            if (firstParent) {
               writer.newNode(lxParent);
               flushAttributes(writer, current.firstChild(), derivationScope, buffer);
               writer.closeNode();

               firstParent = false;
            }
            else {
               SNode typeNode = current.findChild(lxType);
               if (typeNode == lxType && typeNode.argument == V_TEMPLATE) {
                  generateClassImport(writer, typeNode, derivationScope, buffer);
               }
               else raiseError(errInvalidHint, current);
            }
         }
         else if (current == lxClassMethod) {
            flushMethodTree(writer, current, derivationScope, false, current.argument == MODE_PROPERTYMETHOD, buffer);
         }
         else if (current.compare(lxClassField, lxFieldInit)) {
            flushFieldTree(writer, current, derivationScope, buffer);
         }
         else if (current == lxClassProperty) {
            flushPropertyTree(writer, current, derivationScope, buffer);
         }
         current = current.nextNode();
      }

      if (!buffer.isEmpty()) {
         SyntaxTree::copyNode(writer, buffer.readRoot());
      }
   }

   writer.closeNode();
}

void DerivationWriter :: flushTemplateAttributes(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
   ref_t attributeCategory = 0u;
   while (current != lxNone) {
      if (current == lxToken) {
         flushExpressionAttribute(writer, current, derivationScope, attributeCategory, true);
      }
      current = current.nextNode();
   }
}

void DerivationWriter :: flushArrayTypeAttribute(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SNode current = node.firstChild();
   if (current == lxArrayType) {
      writer.newNode(lxArrayType);
      flushArrayTypeAttribute(writer, current, derivationScope);
      writer.closeNode();
   }
   else if (current == lxTokenArgs) {
      writer.newNode(lxArrayType);
      flushTemplateTypeAttribute(writer, current, derivationScope);
      writer.closeNode();
   }
   else if (current == lxDynamicBrackets) {
      if (node == lxDynamicBrackets) {
         writer.newNode(lxArrayType);
         flushArrayTypeAttribute(writer, current, derivationScope);
         writer.closeNode();
      }
      else flushArrayTypeAttribute(writer, current, derivationScope);
   }
   else {
      writer.newNode(lxArrayType);
      flushTypeAttribute(writer, node, current.argument, derivationScope);
      writer.closeNode();
   }
}

void DerivationWriter :: flushTemplateTypeAttribute(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   SNode terminal = node.firstChild(lxTerminalMask);
   SNode current = terminal.nextNode();

   writer.newNode(lxType, V_TEMPLATE);
   copyIdentifier(writer, terminal, derivationScope.ignoreTerminalInfo);
   flushTemplateAttributes(writer, current, derivationScope);
   writer.closeNode();
}

void DerivationWriter :: flushTypeAttribute(SyntaxWriter& writer, SNode node, ref_t typeRef, Scope& derivationScope)
{
   SNode terminal = node.firstChild(lxTerminalMask);

   LexicalType targetType = lxType;
   int targetArgument = typeRef;
   if (derivationScope.withTypeParameters()) {
      // check template parameter if required
      int index = derivationScope.parameters.get(terminal.identifier());
      if (index != 0) {
         targetType = lxTemplateParam;
         targetArgument = index + derivationScope.nestedLevel;
      }
   }

   writer.newNode(targetType, targetArgument);
   copyIdentifier(writer, terminal, derivationScope.ignoreTerminalInfo);
   writer.closeNode();
}

void DerivationWriter :: flushAttributes(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{
   SNode current = node;

   SNode nameNode;
   if (current.compare(lxNameAttr, lxTemplateNameAttr)) {
      nameNode = current;

      current = current.prevNode();
      current = goToFirstNode(current, lxAttribute, lxType, lxArrayType, lxInlineAttribute);
   }

   while (true) {
      if (current == lxType || (current.argument == V_TEMPLATE && current == lxAttribute)) {
         if (current.argument == V_TEMPLATE) {
            flushTemplateTypeAttribute(writer, current.firstChild(), derivationScope);
         }
         else flushTypeAttribute(writer, current, current.argument, derivationScope);
      }
      else if (current == lxArrayType) {
         flushArrayTypeAttribute(writer, current, derivationScope);
      }
      else if (current == lxInlineAttribute) {
         // COMPILER MAGIC : inject an attribute template
         flushInlineTemplateTree(writer, current, node, derivationScope, buffer);
      }
      else if (current == lxAttribute) {
         writer.newNode(lxAttribute, current.argument);
         copyIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);

         writer.closeNode();
      }
      else break;

      current = current.nextNode();
   }

   if (nameNode != lxNone) {
      SNode terminal = (nameNode == lxTemplateNameAttr ? nameNode.firstChild() : nameNode).firstChild(lxTerminalMask);

//      LexicalType nameType = lxNameAttr;

      writer.newNode(lxNameAttr/*nameType, nameArgument*/);
      copyIdentifier(writer, terminal, derivationScope.ignoreTerminalInfo);
      writer.closeNode();
   }
}

inline bool isInitializerPrefix(SNode current)
{
   return current == lxAttribute && (current.argument == V_MEMBER || current.argument == V_META);
}

inline void checkFieldPropAttributes(SNode node, bool& isPropertyTemplate, bool& isInitializer)
{
   SNode current = node.prevNode();
   while (current.compare(lxAttribute, lxNameAttr, lxType)) {
      if (current == lxAttribute && current.argument == V_PROPERTY) {
         isPropertyTemplate = true;
      }
      else if (isInitializerPrefix(current)) {
         isInitializer = true;
      }

      current = current.prevNode();
   }
}

inline bool isFieldPlaceholder(SNode node, _ModuleScope* scope)
{
   if (node == lxToken && node.prevNode() != lxToken) {
      ref_t attr = scope->attributes.get(node.firstChild(lxTerminalMask).identifier());
      if (attr == V_FIELD)
         return true;
   }

   return false;
}

//inline void copyTemplateArgs(SNode src, SNode dst)
//{
//   SNode current = src.firstChild();
//   while (current != lxNone) {
//      if (current == lxDynamicSizeDecl) {
//         dst.appendNode(lxDynamicSizeDecl);
//      }
//      else {
//         SNode token = dst.appendNode(lxToken);
//         SyntaxTree::copyNode(current, token);
//      }
//
//      current = current.nextNode();
//   }
//}

void DerivationWriter :: flushPropertyTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{
   // inject property info
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxScope) {
         if (current.firstChild().compare(lxFieldInit, lxNone)) {
            if (isFieldPlaceholder(current.prevNode(), _scope)) {
               // HOTFIX : check if it is a field placeholder
               // and skip attribute info for it
               SNode prev = current;
               SNode nameNode = node.prevNode();
               while (nameNode.compare(lxNameAttr, lxType)) {
                  prev = prev.prependSibling(lxToken);
                  SyntaxTree::copyNode(nameNode, prev);

                  nameNode = nameNode.prevNode();
               }
            }
         }
         else {
            SNode prev = current;
            SNode nameNode = node.prevNode();
            while (nameNode.compare(lxNameAttr, lxType, lxAttribute)) {
               //if (nameNode == lxTemplateArgs) {
               //   prev = prev.prependSibling(lxTemplateArgs);
               //   copyTemplateArgs(nameNode, prev);
               //}
               //else if (nameNode == lxDynamicSizeDecl) {
               //   prev = prev.prependSibling(lxDynamicSizeDecl);
               //   copyTemplateArgs(nameNode, prev);
               //}
               if (nameNode == lxType) {
                  // check if there is a parameter - copy to it, otherwise it is returning value
                  if (current.existChild(lxParameter)) {
                     SNode param = current.findChild(lxParameter);
                     SyntaxTree::copyNode(nameNode, param.insertNode(lxToken));
                  }
                  else {
                     prev = prev.prependSibling(lxToken);
                     SyntaxTree::copyNode(nameNode, prev);
                  }
               }
               else {
                  prev = prev.prependSibling(lxToken);
                  SyntaxTree::copyNode(nameNode, prev);
               }

               nameNode = nameNode.prevNode();
            }
         }
      }
      current = current.nextNode();
   }

   recognizeClassMembers(node);

   flushPropertyBody(writer, node, derivationScope, buffer);
}

void DerivationWriter :: flushFieldTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{
   // COMPILER MAGIC : property template declaration
   bool isPropertyTemplate = false;
   bool isInitializer = false;
   checkFieldPropAttributes(node, isPropertyTemplate, isInitializer);

   if (isPropertyTemplate) {
      // COMPILER MAGIC : inject a property template
      flushPropertyTemplateTree(writer, node, derivationScope, buffer);
   }
   else if (!isInitializer) {
      writer.newNode(lxClassField/*, templateMode ? -1 : 0*/);
      SNode sizeNode = node.findChild(lxSizeDecl);
      if (sizeNode != lxNone) {
         writer.newNode(lxSize);
         copyIdentifier(writer, sizeNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
         writer.closeNode();
      }

      flushAttributes(writer, node.prevNode(), derivationScope, buffer);

      writer.closeNode();
   }

   // copy inplace initialization
   SNode bodyNode = node.findChild(lxFieldInit/*, lxFieldAccum*/);
   if (bodyNode != lxNone) {
      SyntaxWriter bufferWriter(buffer);
      if (buffer.isEmpty()) {
         // HOTFIX : create a root node
         bufferWriter.newNode(lxRoot);
      }
      else bufferWriter.findRoot();

      SNode nameNode = node.prevNode();

      bufferWriter.newNode(lxFieldInit, /*bodyNode == lxFieldAccum ? -1 : */0);
      bufferWriter.newNode(lxAssigningExpression);

      if (derivationScope.templateMode != stNormal) {
         // HOTFIX : save the template source path
         IdentifierString fullPath(_scope->module->Name());
         fullPath.append('\'');
         appendFilePath(writer.CurrentNode(), fullPath);

         writer.appendNode(lxSourcePath, fullPath.c_str());
         //writer.appendNode(lxTemplate, scope.templateRef);
      }

      SNode attrNode = nameNode.prevNode();
      if (isInitializerPrefix(attrNode)) {
         // HOTFIX : if the field has scope prefix - copy it as well
         bufferWriter.newNode(lxAttribute, attrNode.argument);
         copyIdentifier(bufferWriter, attrNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
         bufferWriter.closeNode();
      }

      ::copyIdentifier(bufferWriter, nameNode.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);

      flushExpressionTree(bufferWriter, bodyNode.findChild(lxExpression), derivationScope);
      bufferWriter.closeNode();
      bufferWriter.closeNode();
   }
}

void DerivationWriter :: flushMethodTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, bool functionMode,
   bool propertyMode, SyntaxTree& buffer)
{
   writer.newNode(lxClassMethod);
   if (derivationScope.templateMode != stNormal) {
      // HOTFIX : save the template source path
      IdentifierString fullPath(_scope->module->Name());
      fullPath.append('\'');
      appendFilePath(writer.CurrentNode(), fullPath);

      writer.appendNode(lxSourcePath, fullPath.c_str());
   }

   if (propertyMode) {
      writer.appendNode(lxAttribute, V_GETACCESSOR);
   }

   if (functionMode) {
      writer.appendNode(lxAttribute, V_FUNCTION);
   }
   else flushAttributes(writer, node.prevNode(), derivationScope, buffer);

   // copy method arguments
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxParameter:
         {
            writer.newNode(lxMethodParameter, current.argument);

            SNode paramNode = current.lastChild();
            flushAttributes(writer, paramNode, derivationScope, buffer);

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
      flushExpressionTree(writer, node.findChild(lxExpression), derivationScope, 0);
      writer.closeNode();
   }
   else {
      SNode bodyNode = node.findChild(lxCode, lxDispatchCode, lxReturning, lxResendExpression, lxNoBody);
      if (bodyNode.compare(lxReturning, lxDispatchCode)) {
         writer.newNode(bodyNode.type);
         flushExpressionTree(writer, bodyNode.firstChild(), derivationScope);
         writer.closeNode();
      }
      else if (bodyNode == lxResendExpression) {
         writer.newNode(bodyNode.type);
         flushExpressionTree(writer, bodyNode, derivationScope, EXPRESSION_IMPLICIT_MODE);
         SNode block = bodyNode.nextNode();

         if (block == lxCode)
            flushCodeTree(writer, block, derivationScope);

         writer.closeNode();
      }
      else if (bodyNode == lxCode) {
         flushCodeTree(writer, bodyNode, derivationScope);
      }
      else if (bodyNode == lxNoBody) {
         writer.appendNode(lxNoBody);
      }
   }

   writer.closeNode();
}

void DerivationWriter :: flushCodeTree(SyntaxWriter& writer, SNode node, Scope& derivationScope)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxExpression:
            flushExpressionTree(writer, current, derivationScope);
            break;
         //case lxCode:
         //   generateCodeTree(writer, current, derivationScope);
         //   break;
         case lxReturning:
            writer.newNode(current.type, current.argument);
            flushExpressionTree(writer, current, derivationScope, EXPRESSION_IMPLICIT_MODE);
            writer.closeNode();
            break;
         case lxEOP:
         {
            writer.newNode(lxEOP);

            if (!derivationScope.ignoreTerminalInfo) {
               SNode terminal = current.firstChild();
               SyntaxTree::copyNode(writer, lxRow, terminal);
               SyntaxTree::copyNode(writer, lxCol, terminal);
               SyntaxTree::copyNode(writer, lxLength, terminal);
            }

            writer.closeNode();
            break;
         }
      }
      current = current.nextNode();
   }

   writer.closeNode();
}

void DerivationWriter :: flushCodeExpression(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
   writer.newNode(lxExpression);
   flushCodeTree(writer, current, derivationScope);
   writer.closeNode();
}

void DerivationWriter :: flushPropertyBody(SyntaxWriter& writer, SNode node, Scope& derivationScope,
   SyntaxTree& buffer)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         flushMethodTree(writer, current, derivationScope, false, current.argument == MODE_PROPERTYMETHOD, buffer);
      }
      else if (current == lxClassField) {
         flushFieldTree(writer, current, derivationScope, buffer);
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: flushInlineTemplateTree(SyntaxWriter&, SNode node, SNode, Scope& derivationScope, SyntaxTree& buffer)
{
   // inject a bookmark into the owner scope
   _output.appendNode(lxBookmark, ++derivationScope.bookmark);

   List<SNode> parameters;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression) {
         parameters.add(current);
      }

      current = current.nextNode();
   }

   ref_t templateRef = /*_scope->attributes.get(templateName.c_str())*/node.argument;
   if (!templateRef)
      raiseError(errInvalidSyntax, node.parentNode());

   SyntaxWriter bufferWriter(buffer);
   if (buffer.isEmpty()) {
      // HOTFIX : create a root node
      bufferWriter.newNode(lxRoot);
   }
   else bufferWriter.findRoot();

   _scope->generateTemplateProperty(bufferWriter, templateRef, parameters, derivationScope.bookmark, true);
}

void DerivationWriter :: flushPropertyTemplateTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, SyntaxTree& buffer)
{
   SyntaxTree paramTree;
   SyntaxWriter paramWriter(paramTree);

   List<SNode> parameters;
   IdentifierString templateName;

   SNode nameNode = node.prevNode();
   SNode current = nameNode.prevNode();

   if (current == lxType/* || (current == lxAttribute && current.argument == V_TEMPLATE)*/) {
      // generate property type
      //derivationScope.nestedLevel += 0x100;
      flushAttributes(paramWriter, current, derivationScope, buffer);
      //derivationScope.nestedLevel -= 0x100;

      current = current.prevNode();
   }

   while (current == lxAttribute) {
      if (current.argument == V_PROPERTY) {
         templateName.copy(current.firstChild(lxTerminalMask).identifier());
         // move to the next attribute to be copied later
         current = current.nextNode();
         break;
      }

      current = current.prevNode();
   }

   SNode param = paramTree.readRoot();
   if (param.compare(lxType, lxTemplateParam)) {
      parameters.add(param/*.findChild(lxTarget, lxTemplateParam)*/);
   }

   SNode t = nameNode.firstChild(lxTerminalMask);
   ident_t s = t.identifier();

   // name parameter is always the last parameter
   parameters.add(nameNode);

   templateName.append("#prop#");
   templateName.appendInt(parameters.Count());

   ref_t templateRef = _scope->attributes.get(templateName.c_str());
   if (!templateRef)
      raiseError(errInvalidSyntax, node.parentNode());

   SyntaxTree tempTree;
   SyntaxWriter tempWriter(tempTree);
   tempWriter.newNode(lxRoot);
   // copy the property declaration
   while (current != node) {
      copyScope(tempWriter, current, derivationScope);

      current = current.nextNode();
   }

   tempWriter.newNode(lxClassProperty);
   _scope->generateTemplateProperty(tempWriter, templateRef, parameters, 0, false);
   tempWriter.closeNode();
   tempWriter.closeNode();

   flushPropertyTree(writer, tempTree.readRoot().findChild(lxClassProperty), derivationScope, buffer);
}

//void DerivationWriter :: generateClosureTree(SyntaxWriter& writer, SNode& node, Scope& derivationScope)
//{
////   if (node == lxInlineClosure) {
////      node = node.firstChild();
////   }
//   /*else */if (node != lxClosureExpr) {
//      writer.inject(lxMethodParameter);
//      writer.closeNode();
//
//      node = node.nextNode();
//      while (node == lxParameter) {
//         writer.newNode(lxMethodParameter);
//         writer.newBookmark();
//
//         SNode tokenNode = node.findChild(lxToken);
//         generateTokenExpression(writer, tokenNode, derivationScope/*, false*/);
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

ref_t DerivationWriter :: resolveTemplate(ident_t templateName)
{
   SNode node = _output.CurrentNode();
   while (node != lxNone) {
      if (node == lxNamespace) {
         SNode current = node.findChild(lxImport);
         while (current != lxNone) {
            if (current == lxImport) {
               IdentifierString fullName(current.identifier());
               fullName.append("'");
               fullName.append(templateName);

               ref_t templateRef = 0;
               _Module* templateModule = _scope->loadReferenceModule(fullName.c_str(), templateRef);
               if (templateModule != nullptr && templateModule->mapSection(templateRef | mskSyntaxTreeRef, true) != nullptr) {
                  if (_scope->module != templateModule) {
                     return importReference(templateModule, templateRef, _scope->module);
                  }
                  else return templateRef;
               }
            }
            current = current.nextNode();
         }
      }

      node = node.parentNode();
   }

   return 0;
}

void DerivationWriter :: generateStatementTemplateTree(SyntaxWriter& writer, SNode node, SyntaxTree& tempTree, ident_t templateName, Scope&)
{
   ref_t templateRef = _scope->attributes.get(templateName.c_str());
   if (!templateRef) {
      // bad luck : we have to find the template reference directly
      templateRef = resolveTemplate(templateName);
   }

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

void DerivationWriter :: saveTemplateParameters(SyntaxWriter& tempWriter, SNode current, Scope& derivationScope, bool rootMode)
{
   while (current != lxNone) {
      if (current == lxMessageExpression && rootMode) {
         saveTemplateParameters(tempWriter, current.firstChild(), derivationScope, false);
      }
      else if (current == lxCode) {
         derivationScope.nestedLevel += 0x100;
         flushCodeExpression(tempWriter, current, derivationScope);
         derivationScope.nestedLevel -= 0x100;
      }
      else if (current == lxExpression) {
         derivationScope.nestedLevel += 0x100;
         flushExpressionTree(tempWriter, current, derivationScope, 0);
         derivationScope.nestedLevel -= 0x100;
      }

      current = current.nextNode();
   }
}

inline SNode findNextToken(SNode current)
{
   while (current != lxNone) {
      if (current == lxIdle) {

      }
      else if (current == lxToken)
      {
         return current;
      }
      else break;

      current = current.nextNode();
   }

   return SNode();
}

void DerivationWriter :: flushExpressionAttribute(SyntaxWriter& writer, SNode node, Scope& derivationScope,
   ref_t& previousCategory, bool templateArgMode)
{
   bool allowType = false;
   bool allowProperty = false;

   if (!templateArgMode) {
      SNode nextNode = findNextToken(node.nextNode());
      if (nextNode == lxToken) {
         allowType = findNextToken(nextNode.nextNode()) != lxToken;
      }
   }
   else allowType = true;

   SNode current = node.firstChild();
   if (current == lxTokenArgs) {
      // if it is a template based type
      flushTemplateTypeAttribute(writer, current, derivationScope);
   }
   else if (current == lxDynamicBrackets) {
      //if (!allowType)
      //   _scope->raiseError(errInvalidSyntax, _filePath, current.findChild(lxDynamicSizeDecl));
      // if it is a template based type
      flushArrayTypeAttribute(writer, current, derivationScope);
   }
   else {
      ref_t attrRef = mapAttribute(node, allowType, allowProperty, previousCategory);
      if (isPrimitiveRef(attrRef)) {
         SNode identNode = node.firstChild(lxTerminalMask);

         writer.newNode(lxAttribute, attrRef);
         copyIdentifier(writer, identNode, derivationScope.ignoreTerminalInfo);
         writer.closeNode();
      }
      else flushTypeAttribute(writer, node, attrRef, derivationScope);
   }
}

void DerivationWriter :: flushIdentifier(SyntaxWriter& writer, SNode current, Scope& derivationScope)
{
   ref_t argument = 0;
   if (derivationScope.templateMode == ScopeType::stCodeTemplate
      || derivationScope.templateMode == ScopeType::stInlineTemplate)
   {
      int paramIndex = derivationScope.parameters.get(current.identifier());
      if (paramIndex != 0) {
         writer.newNode(lxTemplateParam, paramIndex + derivationScope.nestedLevel);
         copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
         writer.closeNode();
      }
      else copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
   }
   else if (derivationScope.isIdentifierParameter(current.identifier(), argument)) {
      writer.newNode(lxTemplateIdentParam, argument);
      copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
      writer.closeNode();
   }
   else copyIdentifier(writer, current, derivationScope.ignoreTerminalInfo);
}

void DerivationWriter :: flushMesage(SyntaxWriter& writer, SNode current, Scope& derivationScope)
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

inline void parseStatement(SNode current, IdentifierString& templateName, int& exprCounters, int& blockCounters, bool root)
{
   while (current != lxNone) {
      if (root && current == lxMessageExpression) {
         parseStatement(current.firstChild(), templateName, exprCounters, blockCounters, false);
      }
      else if (current == lxCode && root) {
         blockCounters++;
      }
      else if (test(current.type, lxObjectMask)) {
         if (/*blockCounters == 0*/!root) {
            exprCounters++;
         }
         else blockCounters++;
      }
      else if (current == lxToken && root) {
         // COMPILER MAGIC : if it is complex code template
         templateName.append(':');
         templateName.append(current.firstChild(lxTerminalMask).identifier());
      }

      current = current.nextNode();
   }
}

void DerivationWriter :: flushStatement(SyntaxWriter& writer, SNode& node, Scope& derivationScope)
{
   // COMPILER MAGIC : recognize the statement template

   IdentifierString templateName;

   SNode token;
   SNode current = node.firstChild();
   if (current != lxMessageExpression) {
      token = current;
      current = current.nextNode();
   }
   else token = current.firstChild();

   templateName.copy(token.firstChild(lxTerminalMask).identifier());

   int exprCounters = 0;
   int blockCounters = 0;
   parseStatement(current, templateName, exprCounters, blockCounters, true);

   templateName.append('#');
   templateName.appendInt(exprCounters);
   templateName.append('#');
   templateName.appendInt(blockCounters);

   // generate members
   SyntaxTree tempTree;
   SyntaxWriter tempWriter(tempTree);
   tempWriter.newNode(lxRoot);
   saveTemplateParameters(tempWriter, current, derivationScope, true);
   tempWriter.closeNode();

   generateStatementTemplateTree(writer, node, tempTree, templateName.ident(), derivationScope);
}

void DerivationWriter :: flushTokenExpression(SyntaxWriter& writer, SNode& node, Scope& derivationScope/*, bool rootMode*/)
{
   ref_t attributeCategory = V_CATEGORY_MAX;

   // save the token expression start
   SNode current = node;

   // find the last token
   SNode lastNode = node;
   while (node.compare(lxToken, lxIdle)) {
      if (node == lxToken)
         lastNode = node;

      node = node.nextNode();
   }

//   // NOTE : set the node back to the last one due to implementation
//   if (node.compare(lxCollection, lxAttrExpression)) {
//      // NOTE : nested class is considered as a last token
//      node = lastNode;
//      lastNode = node.nextNode();
//   }
   /*else */node = lastNode;

   while (current != lastNode) {
      if (current == lxToken)
         flushExpressionAttribute(writer, current, derivationScope, attributeCategory);

      current = current.nextNode();
   }

   if (lastNode == lxToken) {
      if (lastNode.firstChild().compare(lxTokenArgs, lxDynamicBrackets)) {
         flushExpressionAttribute(writer, lastNode, derivationScope, attributeCategory);
      }
      else flushIdentifier(writer, lastNode.firstChild(lxTerminalMask), derivationScope);
//         if (lastNode.nextNode().compare(lxClosureExpr, lxReturning, lxParameter)) {
//            // COMPILER MAGIC : recognize the closure
//            generateClosureTree(writer, current, derivationScope);
//            lastNode = current;
//            node = current;
//         }
   }
}

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

void DerivationWriter :: generateOperatorTemplateTree(SyntaxWriter& writer, SNode& current, Scope& derivationScope)
{
   SNode exprNode = current.firstChild(lxObjectMask);

//   // revert the first operand
//   writer.trim();
//
//   current = lxIdle;
//
//   SNode node = goToFirstNode(current);

   IdentifierString templateName("__inline");
   SNode operatorNode = exprNode.findChild(lxOperator).firstChild();
   if (operatorNode.identifier().compare(IF_OPERATOR)) {
      templateName.append(IF_MESSAGE);
   }
   else if (operatorNode.identifier().compare(ALT_OPERATOR)) {
      templateName.append(ALT_MESSAGE);
   }
   else if (operatorNode.identifier().compare(IFNOT_OPERATOR)) {
      templateName.append(IFNOT_MESSAGE);
   }

   templateName.append("#1#1");

   // generate members
   SyntaxTree tempTree;
   SyntaxWriter tempWriter(tempTree);

   // generate loperand
   derivationScope.nestedLevel += 0x100;
   tempWriter.newNode(lxRoot);

   tempWriter.newNode(lxExpression);
   SNode lnode = exprNode.firstChild();
   flushExpressionNode(tempWriter, lnode, derivationScope);
   tempWriter.closeNode();
   derivationScope.nestedLevel -= 0x100;

   // generate roperand
   derivationScope.nestedLevel += 0x100;
   tempWriter.newNode(lxExpression);
   SNode rnode = current.firstChild();
   flushExpressionNode(tempWriter, rnode, derivationScope);
   tempWriter.closeNode();
   derivationScope.nestedLevel -= 0x100;

   tempWriter.closeNode();

   generateStatementTemplateTree(writer, current, tempTree, templateName.ident(), derivationScope);

//   while (node.nextNode() != lxNone)
//      node = node.nextNode();
//
//   current = node;
}

void DerivationWriter :: flushExpressionNode(SyntaxWriter& writer, SNode& current/*, bool& first, bool& expressionExpected*/,
   Scope& derivationScope)
{
   switch (current.type) {
      case lxMessage:
         flushMesage(writer, current, derivationScope);
         break;
      case lxOperator:
         writer.newNode(current.type, current.argument);
         copyIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
         writer.closeNode();
         break;
      case lxInlineExpression:
         // COMPILER MAGIC : recognize the operator template
         generateOperatorTemplateTree(writer, current, derivationScope);
         break;
//      case lxSubMessage:
//         writer.newNode(lxSubMessage);
//         copyIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope.ignoreTerminalInfo);
//         writer.closeNode();
//         break;
      case lxNestedExpression:
      {
         SNode classNode = current.findChild(lxNestedClass);
         recognizeAttributes(goToFirstNode(classNode.prevNode(), lxToken, lxInlineAttribute, lxIdle),
            0, lxNestedClass);
         recognizeClassMembers(classNode);

         writer.newNode(lxNestedExpression);
         flushClassTree(writer, classNode, derivationScope);
         writer.closeNode();
         //         first = false;
         break;
      }
      case lxCode:
         flushCodeExpression(writer, current, derivationScope);
         break;
      case lxToken:
         flushTokenExpression(writer, current, derivationScope/*, true*/);
         break;
      case lxStatementExpression:
         flushStatement(writer, current, derivationScope/*, true*/);
         break;
//      case lxPropertyParam:
//         // to indicate the get property call
//         writer.appendNode(lxPropertyParam);
//         break;
      case lxSwitchOption:
         writer.newNode(lxOption, EQUAL_OPERATOR_ID);
         flushIdentifier(writer, current.firstChild(lxTerminalMask), derivationScope);
         flushCodeExpression(writer, current.firstChild(lxCode), derivationScope);
         writer.closeNode();
         break;
      case lxLastSwitchOption:
         writer.newNode(lxElse);
         flushCodeExpression(writer, current.firstChild(lxCode), derivationScope);
         writer.closeNode();
         break;
      case lxParameter:
         writer.newNode(lxMethodParameter);
         flushExpressionTree(writer, current, derivationScope, EXPRESSION_IMPLICIT_MODE);
         writer.closeNode();
         break;
         //      case lxCollection:
//         generateCollectionTree(writer, current, derivationScope);
//         first = false;
//         break;
      case lxInteger:
      case lxHexInteger:
      case lxLiteral:
      case lxWide:
      case lxCharacter:
      case lxMetaConstant:
      case lxLong:
      case lxReal:
      case lxExplicitConst:
         flushIdentifier(writer, current, derivationScope);
         break;
////      default:
////         if (isTerminal(current.type)) {
////            generateTokenExpression(writer, current, derivationScope, true);
////
////            if (current.nextNode().compare(lxClosureExpr, lxParameter, lxReturning)) {
////               // COMPILER MAGIC : recognize the closure
////               generateClosureTree(writer, current, derivationScope);
////            }
////         }
////         break;
      default:
         if (test(current.type, lxObjectMask)) {
            writer.newNode(current.type);
            flushExpressionTree(writer, current, derivationScope, EXPRESSION_IMPLICIT_MODE);
            writer.closeNode();
         }
         break;
   }
}

void DerivationWriter :: flushExpressionTree(SyntaxWriter& writer, SNode node, Scope& derivationScope, int mode)
{
//   bool first = true;
   bool expressionExpected = !test(mode, EXPRESSION_IMPLICIT_MODE);
   if (expressionExpected)
      writer.newNode(lxExpression);

   SNode current = node.firstChild();
   while (current != lxNone) {
      flushExpressionNode(writer, current/*, first, expressionExpected*/, derivationScope);

      current = current.nextNode();
   }

   if (expressionExpected) {
      writer.closeNode();
   }
}

void DerivationWriter :: declareType(SNode node)
{
   IdentifierString ns;

   SNode parentNode = _output.CurrentNode();
   while (parentNode != lxNone) {
      if (parentNode == lxNamespace) {
         SNode nameNode = parentNode.findChild(lxNameAttr);
         if (nameNode != lxNone) {
            ns.insert(nameNode.firstChild(lxTerminalMask).identifier(), 0);
            ns.insert("'", 0);
         }
      }

      parentNode = parentNode.parentNode();
   }

   SNode nameNode = node.prevNode().firstChild(lxTerminalMask);
   SNode typeNameNode;

   SNode current = node.firstChild();
   bool invalid = true;
   if (nameNode == lxIdentifier && isSingleStatement(current)) {
      typeNameNode = current.findChild(lxToken).firstChild(lxTerminalMask);

      invalid = typeNameNode != lxIdentifier;
   }

   if (invalid)
      raiseError(errInvalidSyntax, current);

   ident_t shortcut = nameNode.identifier();

   if (_scope->attributes.exist(shortcut))
      raiseError(errDuplicatedDefinition, nameNode);

   ref_t classRef = _scope->mapNewIdentifier(ns.c_str(), typeNameNode.identifier(), Visibility::Public);

   _scope->attributes.add(shortcut, classRef);
   _scope->saveAttribute(shortcut, classRef);
}

void DerivationWriter :: flushImport(SyntaxWriter& writer, SNode ns)
{
   SNode nameNode = ns.prevNode().firstChild(lxTerminalMask);
   if (nameNode != lxNone) {
      ident_t name = nameNode.identifier();

      writer.newNode(lxImport, name);
      copyIdentifier(writer, nameNode, false);
      writer.closeNode();
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

      // HOTFIX : in declaration mode, type is not defined - it should be taken from the declaredtype attribute
      ref_t typeRef = (*it).argument;
      if (typeRef == V_TEMPLATE || !typeRef)
         typeRef = (*it).findChild(lxDeclaredType).argument;

      ident_t param = moduleScope->module->resolveReference(typeRef);
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

TemplateGenerator :: TemplateGenerator()
{
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

void TemplateGenerator :: copyTemplateIdenParam(SyntaxWriter& writer, SNode nodeToInject, TemplateScope& scope)
{
   if (nodeToInject.type == lxType && nodeToInject.argument == V_TEMPLATE) {
      SNode attrNode = nodeToInject.firstChild();
      // copy a template name
      copyTreeNode(writer, attrNode, scope);
      writer.closeNode();
      writer.newNode(lxTokenArgs);
      attrNode = attrNode.nextNode();
      while (attrNode.compare(lxType, lxArrayType)) {
         if (attrNode == lxType) {
            writer.newNode(lxToken);
            copyTemplateIdenParam(writer, attrNode, scope);
            writer.closeNode();
         }
         else {
            writer.newNode(lxToken);
            copyTemplateIdenParam(writer, attrNode.findChild(lxType), scope);
            writer.closeNode();
            //writer.appendNode(lxDynamicSizeDecl);
         }

         attrNode = attrNode.nextNode();
      }
      // copy template attributes
   }
   else copyChildren(writer, nodeToInject, scope);
}

void TemplateGenerator :: copyTreeNode(SyntaxWriter& writer, SNode current, TemplateScope& scope)
{
   if (test(current.type, lxTerminalMask/* | lxObjectMask*/)) {
      copyIdentifier(writer, current, false);
   }
   else if (current == lxTemplateParam) {
      if (scope.type == TemplateScope::ttCodeTemplate) {
         if (current.argument < 0x100) {
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
         else {
            // if it is a nested template
            writer.newNode(current.type, current.argument - 0x100);
            copyChildren(writer, current, scope);
            writer.closeNode();
         }
      }
      else if (scope.type == TemplateScope::ttPropertyTemplate
         || scope.type == TemplateScope::ttClassTemplate || scope.type == TemplateScope::ttInlineTemplate) {
         SNode nodeToInject = scope.parameterValues.get(current.argument);
         copyExpressionTree(writer, nodeToInject, scope);
      }
      else throw InternalError("Not yet supported");
   }
   else if (current == lxTemplateNameParam) {
      if (current.argument < 0x100) {
         // name node is always the last parameter
         SNode nodeToInject = scope.parameterValues.get(scope.parameterValues.Count());

         copyChildren(writer, nodeToInject, scope);
      }
      else {
         // if it is a nested template
         writer.newNode(current.type, current.argument - 0x100);
         copyChildren(writer, current, scope);
         writer.closeNode();
      }
   }
   else if (current == lxTemplateIdentParam) {
      if (current.argument < 0x100) {
         SNode nodeToInject = scope.parameterValues.get(current.argument);

         copyTemplateIdenParam(writer, nodeToInject, scope);
      }
      else {
         // if it is a nested template
         writer.newNode(current.type, current.argument - 0x100);
         copyChildren(writer, current, scope);
         writer.closeNode();
      }
   }
   else copyExpressionTree(writer, current, scope);
}

void TemplateGenerator :: copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, int bookmark)
{
   writer.newNode(node.type, node.argument);
   if (bookmark)
      // HOTFIX : inject the bookmark reference, used for inline templates
      writer.appendNode(lxBookmarkReference, bookmark);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxNameAttr) {
         copyTreeNode(writer, current, scope);
      }
      else if(current == lxTemplateParam) {
         SNode nodeToInject = scope.parameterValues.get(current.argument);

         // NOTE : target should not be imported / exported
         copyExpressionTree(writer, nodeToInject, scope);
      }
      else if (current.compare(lxType, lxArrayType)) {
         copyTreeNode(writer, current, scope);
      }
      else if (current == lxAttribute) {
         copyTreeNode(writer, current, scope);
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void TemplateGenerator :: copyFieldInitTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, int bookmark)
{
   writer.newNode(node.type, node.argument);
   if (bookmark)
      // HOTFIX : inject the bookmark reference, used for inline templates
      writer.appendNode(lxBookmarkReference, bookmark);

   SNode current = node.firstChild();
   while (current != lxNone) {
      copyExpressionTree(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

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
      copyTreeNode(writer, current, scope);

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
      current = current.nextNode();
   }
}

bool TemplateGenerator :: generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass,
   bool importModuleInfo, int bookmark)
{
   _Memory* body = scope.loadTemplateTree();
   if (body == NULL)
      return false;

   SyntaxTree templateTree(body);
   SNode root = templateTree.readRoot();
   if (importModuleInfo) {
      copyModuleInfo(writer, root, scope);
   }

   if (scope.type == TemplateScope::ttPropertyTemplate) {
      // NOTE : property template is copied as is
      copyChildren(writer, root, scope);
   }
   else {
      if (scope.type != TemplateScope::ttCodeTemplate) {
         // HOTFIX : the code template contains the expression directly
         root = root.findChild(lxClass);
      }

      if (declaringClass) {
         // HOTFIX : exiting if the class was already declared in this module
         if (!scope.generateClassName() && scope.moduleScope->isDeclared(scope.reference))
            return true;

         ident_t fullName = scope.moduleScope->resolveFullName(scope.reference);

         writer.newNode(lxClass, INVALID_REF);
         writer.appendNode(lxAttribute, V_TEMPLATEBASED);
         writer.newNode(lxNameAttr, scope.moduleScope->mapFullReference(fullName, true));
         //      writer.appendNode(lxReference, fullName);
         writer.closeNode();
      }

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
         else if (current == lxParent && declaringClass) {
            // generate a parent node only for the template based class; it should be ignored for the template class import
            writer.newNode(lxParent, INVALID_REF);
            copyChildren(writer, current, scope);
            writer.closeNode();
         }
         else if (current == lxClassMethod) {
            copyMethodTree(writer, current, scope);
         }
         else if (current == lxClassField) {
            copyFieldTree(writer, current, scope, bookmark);
         }
         else if (current == lxFieldInit) {
            //writer.newNode(lxFieldInit);
            //copyIdentifier(writer, current.findChild(lxMemberIdentifier));
            //writer.closeNode();

            //SyntaxWriter initWriter(*scope.autogeneratedTree);
            copyFieldInitTree(writer, current, scope, bookmark);
         }
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

ref_t TemplateGenerator :: declareTemplate(SyntaxWriter&, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL);
   templateScope.sourcePath = "compiling template...";

   for (auto it = parameters.start(); !it.Eof(); it++) {
      // HOTFIX : ignore template source parameter
      if ((*it) != lxTemplateSource)
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
      // HOTFIX : ignore template source parameter
      if ((*it) != lxTemplateSource)
         templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   // NOTE : for the import mode, no need to declare a new class
   if (generateTemplate(writer, templateScope, !importMode, importModuleInfo, 0)) {
      return templateScope.reference;
   }
   else return 0;
}

void TemplateGenerator :: generateTemplateCode(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference, List<SNode>& parameters)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL);
   templateScope.type = TemplateScope::Type::ttCodeTemplate;

   for (auto it = parameters.start(); !it.Eof(); it++) {
      // HOTFIX : ignore template source parameter
      if ((*it) != lxTemplateSource)
         templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   generateTemplate(writer, templateScope, false, false, 0);
}

void TemplateGenerator :: generateTemplateProperty(SyntaxWriter& writer, _ModuleScope& scope, ref_t reference,
   List<SNode>& parameters, int bookmark, bool inlineMode)
{
   TemplateScope templateScope(&scope, reference, NULL, NULL/*, NULL*/);
   templateScope.type = inlineMode ? TemplateScope::Type::ttInlineTemplate : TemplateScope::Type::ttPropertyTemplate;

   for (auto it = parameters.start(); !it.Eof(); it++) {
      // HOTFIX : ignore template source parameter
      if ((*it) != lxTemplateSource)
         templateScope.parameterValues.add(templateScope.parameterValues.Count() + 1, *it);
   }

   generateTemplate(writer, templateScope, false, false, bookmark);
}
