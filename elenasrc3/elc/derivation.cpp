//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Syntax Tree Builder class implementation
//
//                                             (C)2021-2024, by Aleksey Rakov
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

void SyntaxTreeBuilder :: flushNode(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode& node)
{
   SyntaxTree::copyNewNode(writer, node);

   if (!testNodeMask(node.key, SyntaxKey::TerminalMask) || !scope.ignoreTerminalInfo) {
      flushCollection(writer, scope, node);
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushCollection(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode& node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushNode(writer, scope, current);

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushNamespace(SyntaxTreeWriter& writer, SyntaxNode& node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::MetaDictionary:
         case SyntaxKey::SharedMetaDictionary:
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
         case SyntaxKey::SharedMetaDictionary:
            flushDictionary(writer, current);
            break;
         case SyntaxKey::LoadStatement:
            loadMetaSection(current);
            break;
         case SyntaxKey::ClearStatement:
            clearMetaSection(current);
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
         case SyntaxKey::ExternalTree:
            SyntaxTree::copyNode(writer, current);
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
   List<SyntaxNode>& arguments, List<SyntaxNode>& parameters, IdentifierString& postfix)
{
   bool firstExpr = true;
   scope.nestedLevel += 0x100;
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::identifier:
            postfix.append(':');
            postfix.append(current.identifier());
            break;
         //case SyntaxKey::ComplexName:
         //   postfix.append(':');
         //   postfix.append(current.firstChild().identifier());
         //   break;
         case SyntaxKey::Expression:
            if (!firstExpr) {
               // if it is a templare with a complex name
               SyntaxNode objNode = current.firstChild();
               if (objNode != SyntaxKey::Object)
                  objNode = objNode.firstChild();

               if (objNode == SyntaxKey::Object && objNode.firstChild() == SyntaxKey::identifier) {
                  SyntaxNode attrNode = objNode.firstChild();
                  if (attrNode.nextNode() != SyntaxKey::None) {
                     postfix.append(':');
                     postfix.append(attrNode.identifier());
                     attrNode.setKey(SyntaxKey::Idle);
                  }
               }
            }
            else firstExpr = false;

            writer.newNode(SyntaxKey::Idle);
            flushExpression(writer, scope, current);
            arguments.add(writer.CurrentNode().firstChild());
            writer.closeNode();
            break;
         case SyntaxKey::NTExpression:
         case SyntaxKey::TExpression:
         case SyntaxKey::LTExpression:
            // unpacking the statement body
            //flushExpression();
            writer.newNode(SyntaxKey::Idle);
            flushExpressionMember(writer, scope, current.firstChild());
            //flushExpression(writer, scope, current.firstChild());
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
   List<SyntaxNode> arguments({});
   List<SyntaxNode> parameters({});

   IdentifierString templateName;

   SyntaxNode identNode = node.firstChild();
   if (identNode == SyntaxKey::Object) {
      templateName.append(identNode.firstChild(SyntaxKey::identifier).identifier());
   }
   else if (identNode == SyntaxKey::identifier) {
      templateName.append(identNode.identifier());
   }

   SyntaxNode current = identNode.nextNode();

   // generate template arguments
   SyntaxTree tempTree;
   SyntaxTreeWriter tempWriter(tempTree);
   parseStatement(tempWriter, scope, current, arguments, parameters, templateName);

   templateName.append('#');
   templateName.appendInt(arguments.count());
   templateName.append('#');
   templateName.appendInt(parameters.count());

   ref_t templateRef = _moduleScope->operations.get(*templateName);

   if(_templateProcessor->importCodeTemplate(*_moduleScope, templateRef, writer.CurrentNode(), 
      arguments, parameters))
   {
   }
   else {
      _errorProcessor->raiseTerminalError(errInvalidOperation, retrievePath(node), node);
   }
      
}

void SyntaxTreeBuilder :: generateTemplateExpression(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   List<SyntaxNode> arguments({});
   List<SyntaxNode> parameters({});

   IdentifierString templateName(INLINEEXPR_PREFIX);

   SyntaxNode identNode = node.findChild(SyntaxKey::identifier);
   templateName.append(identNode.identifier());

   node.setKey(SyntaxKey::Object);

   // generate template arguments
   SyntaxTree tempTree;
   SyntaxTreeWriter tempWriter(tempTree);
   tempWriter.newNode(SyntaxKey::Idle);

   flushExpression(tempWriter, scope, node.firstChild(SyntaxKey::ScopeMask));
   parameters.add(tempWriter.CurrentNode().firstChild());
   tempWriter.closeNode();

   SyntaxNode current = identNode.nextNode();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            tempWriter.newNode(SyntaxKey::Idle);
            flushTemplateArg(tempWriter, scope, current, true);
            arguments.add(tempWriter.CurrentNode().firstChild());
            tempWriter.closeNode();
            break;
         default:
            break;
      }
      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(arguments.count());
   templateName.append('#');
   templateName.appendInt(parameters.count());

   ref_t templateRef = _moduleScope->operations.get(*templateName);

   if (_templateProcessor->importExpressionTemplate(*_moduleScope, templateRef, writer.CurrentNode(),
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

void SyntaxTreeBuilder :: flushL6AsTemplateArg(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(SyntaxKey::TemplateArg);

   SyntaxNode current = node.firstChild();

   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Object) {
         flushTypeAttribute(writer, scope, current, attributeCategory, true, true);
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplateType(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool exprMode)
{
   if (exprMode) {
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
      if (current == SyntaxKey::identifier && scope.isParameter(current, parameterKey, parameterIndex, true)) {
         writer.newNode(parameterKey, parameterIndex);
         flushIdentifier(writer, current, scope.ignoreTerminalInfo);
         writer.closeNode();
      }
      else flushNode(writer, scope, current);

      current = argNode;
      while (current != SyntaxKey::None) {
         if (current == SyntaxKey::L6Expression) {
            flushL6AsTemplateArg(writer, scope, current);
         }
         else if (current == SyntaxKey::TemplateArg) {
            flushTemplateArg(writer, scope, current, true);
         }

         current = current.nextNode();
      }
   }
   else {
      writer.newNode(node.key);

      flushDescriptor(writer, scope, node, false);
      SyntaxNode current = node.firstChild();
      while (current != SyntaxKey::None) {
         switch (current.key) {
            case SyntaxKey::TemplateArg:
               flushTemplateArg(writer, scope, current, true);
               break;
            default:
               break;
         }

         current = current.nextNode();
      }

      writer.closeNode();
   }
}

void SyntaxTreeBuilder :: flushArrayType(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool exprMode, int nestLevel)
{
   SyntaxNode current = node.firstChild();

   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::ArrayType) {
         flushArrayType(writer, scope, current, exprMode, nestLevel + 1);
      }
      else if (current == SyntaxKey::TemplateType) {
         for (int i = 0; i < nestLevel; i++)
            writer.newNode(SyntaxKey::ArrayType);

         if (exprMode) {
            writer.newNode(SyntaxKey::TemplateType);
            flushTemplateType(writer, scope, current);
            writer.closeNode();
         }
         else flushTemplateType(writer, scope, current, false);

         for (int i = 0; i < nestLevel; i++)
            writer.closeNode();
      }
      else {
         bool allowType = current.nextNode() == SyntaxKey::None;
         flushAttribute(writer, scope, current, attributeCategory, allowType, nestLevel);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushResend(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::MessageOperation || current == SyntaxKey::PropertyOperation) {
         writer.newNode(current.key);

         SyntaxNode identNode = current.lastChild(SyntaxKey::TerminalMask);

         ref_t attributeCategory = V_CATEGORY_MAX;
         SyntaxNode attrNode = current.firstChild();
         while (attrNode != identNode) {
            flushAttribute(writer, scope, attrNode, attributeCategory, false);

            attrNode = attrNode.nextNode();
         }

         writer.newNode(SyntaxKey::Message);
         flushIdentifier(writer, attrNode, scope.ignoreTerminalInfo);
         writer.closeNode();

         attrNode = attrNode.nextNode();
         while (attrNode != SyntaxKey::None) {
            flushExpressionMember(writer, scope, attrNode);

            attrNode = attrNode.nextNode();
         }

         writer.closeNode();
      }
      else flushExpressionMember(writer, scope, current);

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushObject(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   if (current == SyntaxKey::Idle)
      current = current.nextNode();

   if (current == SyntaxKey::TemplateType) {
      writer.newNode(SyntaxKey::TemplateType);
      if (current.nextNode() == SyntaxKey::identifier) {
         SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);
         
         flushTemplateType(writer, scope, current);
         writer.closeNode();

         flushNode(writer, scope, identNode);
      }
      else {
         flushTemplateType(writer, scope, current);

         writer.closeNode();
      }
   }
   else if (current == SyntaxKey::ArrayType) {
      if (current.nextNode() == SyntaxKey::identifier) {
         SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);

         flushArrayType(writer, scope, current, true);

         flushNode(writer, scope, identNode);
      }
      else flushArrayType(writer, scope, current, true);
   }
   else if (current == SyntaxKey::Expression) {
      //HOTFIX : expression cannot be inside an object
      writer.CurrentNode().setKey(SyntaxKey::Expression);
      flushExpressionCollection(writer, scope, current);
   }
   else {
      SyntaxNode identNode = node.lastChild(SyntaxKey::TerminalMask);

      bool typeExpr = false;
      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != identNode) {
         bool allowType = current.nextNode() == identNode;

         typeExpr = flushAttribute(writer, scope, current, attributeCategory, allowType);

         current = current.nextNode();
      }

      SyntaxKey parameterKey = SyntaxKey::None;
      ref_t parameterIndex = 0;
      if (current == SyntaxKey::identifier && scope.isParameter(current, parameterKey, parameterIndex, typeExpr)) {
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
      else if (current == SyntaxKey::TemplateType) {
         writer.newNode(SyntaxKey::TemplateType);
         flushTemplateType(writer, scope, current);
         writer.closeNode();
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

void SyntaxTreeBuilder :: generateTemplateOperation(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   List<SyntaxNode> arguments({});
   List<SyntaxNode> parameters({});

   SyntaxNode operation = node.firstChild();
   SyntaxNode op = operation.findChild(SyntaxKey::MessageOperation, SyntaxKey::PropertyOperation);

   IdentifierString templateName("operator:");
   switch (operation.key) {
      case SyntaxKey::AltOperation:
         templateName.append("alt#1#1");
         break;
      case SyntaxKey::IfNotOperation:
         templateName.append("else#1#1");
         break;
      case SyntaxKey::IfOperation:
         templateName.append("ifnil#1#1");
         break;
      default:
         assert(false);
         break;
   }

   // generate template arguments
   SyntaxTree tempTree;
   SyntaxTreeWriter tempWriter(tempTree);

   tempWriter.newNode(SyntaxKey::Idle);
   flushObject(tempWriter, scope, op.firstChild(SyntaxKey::Object));
   arguments.add(tempWriter.CurrentNode().firstChild());
   tempWriter.closeNode();

   tempWriter.newNode(SyntaxKey::Idle);
   flushExpression(tempWriter, scope, op);
   parameters.add(tempWriter.CurrentNode().firstChild());
   tempWriter.closeNode();

   ref_t templateRef = _moduleScope->operations.get(*templateName);

   if (_templateProcessor->importCodeTemplate(*_moduleScope, templateRef, writer.CurrentNode(),
      arguments, parameters))
   {
      if (writer.CurrentNode() == SyntaxKey::Expression)
         writer.CurrentNode().setKey(SyntaxKey::CodeBlock);

      writer.CurrentNode().appendChild(SyntaxKey::Autogenerated);
   }
   else {
      _errorProcessor->raiseTerminalError(errInvalidOperation, retrievePath(node), node);
   }
}

void SyntaxTreeBuilder :: flushNullable(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode objNode = node.firstChild();
   SyntaxNode current = objNode.nextNode();   

   writer.newNode(SyntaxKey::Object);

   writer.newNode(node.key);
   ref_t attributeCategory = V_CATEGORY_MAX;
   flushTypeAttribute(writer, scope, objNode, attributeCategory, true, true);
   writer.closeNode();

   SyntaxNode subNode = current.firstChild();
   SyntaxNode identNode = subNode.firstChild();
   if (identNode == SyntaxKey::identifier && subNode.nextNode() == SyntaxKey::None) {
      flushIdentifier(writer, identNode, scope.ignoreTerminalInfo);
   }
   else _errorProcessor->raiseTerminalError(errInvalidOperation, retrievePath(node), node);

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushExpressionMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode current)
{
   switch (current.key) {
      case SyntaxKey::Parameter:
         flushMethodMember(writer, scope, current);
         break;
      case SyntaxKey::Object:
      case SyntaxKey::SubVariable:
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
      case SyntaxKey::TemplateExpression:
         writer.CurrentNode().setKey(SyntaxKey::Expression);
         generateTemplateExpression(writer, scope, current);
         break;
      case SyntaxKey::TemplateOperation:
         generateTemplateOperation(writer, scope, current);
         break;
      case SyntaxKey::TupleBlock:
         flushExpression(writer, scope, current);
         break;
      case SyntaxKey::NullableType:
         flushNullable(writer, scope, current);
         break;
      case SyntaxKey::TemplateArg:
         flushTemplateArg(writer, scope, current, true);
         break;
      case SyntaxKey::interpolate:
         flushNode(writer, scope, current);
         break;
      case SyntaxKey::Idle:
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
}

void SyntaxTreeBuilder :: flushExpressionCollection(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      flushExpressionMember(writer, scope, current);

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushExpression(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   flushExpressionCollection(writer, scope, node);

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushDictionary(SyntaxTreeWriter& writer, SyntaxNode& node)
{
   writer.newNode(node.key);

   Scope scope;
   flushDescriptor(writer, scope, node);

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushTupleType(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, ref_t& attributeCategory)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::SubDeclaration) {
         flushTypeAttribute(writer, scope, current, attributeCategory, true, true);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushDescriptor(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode& node, bool withNameNode, 
   bool typeDescriptor, bool exprMode)
{
   SyntaxNode nameNode = node.lastChild(SyntaxKey::TerminalMask);
   if (typeDescriptor) {
      nameNode = nameNode.nextNode();
   }
   else if (nameNode.nextNode() == SyntaxKey::SubDeclaration) {
      nameNode = {};
   }

   SyntaxNode current = node.firstChild();
   ref_t attributeCategory = V_CATEGORY_MAX;
   while (current != nameNode) {
      SyntaxNode nextNode = current.nextNode();
      if (nextNode == SyntaxKey::TemplateArg) {
         writer.newNode(SyntaxKey::TemplateType);

         SyntaxKey parameterKey;
         ref_t parameterIndex = 0;
         if (current == SyntaxKey::identifier && scope.isParameter(current, parameterKey, parameterIndex, false)) {
            writer.newNode(parameterKey, parameterIndex);
            flushIdentifier(writer, current, scope.ignoreTerminalInfo);
            writer.closeNode();
         }
         else flushNode(writer, scope, current);

         current = nextNode;
         while (current == SyntaxKey::TemplateArg) {
            flushTypeAttribute(writer, scope, current, attributeCategory, true);

            current = current.nextNode();
         }

         writer.closeNode();
      }
      else if (current == SyntaxKey::TupleType) {
         flushTupleType(writer, scope, current, attributeCategory);

         current = current.nextNode();
      }
      else if (current == SyntaxKey::SubDeclaration) {
         flushDescriptor(writer, scope, current, withNameNode, typeDescriptor);

         current = current.nextNode();

         // HOTFIX : the last sub declaration contains the name node and should be the last one to analyze
         if (current != SyntaxKey::SubDeclaration)
            break;
      }
      else {
         bool allowType = nameNode.key == SyntaxKey::None || nextNode == nameNode;
         if (current == SyntaxKey::ArrayType) {
            //flushAttribute(writer, scope, current, attributeCategory, allowType, true);
            flushArrayType(writer, scope, current, false);
         }
         else if (current == SyntaxKey::TemplateType) {
            flushTemplateType(writer, scope, current, exprMode);
         }
         else flushAttribute(writer, scope, current, attributeCategory, allowType);

         current = current.nextNode();
      }
   }

   if (!typeDescriptor && nameNode != SyntaxKey::None) {
      if (withNameNode) {
         SyntaxKey key = SyntaxKey::Name;
         ref_t attrRef = 0;

         if (scope.withNameParameters()) {
            int index = scope.arguments.get(current.identifier());
            if (index == 1) {
               key = SyntaxKey::NameArgParameter;
               attrRef = index + scope.nestedLevel;
            }
            else flushNode(writer, scope, current);
         }
         else if (scope.withEnumParameter()) {
            int index = scope.parameters.get(current.identifier());
            if (index) {
               key = SyntaxKey::EnumNameArgParameter;
               attrRef = index + scope.nestedLevel;
            }
         }

         writer.newNode(key, attrRef);
         flushNode(writer, scope, current);
         writer.closeNode();
      }
      else flushNode(writer, scope, current);
   }
}

bool SyntaxTreeBuilder :: flushAttribute(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, 
   ref_t& previusCategory, bool allowType, int arrayNestLevel)
{
   bool typeExpr = false;
   ref_t attrRef = mapAttribute(node, allowType, previusCategory);
   if (isPrimitiveRef(attrRef)) {
      typeExpr = attrRef == V_NEWOP || attrRef == V_WRAPPER || attrRef == V_CONVERSION || attrRef == V_CLASS;

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

      if (arrayNestLevel > 0) {
         for (int i = 0; i < arrayNestLevel; i++)
            writer.newNode(SyntaxKey::ArrayType);
         
         writer.newNode(key, attrRef);
         flushNode(writer, scope, node);
         writer.closeNode();

         for (int i = 0; i < arrayNestLevel; i++)
            writer.closeNode();
      }
      else {
         writer.newNode(key, attrRef);
         flushNode(writer, scope, node);
         writer.closeNode();
      }
   }
   else _errorProcessor->raiseTerminalWarning(WARNING_LEVEL_2, wrnUnknownHint, retrievePath(node), node);

   return typeExpr;
}

void SyntaxTreeBuilder :: flushTypeAttribute(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, 
   ref_t& previusCategory, bool allowType, bool onlyChildren)
{
   if (!onlyChildren)
      writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::ArrayType) {
         flushAttribute(writer, scope, current.firstChild(), previusCategory, allowType, 1);
      }
      else if (current == SyntaxKey::TemplateType) {
         flushTemplateType(writer, scope, current, false);
      }
      else flushAttribute(writer, scope, current, previusCategory, allowType);

      current = current.nextNode();
   }

   if (!onlyChildren)
      writer.closeNode();
}

void SyntaxTreeBuilder :: flushTemplateArg(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool allowType)
{
   writer.newNode(SyntaxKey::TemplateArg);

   if (allowType) {
      SyntaxNode current = node.firstChild();

      ref_t attributeCategory = V_CATEGORY_MAX;
      while (current != SyntaxKey::None) {
         switch (current.key) {
            case SyntaxKey::identifier:
            case SyntaxKey::reference:
            case SyntaxKey::globalreference:
               flushAttribute(writer, scope, current, attributeCategory, true);
               break;
            case SyntaxKey::TemplateType:
               flushTemplateType(writer, scope, current, false);
               break;
            default:
               flushNode(writer, scope, current);
               break;
         }

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
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            flushTemplateArg(writer, scope, current, allowType);
            break;
         case SyntaxKey::Expression:
            flushExpression(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushClassMemberPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node/*, bool ignorePostfix*/)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (/*current.key == SyntaxKey::MethodPostfix || (*/current.key == SyntaxKey::Postfix/* && !ignorePostfix)*/) {
         SyntaxNode child = current.firstChild();
         switch (child.key) {
            case SyntaxKey::InlinePostfix:
               flushTemplageExpression(writer, scope, child, SyntaxKey::InlineTemplate, false);
               break;
            case SyntaxKey::identifier:
            case SyntaxKey::reference:
               flushTemplageExpression(writer, scope, current, SyntaxKey::InlinePropertyTemplate, false);
               break;
            default:
               assert(false);
               //_errorProcessor->raiseTerminalError(errInvalidOperation, retrievePath(node), node);
               break;
         }
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushParentTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::TemplateArg) {
         flushTemplateArg(writer, scope, current, true);
      }
      else flushNode(writer, scope, current);

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushEnumTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   writer.newNode(node.key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::TemplateArg) {
         flushTemplateArg(writer, scope, current, true);
      }
      else if (current == SyntaxKey::Expression) {
         flushExpression(writer, scope, current);
      }
      else flushNode(writer, scope, current);

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushParent(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   ref_t attributeCategory = V_CATEGORY_MAX;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::TemplatePostfix) {
         flushParentTemplate(writer, scope, current);
      }
      else if (current == SyntaxKey::EnumPostfix) {
         flushEnumTemplate(writer, scope, current);
      }
      else if (testNodeMask(current.key, SyntaxKey::TerminalMask)) {
         flushAttribute(writer, scope, current, attributeCategory, current.nextNode() == SyntaxKey::None);
      }
      else if (current == SyntaxKey::TemplateType) {
         flushTemplateType(writer, scope, current, false);
      }
      else flushNode(writer, scope, current);

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushSymbolPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Postfix) {
         flushTemplageExpression(writer, scope, current, SyntaxKey::InlineTemplate, false);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushClassPostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Postfix) {
         if (current.firstChild() == SyntaxKey::InlinePostfix) {
            flushTemplageExpression(writer, scope, current.firstChild(), SyntaxKey::InlineTemplate, false);
         }
         else {
            writer.newNode(SyntaxKey::Parent);
            flushParent(writer, scope, current);
            writer.closeNode();
         }
      }
      else if (current.key == SyntaxKey::IncludeStatement) {
         writer.newNode(SyntaxKey::IncludeStatement);
         writer.newNode(SyntaxKey::Object);
         flushTemplateType(writer, scope, current);
         writer.closeNode();
         writer.closeNode();
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

void SyntaxTreeBuilder :: flushExpressionAsDescriptor(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Object) {
         flushDescriptor(writer, scope, current, true, false, true);
      }
      else _errorProcessor->raiseTerminalError(errInvalidSyntax, retrievePath(node), node);

      current = current.nextNode();
   }

}

void SyntaxTreeBuilder :: flushParameterBlock(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         // HOTFIX : treat an expression as a parameter
         writer.newNode(SyntaxKey::Parameter);
         flushExpressionAsDescriptor(writer, scope, current);
         writer.closeNode();
      }

      current = current.nextNode();
   }
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

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Parameter) {
         flushMethodMember(writer, scope, current);
      }
      else if (current == SyntaxKey::ParameterBlock) {
         flushParameterBlock(writer, scope, current);
      }
      else if (SyntaxTree::test(current.key, SyntaxKey::ScopeMask)) {
         flushMethodCode(writer, scope, current);
         break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

void SyntaxTreeBuilder :: flushMethod(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   if (scope.type != ScopeType::Unknown) {
      ustr_t path = retrievePath(writer.CurrentNode());
      if (!path.empty()) {
         IdentifierString pathStr(_moduleScope->module->name());
         pathStr.append('\'');
         pathStr.append(path);

         writer.appendNode(SyntaxKey::SourcePath, *pathStr);
      }
   }

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Parameter:
            flushMethodMember(writer, scope, current);
            break;
         case SyntaxKey::CodeBlock:
         case SyntaxKey::WithoutBody:
         case SyntaxKey::ReturnExpression:
         case SyntaxKey::Redirect:
            flushMethodCode(writer, scope, current);
            break;
         case SyntaxKey::ResendDispatch:
            flushResend(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

inline bool isTypeRelated(SyntaxNode current)
{
   return current.key == SyntaxKey::Type || current.key == SyntaxKey::ArrayType || current.key == SyntaxKey::TemplateType;
}

bool ifTypeRelatedExists(SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (isTypeRelated(current))
         return true;

      current = current.nextNode();
   }

   return false;
}

void SyntaxTreeBuilder :: copyType(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (isTypeRelated(current)) {
         flushNode(writer, scope, current);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: copyHeader(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool includeType)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (!isTypeRelated(current) || includeType)
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
      {
         writer.newNode(SyntaxKey::Method);
         flushDescriptor(writer, scope, node, true, true);

         // COMPILER MAGIC : recognize property type - if the accessor-method has a parameter - header type should be copied into the parameter
         SyntaxNode paramNode = node.findChild(SyntaxKey::Parameter);
         copyHeader(writer, scope, headerNode, paramNode == SyntaxKey::None);

         flushMethod(writer, scope, node);

         if (paramNode != SyntaxKey::None) {
            // COMPILER MAGIC : recognize property type - if the accessor-method has a parameter - header type should be copied into the parameter
            SyntaxNode targetParamNode = writer.CurrentNode().findChild(SyntaxKey::Parameter);
            if (!ifTypeRelatedExists(targetParamNode)) {
               SyntaxTreeWriter paramWriter(targetParamNode);

               copyType(paramWriter, scope, headerNode);
            }
         }

         writer.closeNode();
         break;
      }
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

inline void copyFunctionAttributes(SyntaxTreeWriter& writer, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Type:
         case SyntaxKey::TemplateType:
         case SyntaxKey::ArrayType:
            SyntaxTree::copyNodeSafe(writer, current, true);
            current.setKey(SyntaxKey::Idle);
            break;
         case SyntaxKey::Attribute:
            if (current.arg.reference == V_FUNCTION) {
               // copy the function attribute
               SyntaxTree::copyNodeSafe(writer, current, true);
               current.setKey(SyntaxKey::Idle);
            }
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushClassMember(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node, bool functionMode)
{
   writer.newNode(node.key);

   if (!functionMode) {
      flushDescriptor(writer,  scope, node);
      flushClassMemberPostfixes(writer, scope, node/*, false*/);
   }
   else {
      writer.appendNode(SyntaxKey::Attribute, V_FUNCTION);

      // HOTFIX : move the type attribute ti the method
      SyntaxNode classNode = writer.CurrentNode().parentNode();

      copyFunctionAttributes(writer, classNode);
   }

   SyntaxNode member = node.firstChild(SyntaxKey::MemberMask);
   switch (member.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::WithoutBody:
      case SyntaxKey::ReturnExpression:
      case SyntaxKey::ResendDispatch:
      case SyntaxKey::Redirect:
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
      case SyntaxKey::AccumExpression:
      {
         bool isInitizializer = SyntaxTree::ifChildExists(writer.CurrentNode(), SyntaxKey::Attribute, V_MEMBER);

         SyntaxNode nameNode = writer.CurrentNode().findChild(SyntaxKey::Name);
         // HOTFIX : if it is an initializer, ignore field
         writer.CurrentNode().setKey(isInitizializer ? SyntaxKey::Idle : SyntaxKey::Field);
         writer.closeNode();

         writer.newNode(member.key == SyntaxKey::InitExpression ? SyntaxKey::AssignOperation : SyntaxKey::AddAssignOperation);
         writer.newNode(SyntaxKey::Object);
         if (isInitizializer)
            writer.appendNode(SyntaxKey::Attribute, V_MEMBER);
         flushCollection(writer, scope, nameNode);
         writer.closeNode();

         flushExpression(writer, scope, node.findChild(member.key).firstChild());
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

void SyntaxTreeBuilder :: flushClass(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode& node, bool functionMode)
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
            flushExpression(writer, scope, current);
            break;
         case SyntaxKey::MetaExpression:
            flushStatement(writer, scope, current);
            break;
         case SyntaxKey::MetaDictionary:
         case SyntaxKey::SharedMetaDictionary:
            flushDictionary(writer, current);
            break;
         case SyntaxKey::EOP:
            //flushNode(writer, scope, current);
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

void SyntaxTreeBuilder :: flushInlineTemplatePostfixes(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Postfix) {
         flushNode(writer, scope, current);
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushExpressionTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode& node)
{
   scope.type = ScopeType::ExpressionTemplate;
   scope.ignoreTerminalInfo = true;

   flushClassMemberPostfixes(writer, scope, node);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            flushTemplateArgDescr(writer, scope, current);
            break;
         case SyntaxKey::Parameter:
            flushParameterArgDescr(writer, scope, current);
            break;
         case SyntaxKey::ReturnExpression:
            flushTemplateCode(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void SyntaxTreeBuilder :: flushInlineTemplate(SyntaxTreeWriter& writer, Scope& scope, SyntaxNode& node)
{
   scope.type = ScopeType::InlineTemplate;
   scope.ignoreTerminalInfo = true;

   //flushInlineTemplatePostfixes(writer, scope, node);
   flushClassMemberPostfixes(writer, scope, node/*, true*/);

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
   // load arguments
   SyntaxNode current = node.findChild(SyntaxKey::TemplateArg);
   if (scope.type == ScopeType::Enumeration) {
      // NOTE : the first argument is the enumeration member
      flushParameterArgDescr(writer, scope, current);

      current = current.nextNode();
   }

   while (current == SyntaxKey::TemplateArg) {
      flushTemplateArgDescr(writer, scope, current);

      current = current.nextNode();
   }

   flushClassPostfixes(writer, scope, node);

   current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
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

void SyntaxTreeBuilder::saveTree(SyntaxTree& tree)
{
   _cacheWriter.newNode(SyntaxKey::ExternalTree);

   SyntaxTree::copyNode(_cacheWriter, tree.readRoot(), false);

   _cacheWriter.closeNode();
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

inline bool isTemplate(SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute && (current.arg.reference == V_INLINE || current.arg.reference == V_TEMPLATE)) {
         return true;
      }
      current = current.nextNode();
   }

   return false;
}

inline bool isTextblock(SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current == SyntaxKey::Attribute) {
      if (current.arg.reference == V_TEXTBLOCK) {
         return true;
      }
      current = current.nextNode();
   }

   return false;
}

inline bool isTemplateDeclaration(SyntaxNode node, SyntaxNode declaration, bool& withComplexName)
{
   bool withPostfix = false;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            return true;
         case SyntaxKey::Postfix:
            withPostfix = true;
            break;
         case SyntaxKey::ComplexName:
            withComplexName = true;
            break;
         case SyntaxKey::Parameter:
            if (withPostfix || isTemplate(declaration)) {
               return true;
            }
            break;
         case SyntaxKey::CodeBlock:
            if (!withPostfix && isTemplate(declaration)) {
               // HOTFIX : recognize inline template with no arguments
               return true;
            }
            break;
         case SyntaxKey::identifier:
            if (isTextblock(declaration)) {
               // HOTFIX : recognize text blocks
               return true;
            }
            break;
         default:
            return false;
      }

      current = current.nextNode();
   }

   return false;
}

SyntaxTreeBuilder::ScopeType SyntaxTreeBuilder :: defineTemplateType(SyntaxNode node)
{
   ScopeType type = ScopeType::ClassTemplate;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         switch (current.arg.reference) {
            case V_FIELD:
               type = ScopeType::PropertyTemplate;
               break;
            case V_EXTENSION:
               type = ScopeType::ExtensionTemplate;
               break;
            case V_ENUMERATION:
               type = ScopeType::Enumeration;
               break;
            case V_TEXTBLOCK:
               type = ScopeType::Textblock;
               break;
            default:
               break;
         }
      }

      current = current.nextNode();
   }

   return type;
}

void SyntaxTreeBuilder :: flushDeclaration(SyntaxTreeWriter& writer, SyntaxNode& node)
{
   Scope scope(_noDebugInfo);

   writer.newNode(node.key);

   flushDescriptor(writer, scope, node);

   bool withComplexName = false;
   if(node.existChild(SyntaxKey::GetExpression)) {
      writer.CurrentNode().setKey(SyntaxKey::Symbol);

      flushSymbolPostfixes(writer, scope, node);

      flushStatement(writer, scope, node.findChild(SyntaxKey::GetExpression));
   }
   else if (isTemplateDeclaration(node, writer.CurrentNode(), withComplexName)) {
      if (withComplexName) {
         SyntaxNode complexName = node.findChild(SyntaxKey::ComplexName);
         while (complexName == SyntaxKey::ComplexName) {
            flushNode(writer, scope, complexName);
            complexName = complexName.nextNode();
         }
      }

      SyntaxNode body = node.firstChild(SyntaxKey::MemberMask);
      switch (body.key) {
         case SyntaxKey::CodeBlock:
            // if it is a code snipshot
            writer.CurrentNode().setKey(SyntaxKey::TemplateCode);
            flushInlineTemplate(writer, scope, node);
            break;
         case SyntaxKey::Declaration:
         case SyntaxKey::None:
            scope.type = defineTemplateType(writer.CurrentNode());

            if (scope.type == ScopeType::ExtensionTemplate) {
               writer.CurrentNode().setKey(SyntaxKey::ExtensionTemplate);
            }
            else writer.CurrentNode().setKey(SyntaxKey::Template);
            flushTemplate(writer, scope, node);
            break;
         case SyntaxKey::ReturnExpression:
            writer.CurrentNode().setKey(SyntaxKey::InlineTemplateExpr);
            flushExpressionTemplate(writer, scope, node);
            break;
         default:
            assert(false);
            break;
      }
      
   }
   else {
      if (node.existChild(SyntaxKey::InitExpression))
         _errorProcessor->raiseTerminalError(errInvalidOperation, retrievePath(node), node);

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

            flushClass(writer, scope, node, node.existChild(SyntaxKey::CodeBlock, SyntaxKey::ReturnExpression));
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
      case SyntaxKey::interpolate:
      {
         QuoteString quote(value, value.length_pos());

         _cacheWriter.newNode(syntaxKey, quote.str());
         break;
      }
      case SyntaxKey::wide:
      {
         QuoteString quote(value, value.length_pos());

         _cacheWriter.newNode(syntaxKey, quote.str());
         break;
      }
      case SyntaxKey::globalreference:
         _cacheWriter.newNode(syntaxKey, value + 1);
         break;
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

void SyntaxTreeBuilder :: renameNode(parse_key_t key)
{
   SyntaxNode current = _cacheWriter.CurrentNode();
   current.setKey(SyntaxTree::fromParseKey(key));
}

void SyntaxTreeBuilder :: mergeRChildren(parse_key_t key)
{
   SyntaxNode current = _cacheWriter.CurrentNode();

   SyntaxNode rchild = current.firstChild().nextNode();
   SyntaxNode nextRChild = rchild.nextNode();
   if (rchild != SyntaxKey::None) {
      rchild.encloseNode(SyntaxTree::fromParseKey(key));

      while (nextRChild != SyntaxKey::None) {
         SyntaxNode c = nextRChild;
         nextRChild = nextRChild.nextNode();

         rchild.mergeNodes(c);
      }
   }   
}

void SyntaxTreeBuilder::mergeLChildren(parse_key_t key)
{
   SyntaxNode current = _cacheWriter.CurrentNode();

   SyntaxNode lchild = current.firstChild();
   SyntaxNode nextLChild = lchild.nextNode();
   lchild.encloseNode(SyntaxTree::fromParseKey(key));

   while (nextLChild.nextNode() != SyntaxKey::None) {
      SyntaxNode c = nextLChild;
      nextLChild = nextLChild.nextNode();

      lchild.mergeNodes(c);
   }
}

void SyntaxTreeBuilder :: encloseLastChild(parse_key_t key)
{
   SyntaxNode current = _cacheWriter.CurrentNode();
   SyntaxNode lastChild = current.lastChild();

   if (lastChild != SyntaxKey::None)
      lastChild.encloseNode(SyntaxTree::fromParseKey(key));
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

void SyntaxTreeBuilder :: loadMetaSection(SyntaxNode node)
{
   SyntaxNode terminalNode = node.firstChild(SyntaxKey::TerminalMask);
   if (terminalNode == SyntaxKey::reference) {
      ReferenceProperName aliasName(terminalNode.identifier());
      NamespaceString ns(terminalNode.identifier());

      CompilerLogic::loadMetaData(_moduleScope, *aliasName, *ns);
   }
}

void SyntaxTreeBuilder :: clearMetaSection(SyntaxNode node)
{
   SyntaxNode terminalNode = node.firstChild(SyntaxKey::TerminalMask);

   CompilerLogic::clearMetaData(_moduleScope, terminalNode.identifier());
}

// --- TemplateProssesor ---

void TemplateProssesor :: copyKVKey(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   SyntaxNode key = node.firstChild();

   copyChildren(writer, scope, key);
}

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
      case SyntaxKey::NameParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.argValues.get(node.arg.reference);

            copyChildren(writer, scope, nodeToInject.firstChild());
         }
         else {
            writer.newNode(node.key, node.arg.reference - 0x100);
            copyChildren(writer, scope, node);
            writer.closeNode();
         }
         break;
      case SyntaxKey::NameArgParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.argValues.get(node.arg.reference);

            //writer.newNode(SyntaxKey::Name);
            copyChildren(writer, scope, nodeToInject);
            //writer.closeNode();
         }
         else {
            writer.newNode(node.key, node.arg.reference - 0x100);
            copyChildren(writer, scope, node);
            writer.closeNode();
         }
         break;
      case SyntaxKey::EnumNameArgParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.parameterValues.get(scope.enumIndex);

            writer.newNode(SyntaxKey::Name);
            copyKVKey(writer, scope, nodeToInject.firstChild());
            writer.closeNode();
         }
         else {
            writer.newNode(node.key, node.arg.reference - 0x100);
            copyChildren(writer, scope, node);
            writer.closeNode();
         }
         break;
      case SyntaxKey::EnumArgParameter:
         if (node.arg.reference < 0x100) {
            SyntaxNode nodeToInject = scope.parameterValues.get(scope.enumIndex);
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
      case SyntaxKey::IncludeStatement:
         writer.newNode(SyntaxKey::IncludeStatement);
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

void TemplateProssesor :: copyClassMembers(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Field:
            copyField(writer, scope, current);
            break;
         case SyntaxKey::Method:
            copyMethod(writer, scope, current);
            break;
         case SyntaxKey::AddAssignOperation:
            copyMethod(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void TemplateProssesor :: generateEnumTemplate(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   scope.enumIndex = 0;
   for (auto it = scope.parameterValues.start(); !it.eof(); ++it) {
      scope.enumIndex++;

      copyClassMembers(writer, scope, node);
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
      case Type::InlineProperty:
      case Type::Class:
      case Type::Textblock:
         copyClassMembers(writer, scope, root);
         break;
      case Type::Enumeration:
         generateEnumTemplate(writer, scope, root);
         break;
      case Type::ExpressionTemplate:
         copyChildren(writer, scope, root.findChild(SyntaxKey::ReturnExpression));
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
   if (type == Type::Class || type == Type::InlineProperty || type == Type::Enumeration) {
      SyntaxNode current = bufferTree.readRoot().firstChild();
      while (current != SyntaxKey::None) {
         if(current == SyntaxKey::Method) {
            targetWriter.newNode(current.key, current.arg.value);
            targetWriter.appendNode(SyntaxKey::Autogenerated);
            SyntaxTree::copyNode(targetWriter, current);
            targetWriter.closeNode();
         }
         else if (current == SyntaxKey::Field) {
            targetWriter.newNode(current.key, current.arg.value);
            targetWriter.appendNode(SyntaxKey::Autogenerated);
            SyntaxTree::copyNode(targetWriter, current);
            targetWriter.closeNode();
         }
         else SyntaxTree::copyNode(targetWriter, current, true);

         current = current.nextNode();
      }
   }
   else SyntaxTree::copyNode(targetWriter, bufferTree.readRoot());
}

void TemplateProssesor :: importTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   importTemplate(Type::Class, templateSection, target, &parameters, nullptr);
}

void TemplateProssesor :: importInlineTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   importTemplate(Type::Inline, templateSection, target, nullptr, &parameters);
}

void TemplateProssesor :: importInlinePropertyTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   importTemplate(Type::InlineProperty, templateSection, target, &parameters, nullptr);
}

void TemplateProssesor :: importCodeTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   importTemplate(Type::CodeTemplate, templateSection, target, &arguments, &parameters);
}

void TemplateProssesor :: importExpressionTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   importTemplate(Type::ExpressionTemplate, templateSection, target, &arguments, &parameters);
}

void TemplateProssesor :: importEnumTemplate(MemoryBase* templateSection,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   importTemplate(Type::Enumeration, templateSection, target, &arguments, &parameters);
}

void TemplateProssesor :: importTextblock(MemoryBase* templateSection, SyntaxNode target)
{
   importTemplate(Type::Textblock, templateSection, target, nullptr, nullptr);
}

void TemplateProssesor :: copyField(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   writer.newNode(node.key);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      //switch (current.key) {
         //case SyntaxKey::TemplateArgParameter:
         //{
         //   SyntaxNode nodeToInject = scope.argValues.get(current.arg.value);
         //   copyNode(writer, scope, nodeToInject);
         //   break;
         //}
         //default:
            copyNode(writer, scope, current);
            //break;
      //}

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
         case SyntaxKey::InlineTemplate:
            copyTemplatePostfix(writer, scope, current);
            break;
         //case SyntaxKey::TemplateArgParameter:
         //{
         //   SyntaxNode nodeToInject = scope.argValues.get(current.arg.value);
         //   copyNode(writer, scope, nodeToInject);
         //   break;
         //}
         default:
            copyNode(writer, scope, current);
            break;
      }

      current = current.nextNode();
   }
   writer.closeNode();
}

void TemplateProssesor :: copyParent(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   writer.newNode(node.key);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplatePostfix:
            copyTemplatePostfix(writer, scope, current);
            break;
         default:
            copyNode(writer, scope, current);
            break;
      }

      current = current.nextNode();
   }
   writer.closeNode();
}

void TemplateProssesor :: copyTemplatePostfix(SyntaxTreeWriter& writer, TemplateScope& scope, SyntaxNode node)
{
   writer.newNode(node.key);
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            writer.newNode(current.key);
            copyChildren(writer, scope, current);
            writer.closeNode();
            break;
         default:
            copyNode(writer, scope, current);
            break;
      }

      current = current.nextNode();
   }
   writer.closeNode();
}

void TemplateProssesor :: copyModuleInfo(SyntaxTreeWriter& writer, SyntaxNode node, TemplateScope& scope)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Import:
         case SyntaxKey::SourcePath:
            copyNode(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void TemplateProssesor :: generateTemplate(SyntaxTreeWriter& writer, TemplateScope& scope, 
   MemoryBase* templateBody, bool importModuleInfo)
{
   SyntaxTree templateTree;
   templateTree.load(templateBody);

   SyntaxNode rootNode = templateTree.readRoot();
   if (importModuleInfo)
      copyModuleInfo(writer, rootNode, scope);

   if (scope.type == Type::Class) {
      ustr_t fullName = scope.moduleScope->resolveFullName(scope.targetRef);

      writer.newNode(SyntaxKey::Class, INVALID_REF);
      writer.appendNode(SyntaxKey::Attribute, V_TEMPLATEBASED);
      writer.appendNode(SyntaxKey::Name, scope.moduleScope->mapFullReference(fullName, true));
   }

   SyntaxNode current = rootNode.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            copyNode(writer, scope, current);
            break;
         case SyntaxKey::Parent:
            copyParent(writer, scope, current);
            break;
         case SyntaxKey::Field:
            copyField(writer, scope, current);
            break;
         case SyntaxKey::Method:
            copyMethod(writer, scope, current);
            break;
         case SyntaxKey::AssignOperation:
         case SyntaxKey::AddAssignOperation:
            copyNode(writer, scope, current);
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
   SyntaxTreeWriter& writer, MemoryBase* sectionBody, List<SyntaxNode>& args)
{
   TemplateScope templateScope(Type::Class, moduleScope, classRef);
   loadArguments(templateScope, &args);

   generateTemplate(writer, templateScope, sectionBody, true);
}
