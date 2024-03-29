//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Syntax Tree class implementation
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syntaxtree.h"

using namespace elena_lang;

// --- SyntaxTree ---

void SyntaxTree :: loadTokens(TokenMap& map)
{
   map.add("root", SyntaxKey::Root);
   map.add("namespace", SyntaxKey::Namespace);
   map.add("public_namespace", SyntaxKey::Namespace);
   //   tokens.add("class", lxClass);
   map.add("singleton", SyntaxKey::Class);
   map.add("public_singleton", SyntaxKey::Class);
   map.add("public_symbol", SyntaxKey::Symbol);
   map.add("nested", SyntaxKey::NestedBlock);
   map.add("script_method", SyntaxKey::Method);
   map.add("script_function", SyntaxKey::Method);
   //   tokens.add("method", lxClassMethod);
   map.add("function", SyntaxKey::Method);
   map.add("get_method", SyntaxKey::Method);
   map.add("message", SyntaxKey::Message);
   map.add("code", SyntaxKey::CodeBlock);
   map.add("closure", SyntaxKey::ClosureBlock);
   map.add("object", SyntaxKey::Object);
   map.add("new_variable", SyntaxKey::Object);
   map.add("new_identifier", SyntaxKey::Object);
   map.add("expression", SyntaxKey::Expression);
   map.add("get_expression", SyntaxKey::GetExpression);
   map.add("returning", SyntaxKey::ReturnExpression);
   map.add("message_operation", SyntaxKey::MessageOperation);
   map.add("property_operation", SyntaxKey::PropertyOperation);

   map.add("equal_operation", SyntaxKey::EqualOperation);
   map.add("notequal_operation", SyntaxKey::NotEqualOperation);
   map.add("less_operation", SyntaxKey::LessOperation);
   map.add("greater_operation", SyntaxKey::GreaterOperation);
   map.add("notgreater_operation", SyntaxKey::NotGreaterOperation);
   map.add("notless_operation", SyntaxKey::NotLessOperation);
   map.add("add_operation", SyntaxKey::AddOperation);
   map.add("sub_operation", SyntaxKey::SubOperation);
   map.add("mul_operation", SyntaxKey::MulOperation);
   map.add("div_operation", SyntaxKey::DivOperation);
   map.add("assign_operation", SyntaxKey::AssignOperation);

   map.add("if_operation", SyntaxKey::IfOperation);
   map.add("branch_operation", SyntaxKey::BranchOperation);
   map.add("loop_expression", SyntaxKey::LoopOperation);

   map.add("symbol", SyntaxKey::Symbol);
   //   tokens.add("preloaded_symbol", lxSymbol);
   map.add("literal", SyntaxKey::string);
   map.add("identifier", SyntaxKey::identifier);
   map.add("super_identifier", SyntaxKey::identifier);
   map.add("character", SyntaxKey::character);
   //   tokens.add("variable_identifier", lxIdentifier);
   //   tokens.add("new_identifier", lxIdentifier);
   map.add("integer", SyntaxKey::integer);
   map.add("parameter", SyntaxKey::Parameter);
   ////   tokens.add("include", lxInclude);
   //   //tokens.add("forward", lxForward);
   map.add("reference", SyntaxKey::reference);
   //   tokens.add("new_reference", lxReference);
   map.add("nameattr", SyntaxKey::Name);
   map.add("property_parameter", SyntaxKey::PropertyOperation); // !!temporal - should be removed
   map.add("import", SyntaxKey::Import);   
}

bool SyntaxTree :: save(MemoryBase* section)
{
   MemoryWriter writer(section);

   writer.writePos(_body.length());
   writer.write(_body.get(0), _body.length());

   writer.writePos(_strings.length());
   writer.write(_strings.get(0), _strings.length());

   return _body.length() > 0;
}

void SyntaxTree :: load(MemoryBase* section)
{
   _body.clear();
   _strings.clear();

   MemoryReader reader(section);
   pos_t bodyLength = reader.getPos();
   _body.load(reader, bodyLength);

   pos_t stringLength = reader.getPos();
   _strings.load(reader, stringLength);
}

void SyntaxTree :: copyNode(SyntaxTreeWriter& writer, SyntaxNode node, bool includingNode)
{
   if (includingNode) {
      if (node.arg.strArgPosition != INVALID_POS) {
         writer.newNode(node.key, node.identifier());
      }
      else writer.newNode(node.key, node.arg.reference);
   }

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      copyNode(writer, current, true);

      current = current.nextNode();
   }

   if (includingNode)
      writer.closeNode();
}

void SyntaxTree :: copyNodeSafe(SyntaxTreeWriter& writer, SyntaxNode node, bool includingNode)
{
   if (includingNode) {
      if (node.arg.strArgPosition != INVALID_POS) {
         IdentifierString tmp(node.identifier());
         writer.newNode(node.key, *tmp);
      }
      else writer.newNode(node.key, node.arg.reference);
   }

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      copyNodeSafe(writer, current, true);

      current = current.nextNode();
   }

   if (includingNode)
      writer.closeNode();
}

void SyntaxTree :: saveNode(SyntaxNode node, MemoryBase* section, bool includingNode)
{
   SyntaxTree tree;
   SyntaxTreeWriter writer(tree);

   writer.newNode(SyntaxKey::Root);

   copyNode(writer, node, includingNode);

   writer.closeNode();

   tree.save(section);
}
