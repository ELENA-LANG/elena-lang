//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Syntax Tree Builder class implementation
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "derivation.h"
#include "langcommon.h"

using namespace elena_lang;

//inline void testNodes(SyntaxNode node)
//{
//   SyntaxNode current = node.firstChild();
//   while (current != SyntaxKey::None) {
//      testNodes(current);
//
//      current = current.nextNode();
//   }
//}

inline bool testNodeMask(SyntaxKey key, SyntaxKey mask)
{
   return test((unsigned int)key, (unsigned int)mask);
}

inline ustr_t retrievePath(SyntaxNode node)
{
   while (node != SyntaxKey::None) {
      if (node == SyntaxKey::Namespace) {
         SyntaxNode sourceNode = node.findChild(SyntaxKey::SourcePath);
         if (sourceNode != SyntaxKey::None)
            return sourceNode.identifier();
      }

      node = node.parentNode();
   }

   return "<undefined>";
}

void SyntaxTreeBuilder :: flushNode(Scope& scope, SyntaxNode node)
{
   SyntaxTree::copyNewNode(_writer, node);

   if (!testNodeMask(node.key, SyntaxKey::TerminalMask) || !scope.ignoreTerminalInfo) {
      flushCollection(scope, node);
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder::flushCollection(Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushNode(scope, current);

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flush(SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::MetaDictionary:
            flushDictionary(current);
            break;
         case SyntaxKey::MetaExpression:
         {
            Scope scope;
            flushStatement(scope, current);
            break;
         }
         case SyntaxKey::Declaration:
            flushDeclaration(current);
            break;
         default:
            SyntaxTree::copyNewNode(_writer, current);

            flush(current);

            _writer.closeNode();
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushIdentifier(SyntaxNode identNode, bool ignoreTerminalInfo)
{
   SyntaxTree::copyNewNode(_writer, identNode);

   if (!ignoreTerminalInfo)
      SyntaxTree::copyNode(_writer, identNode);

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushObject(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);

   SyntaxNode current = node.firstChild();
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != identNode) {
      flushAttribute(scope, current, identNode, attributeCategory);

      current = current.nextNode();
   }

   ref_t parameterIndex = 0;
   if (current ==  SyntaxKey::identifier && scope.isParameter(current, parameterIndex)) {
      _writer.newNode(SyntaxKey::TemplateParameter, parameterIndex);
      flushIdentifier(current, scope.ignoreTerminalInfo);
      _writer.closeNode();
   }
   else flushNode(scope, current);

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushMessage(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushNode(scope, current);

      current = current.nextNode();
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushExpression(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Object:
            flushObject(scope, current);
            break;
         case SyntaxKey::Message:
            flushMessage(scope, current);
            break;
         case SyntaxKey::Expression:
         case SyntaxKey::AssignOperation:
         case SyntaxKey::IndexerOperation:
         case SyntaxKey::AddAssignOperation:
         case SyntaxKey::MessageOperation:
            flushExpression(scope, current);
            break;
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushDictionary(SyntaxNode node)
{
   _writer.newNode(node.key);

   Scope scope;
   flushDescriptor(scope, node);

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushDescriptor(Scope& scope, SyntaxNode node, bool withNameNode)
{
   SyntaxNode nameNode = node.lastChild(SyntaxKey::TerminalMask);

   SyntaxNode current = node.firstChild();
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != nameNode) {
      flushAttribute(scope, current, nameNode, attributeCategory);

      current = current.nextNode();
   }

   if (nameNode != SyntaxKey::None) {
      if (withNameNode) {
         _writer.newNode(SyntaxKey::Name);
         flushNode(scope, current);
         _writer.closeNode();
      }
      else flushNode(scope, current);
   }
}

void SyntaxTreeBuilder :: flushAttribute(Scope& scope, SyntaxNode node, SyntaxNode nameNode, ref_t& previusCategory)
{
   bool allowType = nameNode.key == SyntaxKey::None || node.nextNode() == nameNode;

   ref_t attrRef = mapAttribute(node, allowType, previusCategory);
   if (isPrimitiveRef(attrRef)) {
      _writer.newNode(SyntaxKey::Attribute, attrRef);
      flushNode(scope, node);
      _writer.closeNode();
   }
   else if (attrRef != 0 || allowType) {
      _writer.newNode(SyntaxKey::Type, attrRef);
      flushNode(scope, node);
      _writer.closeNode();
   }
   else _errorProcessor->raiseTerminalWarning(WARNING_LEVEL_2, wrnUnknownHint, retrievePath(node), node);
}

void SyntaxTreeBuilder :: flushTemplateArg(Scope& scope, SyntaxNode node, bool allowType)
{
   _writer.newNode(SyntaxKey::TemplateArg);

   if (allowType) {
      SyntaxNode current = node.firstChild();

      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != SyntaxKey::None) {
         flushAttribute(scope, current, {}, attributeCategory);

         current = current.nextNode();
      }
   }
   else {
      SyntaxNode nameNode = node.lastChild(SyntaxKey::TerminalMask);
      SyntaxNode current = node.firstChild();

      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != nameNode) {
         flushAttribute(scope, current, nameNode, attributeCategory);

         current = current.nextNode();
      }

      flushNode(scope, nameNode);
   }


   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplageExpression(Scope& scope, SyntaxNode node, SyntaxKey type, bool allowType)
{
   _writer.newNode(type);

   flushDescriptor(scope, node, false);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::TemplateArg) {
         flushTemplateArg(scope, current, allowType);
      }

      current = current.nextNode();
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushImports(Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Postfix) {
         flushTemplageExpression(scope, current, SyntaxKey::InlineTemplate, false);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushStatement(Scope& scope, SyntaxNode node)
{
   flushExpression(scope, node);
}

void SyntaxTreeBuilder :: flushMethodMember(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   flushDescriptor(scope, node);
   flushImports(scope, node);

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushMethodCode(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         flushStatement(scope, current);
      }

      current = current.nextNode();
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushMethod(Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Parameter:
            flushMethodMember(scope, current);
            break;
         case SyntaxKey::CodeBlock:
         case SyntaxKey::WithoutBody:
            flushMethodCode(scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushClassMember(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   flushDescriptor(scope, node);
   flushImports(scope, node);

   SyntaxNode member = node.firstChild(SyntaxKey::MemberMask);
   switch (member.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::WithoutBody:
         _writer.CurrentNode().setKey(SyntaxKey::Method);
         flushMethod(scope, node);
         break;
      default:
         break;
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushClass(Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Declaration) {
         flushClassMember(scope, current);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushTemplateCode(Scope& scope, SyntaxNode node)
{
   _writer.newNode(node.key);

   SyntaxNode current = node.firstChild();

   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::IncludeStatement:
            flushObject(scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   _writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplateArgDescr(Scope& scope, SyntaxNode node)
{
   SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);
   if(!scope.parameters.add(identNode.identifier(), scope.parameters.count() + 1, true)) {
      _errorProcessor->raiseTerminalError(errDuplicatedDefinition, retrievePath(node), node);
   }

   SyntaxTree::copyNode(_writer, node, true);
}

void SyntaxTreeBuilder :: flushInlineTemplate(Scope& scope, SyntaxNode node)
{
   scope.type = ScopeType::InlineTemplate;
   scope.ignoreTerminalInfo = true;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            flushTemplateArgDescr(scope, current);
            break;
         case SyntaxKey::CodeBlock:
            flushTemplateCode(scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushTemplate(Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            flushNode(scope, current);
            break;
         case SyntaxKey::CodeBlock:
            flushNode(scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

}

void SyntaxTreeBuilder :: flushDeclaration(SyntaxNode node)
{
   Scope scope;

   _writer.newNode(node.key);

   flushDescriptor(scope, node);

   if(node.existChild(SyntaxKey::Expression)) {
      _writer.CurrentNode().setKey(SyntaxKey::Symbol);

      flushStatement(scope, node.findChild(SyntaxKey::Expression));
   }
   else if (node.existChild(SyntaxKey::TemplateArg)) {
      SyntaxNode body = node.firstChild(SyntaxKey::MemberMask);
      switch (body.key) {
         case SyntaxKey::CodeBlock:
            // if it is a code snipshot
            _writer.CurrentNode().setKey(SyntaxKey::TemplateCode);
            flushInlineTemplate(scope, node);
            break;
         default:
            //_writer.CurrentNode().setKey(SyntaxKey::Template);
            //flushTemplate(node);
            break;
      }
      
   }
   else { 
      _writer.CurrentNode().setKey(SyntaxKey::Class);

      flushClass(scope, node);
   }

   _writer.closeNode();   
}

ref_t SyntaxTreeBuilder :: mapAttribute(SyntaxNode terminal, bool allowType, ref_t& previusCategory)
{
   ref_t attrRef = 0;

   ustr_t token = terminal.identifier();

   ref_t ref = _moduleScope->attributes.get(token);
   if (isPrimitiveRef(ref)) {
      // Compiler magic : check if the attribute have correct order
      if ((ref & V_CATEGORY_MASK) < previusCategory) {
         previusCategory = ref & V_CATEGORY_MASK;
      }
      else ref = 0u;
   }

   if (allowType) {
      //if (!isPrimitiveRef(ref))
      //   attrRef = ref;

      if (/*attrRef || */!ref)
         return attrRef;
   }

   if (!isPrimitiveRef(ref) && !allowType)
      _errorProcessor->raiseTerminalError(errInvalidHint, retrievePath(terminal), terminal);

   return ref;
}

//void SyntaxTreeBuilder :: recognizeDeclarationAttributes(SyntaxNode node)
//{
//   recognizeAttributes(node, node.lastChild(SyntaxKey::TerminalMask));
//}
//
//SyntaxTreeBuilder::ScopeType SyntaxTreeBuilder :: recognizeDeclaration(SyntaxNode node)
//{
//   recognizeDeclarationAttributes(node);
//
//   ScopeType type = ScopeType::Unknown;
//
//
//   return type;
//}

void SyntaxTreeBuilder :: newNode(parse_key_t key)
{
   SyntaxKey syntaxKey = SyntaxTree::fromParseKey(key);

   _level++;

   _cacheWriter.newNode(syntaxKey);
}

void SyntaxTreeBuilder :: newNode(parse_key_t key, ustr_t arg)
{
   newNode(key);

   _cacheWriter.CurrentNode().setStrArgument(arg);
}

void SyntaxTreeBuilder :: appendTerminal(parse_key_t key, ustr_t value, LineInfo lineInfo)
{
   SyntaxKey syntaxKey = SyntaxTree::fromParseKey(key);

   switch (syntaxKey) {
      case SyntaxKey::string:
      {
         QuoteString quote(value, value.length_pos());

         _cacheWriter.newNode(syntaxKey, quote.str());
         break;
      }
      default:
         _cacheWriter.newNode(syntaxKey, value);
         break;
   }
   
   _cacheWriter.appendNode(SyntaxKey::Column, lineInfo.column);
   _cacheWriter.appendNode(SyntaxKey::Row, lineInfo.row);
   _cacheWriter.closeNode();
}

void SyntaxTreeBuilder :: injectNode(parse_key_t key)
{
   SyntaxNode current = _cacheWriter.CurrentNode();
   current.injectNode(SyntaxTree::fromParseKey(key));
}  

void SyntaxTreeBuilder :: closeNode()
{
   _level--;

   _cacheWriter.closeNode();
   if (_level == 0) {
      flush(_cacheWriter.CurrentNode());

      _cacheWriter.clear();
      _cacheWriter.newNode(SyntaxKey::Root);
   }
}

// --- TemplateProssesor ---

void TemplateProssesor :: copyNode(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   switch (node.key) {
      case SyntaxKey::TemplateParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.parameterValues.get(node.arg.reference);
            copyChildren(writer, scope, nodeToInject);
         }
         else {
            writer.newNode(node.key, node.arg.reference - 0x100);
            copyChildren(writer, scope, node);
            writer.closeNode();
         }
         break;
      default:
         SyntaxTree::copyNewNode(writer, node);
         copyChildren(writer, scope, node);
         writer.closeNode();
         break;
   }
}

void TemplateProssesor :: copyChildren(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      copyNode(writer, scope, current);

      current = current.nextNode();
   }
}

void TemplateProssesor :: generate(SyntaxTreeWriter& writer, TemplateScope& scope, MemoryBase* templateSection)
{
   SyntaxTree templateTree;
   templateTree.load(templateSection);

   SyntaxNode root = templateTree.readRoot();

   switch (scope.type) {
      case Type::Inline:
         copyChildren(writer, scope, root.findChild(SyntaxKey::CodeBlock));
         break;
      default:
         break;
   }
}

bool TemplateProssesor :: importTemplate(Type type, ModuleScopeBase& moduleScope, ref_t templateRef, 
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   TemplateScope scope(type);
   for(auto it = parameters.start(); !it.eof(); ++it) {
      scope.parameterValues.add(scope.parameterValues.count() + 1, *it);
   }

   SyntaxTree bufferTree;
   SyntaxTreeWriter bufferWriter(bufferTree);

   bufferWriter.newNode(SyntaxKey::Root);

   auto templateSection = moduleScope.mapSection(templateRef, true);
   if (templateSection != nullptr) {
      generate(bufferWriter, scope, templateSection);
   }
   else return false;

   bufferWriter.closeNode();

   SyntaxTreeWriter targetWriter(target);
   SyntaxTree::copyNode(targetWriter, bufferTree.readRoot());

   return true;
}

bool TemplateProssesor :: importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, 
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   return importTemplate(Type::Inline, moduleScope, templateRef, target, parameters);
}
