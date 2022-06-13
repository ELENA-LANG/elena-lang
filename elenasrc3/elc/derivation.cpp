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

void SyntaxTreeBuilder :: flushNode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxTree::copyNewNode(writer, node);

   if (!testNodeMask(node.key, SyntaxKey::TerminalMask) || !scope.ignoreTerminalInfo) {
      flushCollection(writer, scope, node);
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushCollection(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushNode(writer, scope, current);

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushNamespace(SyntaxTreeWriter& writer, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::MetaDictionary:
            flushDictionary(writer, current);
            break;
         case SyntaxKey::MetaExpression:
         {
            Scope scope;
            flushStatement(writer, scope, current);
            break;
         }
         case SyntaxKey::Declaration:
            flushDeclaration(writer, current);
            break;
         default:
            break;
      }
      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flush(SyntaxTreeWriter& writer, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::MetaDictionary:
            flushDictionary(writer, current);
            break;
         case SyntaxKey::MetaExpression:
         {
            Scope scope;
            flushStatement(writer, scope, current);
            break;
         }
         case SyntaxKey::Declaration:
            flushDeclaration(writer, current);
            break;
         default:
            SyntaxTree::copyNewNode(writer, current);

            flush(writer, current);

            writer.closeNode();
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: parseStatement(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode current, 
   List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   scope.nestedLevel += 0x100;
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Expression:
            writer.newNode(SyntaxKey::Idle);
            flushExpression(writer, scope, current);
            arguments.add(writer.CurrentNode().firstChild());
            writer.closeNode();
            break;
         case SyntaxKey::TExpression:
            // unpacking the statement body
            //flushExpression();
            writer.newNode(SyntaxKey::Idle);
            flushExpression(writer, scope, current.firstChild());
            parameters.add(writer.CurrentNode().firstChild());
            writer.closeNode();
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
   scope.nestedLevel -= 0x100;
}

void SyntaxTreeBuilder :: generateTemplateStatement(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode objNode = node.findChild(SyntaxKey::Object);
   SyntaxNode current = objNode.nextNode();

   IdentifierString templateName;

   List<SyntaxNode> arguments({});
   List<SyntaxNode> parameters({});

   // generate template arguments
   SyntaxTree tempTree;
   SyntaxTreeWriter tempWriter(tempTree);
   parseStatement(tempWriter, scope, current, arguments, parameters);

   templateName.appendInt(arguments.count());
   templateName.append('#');
   templateName.appendInt(parameters.count());
   templateName.append('#');
   templateName.append(objNode.firstChild(SyntaxKey::identifier).identifier());

   ref_t templateRef = _moduleScope->operations.get(*templateName);

   if(_templateProcessor->importCodeTemplate(*_moduleScope, templateRef, writer.CurrentNode(), 
      arguments, parameters))
   {
   }
   else {
      _errorProcessor->raiseTerminalError(errInvalidOperation, retrievePath(node), node);
   }
      
}

void SyntaxTreeBuilder :: flushIdentifier(SyntaxTreeWriter& writer, SyntaxNode identNode, bool ignoreTerminalInfo)
{
   SyntaxTree::copyNewNode(writer, identNode);

   if (!ignoreTerminalInfo)
      SyntaxTree::copyNode(writer, identNode);

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplateType(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode objNode = node.findChild(SyntaxKey::Object);
   SyntaxNode argNode = objNode.nextNode();

   SyntaxNode current = objNode.firstChild();
   SyntaxNode identNode = objNode.lastChild(SyntaxKey::TerminalMask);

   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != identNode) {
      bool allowType = current.nextNode() == identNode;

      flushAttribute(writer, scope, current, attributeCategory, allowType);

      current = current.nextNode();
   }

   SyntaxKey parameterKey;
   ref_t parameterIndex = 0;
   if (current == SyntaxKey::identifier && scope.isParameter(current, parameterKey, parameterIndex)) {
      writer.newNode(parameterKey, parameterIndex);
      flushIdentifier(writer, current, scope.ignoreTerminalInfo);
      writer.closeNode();
   }
   else flushNode(writer, scope, current);

   current = argNode;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::L6Expression) {
         writer.newNode(SyntaxKey::TemplateArg);
         flushCollection(writer, scope, current);
         writer.closeNode();
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushObject(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   if (current == SyntaxKey::TemplateType) {
      if (current.nextNode() == SyntaxKey::identifier) {
         writer.newNode(SyntaxKey::Type);
         flushTemplateType(writer, scope, current);
         writer.closeNode();
      }
      else flushTemplateType(writer, scope, current);
   }
   else {
      SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);

      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != identNode) {
         bool allowType = current.nextNode() == identNode;

         flushAttribute(writer, scope, current, attributeCategory, allowType);

         current = current.nextNode();
      }

      SyntaxKey parameterKey = SyntaxKey::None;
      ref_t parameterIndex = 0;
      if (current == SyntaxKey::identifier && scope.isParameter(current, parameterKey, parameterIndex)) {
         writer.newNode(parameterKey, parameterIndex);
         flushIdentifier(writer, current, scope.ignoreTerminalInfo);
         writer.closeNode();
      }
      else flushNode(writer, scope, current);
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushNested(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::NestedExpression) {
         flushClass(writer, scope, current, false);
      }
      else flushAttribute(writer, scope, current, attributeCategory, true);

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushMessage(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushNode(writer, scope, current);

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushExpression(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Object:
            flushObject(writer, scope, current);
            break;
         case SyntaxKey::NestedBlock:
            flushNested(writer, scope, current);
            break;
         case SyntaxKey::ClosureBlock:
            flushClosure(writer, scope, current);
            break;
         case SyntaxKey::Message:
            flushMessage(writer, scope, current);
            break;
         case SyntaxKey::TemplateCode:
            writer.CurrentNode().setKey(SyntaxKey::CodeBlock);
            generateTemplateStatement(writer, scope, current);
            break;
         default:
            if (SyntaxTree::testSuperKey(current.key, SyntaxKey::Expression)) {
               current.setKey(SyntaxKey::Expression);
               flushExpression(writer, scope, current);
            }
            else if (SyntaxTree::test(current.key, SyntaxKey::ScopeMask)) {
               flushExpression(writer, scope, current);
            }
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushDictionary(SyntaxTreeWriter& writer, SyntaxNode node)
{
   writer.newNode(node.key);

   Scope scope;
   flushDescriptor(writer, scope, node);

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushDescriptor(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool withNameNode, 
   bool typeDescriptor)
{
   SyntaxNode nameNode = node.lastChild(SyntaxKey::TerminalMask);
   if (typeDescriptor)
      nameNode = nameNode.nextNode();

   SyntaxNode current = node.firstChild();
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != nameNode) {
      bool allowType = nameNode.key == SyntaxKey::None || current.nextNode() == nameNode;

      if (current == SyntaxKey::ArrayType) {
         flushTypeAttribute(writer, scope, current, attributeCategory, allowType);
      }
      else flushAttribute(writer, scope, current, attributeCategory, allowType);

      current = current.nextNode();
   }

   if (!typeDescriptor && nameNode != SyntaxKey::None) {
      if (withNameNode) {
         writer.newNode(SyntaxKey::Name);
         flushNode(writer, scope, current);
         writer.closeNode();
      }
      else flushNode(writer, scope, current);
   }
}

void SyntaxTreeBuilder :: flushAttribute(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, 
   ref_t& previusCategory, bool allowType)
{
   ref_t attrRef = mapAttribute(node, allowType, previusCategory);
   if (isPrimitiveRef(attrRef)) {
      writer.newNode(SyntaxKey::Attribute, attrRef);
      flushNode(writer, scope, node);
      writer.closeNode();
   }
   else if (attrRef != 0 || allowType) {
      SyntaxKey key = SyntaxKey::Type;
      if (scope.withTypeParameters()) {
         int index = scope.arguments.get(node.identifier());
         if (index != 0) {
            key = SyntaxKey::TemplateArgParameter;
            attrRef = index + scope.nestedLevel;
         }
      }

      writer.newNode(key, attrRef);
      flushNode(writer, scope, node);
      writer.closeNode();
   }
   else _errorProcessor->raiseTerminalWarning(WARNING_LEVEL_2, wrnUnknownHint, retrievePath(node), node);
}

void SyntaxTreeBuilder :: flushTypeAttribute(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, 
   ref_t& previusCategory, bool allowType)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::ArrayType) {
         flushTypeAttribute(writer, scope, current, previusCategory, allowType);
      }
      else flushAttribute(writer, scope, current, previusCategory, allowType);

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplateArg(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool allowType)
{
   writer.newNode(SyntaxKey::TemplateArg);

   if (allowType) {
      SyntaxNode current = node.firstChild();

      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != SyntaxKey::None) {
         flushAttribute(writer, scope, current, attributeCategory, true);

         current = current.nextNode();
      }
   }
   else {
      SyntaxNode nameNode = node.lastChild(SyntaxKey::TerminalMask);
      SyntaxNode current = node.firstChild();

      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != nameNode) {
         bool allowType = current.nextNode() == nameNode;
         flushAttribute(writer, scope, current, attributeCategory, allowType);

         current = current.nextNode();
      }

      flushNode(writer, scope, nameNode);
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplageExpression(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, 
   SyntaxKey type, bool allowType)
{
   writer.newNode(type);

   flushDescriptor(writer, scope, node, false);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::TemplateArg) {
         flushTemplateArg(writer, scope, current, allowType);
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushClassMemberPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Postfix) {
         SyntaxNode child = current.firstChild();
         if (child == SyntaxKey::TemplatePostfix) {
            flushTemplageExpression(writer, scope, child, SyntaxKey::InlineTemplate, false);
         }
         else throw InternalError(errFatalError);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushClassPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Postfix) {
         SyntaxNode child = current.firstChild();
         if (current == SyntaxKey::TemplatePostfix) {
            throw InternalError(errFatalError);
            //flushTemplageExpression(scope, current, SyntaxKey::InlineTemplate, false);
         }
         else {
            writer.newNode(SyntaxKey::Parent);
            flushCollection(writer, scope, current);
            writer.closeNode();
         }
         
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushStatement(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   flushExpression(writer, scope, node);
}

void SyntaxTreeBuilder :: flushMethodMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   flushDescriptor(writer, scope, node);
   //flushImports(scope, node);

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushMethodCode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::EOP:
            flushNode(writer, scope, current);
            break;
         default:
            if (SyntaxTree::testSuperKey(current.key, SyntaxKey::Expression)) {
               current.setKey(SyntaxKey::Expression);
               flushStatement(writer, scope, current);
            }
            else if (SyntaxTree::test(current.key, SyntaxKey::ScopeMask)) {
               flushStatement(writer, scope, current);
            }
            break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushClosure(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   flushMethodCode(writer, scope, node.firstChild(SyntaxKey::ScopeMask));

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushMethod(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Parameter:
            flushMethodMember(writer, scope, current);
            break;
         case SyntaxKey::CodeBlock:
         case SyntaxKey::WithoutBody:
         case SyntaxKey::ReturnExpression:
            flushMethodCode(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: copyHeader(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushNode(writer, scope, current);

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushSubScopeMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, SyntaxNode headerNode)
{
   SyntaxNode member = node.firstChild(SyntaxKey::MemberMask);
   switch (member.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::WithoutBody:
      case SyntaxKey::ReturnExpression:
         writer.newNode(SyntaxKey::Method);

         flushDescriptor(writer, scope, node, true, true);
         copyHeader(writer, scope, headerNode);

         flushMethod(writer, scope, node);

         writer.closeNode();
         break;
      default:
         break;
   }
}

void SyntaxTreeBuilder :: flushSubScope(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, SyntaxNode headerNode)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Declaration) {
         flushSubScopeMember(writer, scope, current, headerNode);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushClassMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool functionMode)
{
   writer.newNode(node.key);

   if (!functionMode) {
      flushDescriptor(writer,  scope, node);
      flushClassMemberPostfixes(writer, scope, node);
   }
   else writer.appendNode(SyntaxKey::Attribute, V_FUNCTION);

   SyntaxNode member = node.firstChild(SyntaxKey::MemberMask);
   switch (member.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::WithoutBody:
      case SyntaxKey::ReturnExpression:
         writer.CurrentNode().setKey(SyntaxKey::Method);
         flushMethod(writer, scope, node);
         break;
      case SyntaxKey::GetExpression:
         writer.CurrentNode().setKey(SyntaxKey::Method);
         writer.appendNode(SyntaxKey::Attribute, V_GETACCESSOR);

         member.setKey(SyntaxKey::ReturnExpression);

         flushMethod(writer, scope, node);
         break;
      case SyntaxKey::Declaration:
      {
         SyntaxNode headerNode = writer.CurrentNode();
         writer.closeNode();

         flushSubScope(writer, scope, node, headerNode);
         return;
      }
      case SyntaxKey::InitExpression:
      {
         SyntaxNode nameNode = writer.CurrentNode().findChild(SyntaxKey::Name);

         writer.CurrentNode().setKey(SyntaxKey::Field);
         writer.closeNode();
         writer.newNode(SyntaxKey::AssignOperation);
         flushCollection(writer, scope, nameNode);
         flushExpression(writer, scope, node.findChild(SyntaxKey::InitExpression).firstChild());
         break;
      }
      case SyntaxKey::Dimension:
         flushNode(writer, scope, member);
      default:
         writer.CurrentNode().setKey(SyntaxKey::Field);
         break;
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushClass(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool functionMode)
{
   flushClassPostfixes(writer, scope, node);

   if (functionMode) {
      writer.appendNode(SyntaxKey::Attribute, V_SINGLETON);

      flushClassMember(writer, scope, node, true);
   }
   else {
      SyntaxNode current = node.firstChild();
      while (current != SyntaxKey::None) {
         if (current.key == SyntaxKey::Declaration) {
            flushClassMember(writer, scope, current);
         }

         current = current.nextNode();
      }
   }
}

void SyntaxTreeBuilder :: flushTemplateCode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();

   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::IncludeStatement:
            flushObject(writer, scope, current);
            break;
         case SyntaxKey::MetaExpression:
            flushStatement(writer, scope, current);
            break;
         case SyntaxKey::EOP:
            flushNode(writer, scope, current);
            break;
         default:
            if (SyntaxTree::testSuperKey(current.key, SyntaxKey::Expression)) {
               current.setKey(SyntaxKey::Expression);
               flushStatement(writer, scope, current);
            }
            else if (SyntaxTree::test(current.key, SyntaxKey::ScopeMask)) {
               flushStatement(writer, scope, current);
            }
            break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplateArgDescr(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);
   if(!scope.arguments.add(identNode.identifier(), scope.arguments.count() + 1, true)) {
      _errorProcessor->raiseTerminalError(errDuplicatedDefinition, retrievePath(node), node);
   }

   SyntaxTree::copyNode(writer, node, true);
}

void SyntaxTreeBuilder :: flushParameterArgDescr(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);
   if (!scope.parameters.add(identNode.identifier(), scope.parameters.count() + 1, true)) {
      _errorProcessor->raiseTerminalError(errDuplicatedDefinition, retrievePath(node), node);
   }

   SyntaxTree::copyNode(writer, node, true);
}

void SyntaxTreeBuilder :: flushInlineTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   scope.type = ScopeType::InlineTemplate;
   scope.ignoreTerminalInfo = true;

   flushClassMemberPostfixes(writer, scope, node);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            flushTemplateArgDescr(writer,  scope, current);
            break;
         case SyntaxKey::Parameter:
            flushParameterArgDescr(writer, scope, current);
            break;
         case SyntaxKey::CodeBlock:
            flushTemplateCode(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   scope.type = ScopeType::ClassTemplate;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            flushTemplateArgDescr(writer, scope, current);
            break;
         case SyntaxKey::CodeBlock:
            flushNode(writer, scope, current);
            break;
         case SyntaxKey::Declaration:
            flushClassMember(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

enum DeclarationType
{
   Class,
   Import,
   Namespace
};

inline DeclarationType defineDeclarationType(SyntaxNode node)
{
   DeclarationType type = DeclarationType::Class;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         switch (current.arg.reference) {
            case V_IMPORT:
               type = DeclarationType::Import;
               break;
            case V_NAMESPACE:
               type = DeclarationType::Namespace;
               break;
            default:
               break;
         }
      }

      current = current.nextNode();
   }

   return type;
}

void SyntaxTreeBuilder :: flushDeclaration(SyntaxTreeWriter& writer, SyntaxNode node)
{
   Scope scope;

   writer.newNode(node.key);

   flushDescriptor(writer, scope, node);

   if(node.existChild(SyntaxKey::GetExpression)) {
      writer.CurrentNode().setKey(SyntaxKey::Symbol);

      flushStatement(writer, scope, node.findChild(SyntaxKey::GetExpression));
   }
   else if (node.existChild(SyntaxKey::TemplateArg)) {
      SyntaxNode body = node.firstChild(SyntaxKey::MemberMask);
      switch (body.key) {
         case SyntaxKey::CodeBlock:
            // if it is a code snipshot
            writer.CurrentNode().setKey(SyntaxKey::TemplateCode);
            flushInlineTemplate(writer, scope, node);
            break;
         case SyntaxKey::Declaration:
            writer.CurrentNode().setKey(SyntaxKey::Template);
            flushTemplate(writer, scope, node);
            break;
         default:
            break;
      }
      
   }
   else {
      DeclarationType type = defineDeclarationType(writer.CurrentNode());
      switch (type) {
         case DeclarationType::Import:
            writer.CurrentNode().setKey(SyntaxKey::Import);
            break;
         case DeclarationType::Namespace:
            writer.CurrentNode().setKey(SyntaxKey::Namespace);

            flushNamespace(writer, node);
            break;
         case DeclarationType::Class:
            writer.CurrentNode().setKey(SyntaxKey::Class);

            flushClass(writer, scope, node, node.existChild(SyntaxKey::CodeBlock));
            break;
         default:
            break;
      }
   }

   writer.closeNode();   
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
      flush(_writer, _cacheWriter.CurrentNode());

      _cacheWriter.clear();
      _cacheWriter.newNode(SyntaxKey::Root);
   }
}

// --- TemplateProssesor ---

void TemplateProssesor :: copyNode(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   switch (node.key) {
      case SyntaxKey::TemplateArgParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.argValues.get(node.arg.reference);
            if (nodeToInject.key != SyntaxKey::TemplateArg)
               writer.CurrentNode().setKey(nodeToInject.key);

            copyChildren(writer, scope, nodeToInject);
         }
         else {
            writer.newNode(node.key, node.arg.reference - 0x100);
            copyChildren(writer, scope, node);
            writer.closeNode();
         }
         break;
      case SyntaxKey::TemplateParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.parameterValues.get(node.arg.reference);
            writer.CurrentNode().setKey(nodeToInject.key);
            copyChildren(writer, scope, nodeToInject);
         }
         else {
            writer.newNode(node.key, node.arg.reference - 0x100);
            copyChildren(writer, scope, node);
            writer.closeNode();
         }
         break;
      case SyntaxKey::TemplateArg:
         writer.newNode(SyntaxKey::Type);
         copyChildren(writer, scope, node);
         writer.closeNode();
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
      case Type::CodeTemplate:
         copyChildren(writer, scope, root.findChild(SyntaxKey::CodeBlock));
         break;
      default:
         break;
   }
}

void TemplateProssesor :: loadArguments(TemplateScope& scope, List<SyntaxNode>* parameters)
{
   for (auto it = parameters->start(); !it.eof(); ++it) {
      scope.argValues.add(scope.argValues.count() + 1, *it);
   }
}

void TemplateProssesor :: loadParameters(TemplateScope& scope, List<SyntaxNode>* parameters)
{
   for (auto it = parameters->start(); !it.eof(); ++it) {
      scope.parameterValues.add(scope.parameterValues.count() + 1, *it);
   }
}

void TemplateProssesor :: importTemplate(Type type, MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>* arguments, List<SyntaxNode>* parameters)
{
   TemplateScope scope(type, nullptr, 0);
   if (arguments)
      loadArguments(scope, arguments);
   if (parameters)
      loadParameters(scope, parameters);

   SyntaxTree bufferTree;
   SyntaxTreeWriter bufferWriter(bufferTree);

   bufferWriter.newNode(SyntaxKey::Root);

   generate(bufferWriter, scope, templateSection);

   bufferWriter.closeNode();

   SyntaxTreeWriter targetWriter(target);
   SyntaxTree::copyNode(targetWriter, bufferTree.readRoot());
}

void TemplateProssesor :: importInlineTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   importTemplate(Type::Inline, templateSection, target, &parameters, nullptr);
}

void TemplateProssesor :: importCodeTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   importTemplate(Type::CodeTemplate, templateSection, target, &arguments, &parameters);
}

void TemplateProssesor :: copyField(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   writer.newNode(node.key);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArgParameter:
         {
            SyntaxNode nodeToInject = scope.argValues.get(current.arg.value);
            copyNode(writer, scope, nodeToInject);
            break;
         }
         default:
            copyNode(writer, scope, current);
            break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void TemplateProssesor :: copyMethod(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   writer.newNode(node.key);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArgParameter:
         {
            SyntaxNode nodeToInject = scope.argValues.get(current.arg.value);
            copyNode(writer, scope, nodeToInject);
            break;
         }
         default:
            copyNode(writer, scope, current);
            break;
      }

      current = current.nextNode();
   }
   writer.closeNode();
}

void TemplateProssesor :: generateTemplate(SyntaxTreeWriter& writer, TemplateScope& scope, MemoryBase* templateBody)
{
   SyntaxTree templateTree;
   templateTree.load(templateBody);

   SyntaxNode rootNode = templateTree.readRoot();
   if (scope.type == Type::Class) {
      ustr_t fullName = scope.moduleScope->resolveFullName(scope.targetRef);

      writer.newNode(SyntaxKey::Class/*, INVALID_REF*/);
      writer.appendNode(SyntaxKey::Name, scope.moduleScope->mapFullReference(fullName, true));
   }

   SyntaxNode current = rootNode.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            copyNode(writer, scope, current);
            break;
         case SyntaxKey::Field:
            copyField(writer, scope, current);
            break;
         case SyntaxKey::Method:
            copyMethod(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   if (scope.type == Type::Class)
      writer.closeNode();
}

void TemplateProssesor :: generateClassTemplate(ModuleScopeBase* moduleScope, ref_t classRef, 
   SyntaxTree* syntaxTree, MemoryBase* sectionBody, List<SyntaxNode>& parameters)
{
   TemplateScope templateScope(Type::Class, moduleScope, classRef);
   loadParameters(templateScope, &parameters);

   SyntaxTreeWriter writer(*syntaxTree);

   generateTemplate(writer, templateScope, sectionBody);
}
