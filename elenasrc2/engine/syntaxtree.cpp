//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree class implementation
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syntaxtree.h"
#include <stdarg.h>

using namespace _ELENA_;

// --- loadSyntaxTokens ---

void _ELENA_::loadSyntaxTokens(Map<ident_t, int>& tokens, bool fullMode)
{
//   tokens.add("root", lxRoot);
//   tokens.add("namespace", lxNamespace);
//   tokens.add("public_namespace", lxNamespace);
//   tokens.add("class", lxClass);
//   tokens.add("singleton", lxClass);
//   //tokens.add("nested", lxNestedClass);
//   tokens.add("script_method", lxClassMethod);
//   tokens.add("script_function", lxClassMethod);
//   tokens.add("method", lxClassMethod);
//   tokens.add("function", lxClassMethod);
//   tokens.add("get_method", lxClassMethod);
//   tokens.add("message", lxMessage);
//   tokens.add("code", lxCode);
//   tokens.add("expression", lxExpression);
//   tokens.add("returning", lxReturning);
//   tokens.add("symbol", lxSymbol);
//   tokens.add("preloaded_symbol", lxSymbol);
//   tokens.add("literal", lxLiteral);
//   tokens.add("identifier", lxIdentifier);
//   tokens.add("character", lxCharacter);
//   tokens.add("variable_identifier", lxIdentifier);
//   tokens.add("new_identifier", lxIdentifier);
//   tokens.add("prev_identifier", lxIdentifier);
//   tokens.add("integer", lxInteger);
//   tokens.add("parameter", lxMethodParameter);
////   tokens.add("include", lxInclude);
//   //tokens.add("forward", lxForward);
//   tokens.add("reference", lxReference);
//   tokens.add("new_reference", lxReference);
//   tokens.add("variable", lxVariable);
//   //tokens.add("assign", lxAssign);
//   //tokens.add("operator", lxOperator);
//   tokens.add("nameattr", lxNameAttr);
//   //tokens.add("property_parameter", lxPropertyParam);
//   //tokens.add("import", lxImport);
//   tokens.add("loop_expression", lxExpression);
//
   if (fullMode) {
////      tokens.add("argarrop", lxArgArrOp);
      tokens.add("assigning", lxAssigning);
      tokens.add("copying", lxCopying);
//      //      tokens.add("boxing", lxBoxing);
//      tokens.add("call", lxCalling_0);
////      tokens.add("condboxing", lxCondBoxing);
      tokens.add("directcall", lxDirectCalling);
      tokens.add("embeddable", lxEmbeddableAttr);
////      tokens.add("fieldaddress", lxFieldAddress);
////      tokens.add("localaddress", lxLocalAddress);
      tokens.add("templocal", lxTempLocal);
      tokens.add("sdirectcall", lxSDirectCalling);
////      tokens.add("unboxing", lxUnboxing);
//      tokens.add("newframe", lxNewFrame);
//      //tokens.add("field_expr", lxFieldExpression);
//      //tokens.add("self_local", lxSelfLocal);
//      //tokens.add("generic_resend", lxGenericResending);
//
      tokens.add("constint", lxConstantInt);
//      //tokens.add("constreal", lxConstantReal);
////      tokens.add("stacksafe", lxStacksafeAttr);
////      tokens.add("boxable", lxBoxableAttr);
//      //tokens.add("intop", lxIntOp);
      tokens.add("intboolop", lxIntBoolOp);
//      //tokens.add("longop", lxLongOp);
//      //tokens.add("realop", lxRealOp);
//      //tokens.add("realintop", lxRealIntOp);
//      //      tokens.add("intarrop", lxIntArrOp);
////      tokens.add("bytearrop", lxByteArrOp);
////      tokens.add("shortarrop", lxShortArrOp);
////      tokens.add("temp", lxTempAttr);
////      tokens.add("stdextcall", lxStdExternalCall);
////      tokens.add("extcall", lxExternalCall);
////      tokens.add("corecall", lxCoreAPICall);
////      tokens.add("extarg", lxExtArgument);
////      tokens.add("intextarg", lxIntExtArgument);
////      tokens.add("interncall", lxInternalCall);
      tokens.add("branching", lxBranching);
//      //tokens.add("looping", lxLooping);
////      tokens.add("constlong", lxConstantLong);
////      tokens.add("constsymbol", lxConstantSymbol);
////      tokens.add("constmssg", lxMessageConstant);
////      tokens.add("constsubj", lxSubjectConstant);
////      tokens.add("argunboxing", lxArgUnboxing);
////      tokens.add("argboxing", lxArgBoxing);
////      tokens.add("byreftarget", lxByRefTarget);
////      tokens.add("arrop", lxArrOp);
////      tokens.add("binarrop", lxBinArrOp);
////      tokens.add("duplicateboxing", lxDuplicateBoxingAttr);
////      tokens.add("newarrop", lxNewArrOp);
////      tokens.add("nested_expr", lxNested);
      tokens.add("constattr", lxConstAttr);
//      tokens.add("seqexpression", lxSeqExpression);
//      //tokens.add("ret_embeddable", lxRetEmbeddableAttr);
//      //tokens.add("dispatch_mode", lxDispatchMode);
   }
}

// --- SyntaxWriter ---

//inline pos_t getBookmark(int bookmark, Stack<pos_t>& bookmarks)
//{
//   return (bookmark == 0) ? bookmarks.peek() : *bookmarks.get(bookmarks.Count() - bookmark);
//}

inline void updateBookmarks(Stack<pos_t>& bookmarks, pos_t oldPos, pos_t newPos)
{
   for (auto it = bookmarks.start(); !it.Eof(); it++) {
      if (*it == oldPos)
         * it = newPos;
   }
}

void SyntaxWriter :: inject(pos_t position, LexicalType type, ref_t argument, pos_t strArgRef)
{
   pos_t prev = _syntaxTree->getPrevious(position);
   if (prev != INVALID_REF) {
      _current = _syntaxTree->injectSibling(prev, type, argument, strArgRef);
   }
   else _current = _syntaxTree->injectChild(position, type, argument, strArgRef);

   updateBookmarks(_bookmarks, position, _current);
}

void SyntaxWriter :: replace(pos_t position, LexicalType type, ref_t argument, pos_t strArgRef)
{
   _current = position;

   pos_t parent = _syntaxTree->getParent(position);
   auto node = _syntaxTree->read(parent);

   node.type = type;
   node.argument = argument;
   node.strArgument = strArgRef;

   _syntaxTree->save(node);
}

//void SyntaxWriter :: insert(LexicalType type, ref_t argument, pos_t strArgRef, bool newMode)
//{
//   pos_t pos = _syntaxTree->insertChild(_current, type, argument, strArgRef);
//   if (newMode)
//      _current = pos;
//}
//
//void SyntaxWriter :: trim()
//{
//   if (_pendingBookmarks == 0) {
//      pos_t position = _bookmarks.peek();
//      pos_t prev = _syntaxTree->getPrevious(position);
//      if (prev != INVALID_REF) {
//         _syntaxTree->clearSibling(prev);
//      }
//      else _syntaxTree->clearChildren(position);
//
//      while (_bookmarks.peek() == position) {
//         _bookmarks.pop();
//         _pendingBookmarks++;
//      }
//   }
//}

void SyntaxWriter :: newNode(LexicalType type, ref_t argument)
{
   if (_current == INVALID_REF) {
      _current = _syntaxTree->newRoot(type, argument, INVALID_REF);
   }
   else _current = _syntaxTree->appendChild(_current, type, argument, INVALID_REF);

   insertPendingBookmarks(_current);
}

void SyntaxWriter :: newNode(LexicalType type, ident_t argument)
{
   if (_current == INVALID_REF) {
      _current = _syntaxTree->newRoot(type, 0, _syntaxTree->saveStrArgument(argument));
   }
   else _current = _syntaxTree->appendChild(_current, type, 0, _syntaxTree->saveStrArgument(argument));

   insertPendingBookmarks(_current);
}

void SyntaxWriter :: closeNode()
{
   _current = _syntaxTree->getParent(_current);
}

//bool SyntaxWriter :: seekUp(LexicalType type)
//{
//   pos_t node = _current;
//   while (node != INVALID_REF && _syntaxTree->getType(node) != type) {
//      node = _syntaxTree->getParent(node);
//   }
//
//   if (node != INVALID_REF) {
//      _current = node;
//
//      return true;
//   }
//   else return false;
//}
//
//inline bool checkTypes(LexicalType v, LexicalType r1, LexicalType r2)
//{
//   return v == r1 || v == r2;
//}
//
//bool SyntaxWriter :: seekUp(LexicalType type1, LexicalType type2)
//{
//   pos_t node = _current;
//   while (node != INVALID_REF && !checkTypes(_syntaxTree->getType(node), type1, type2)) {
//      node = _syntaxTree->getParent(node);
//   }
//
//   if (node != INVALID_REF) {
//      _current = node;
//
//      return true;
//   }
//   else return false;
//}

void SyntaxWriter :: moveToPrevious()
{
   pos_t lastChild = _syntaxTree->getChild(_current);

   pos_t next = _syntaxTree->getNext(lastChild);
   while (next != INVALID_REF) {
      lastChild = next;
      next = _syntaxTree->getNext(lastChild);
   }

   _current = lastChild;
}

bool SyntaxWriter :: moveToChild()
{
   pos_t child = _syntaxTree->getChild(_current);
   if (child != INVALID_REF) {
      _current = child;

      return true;
   }
   else return false;
}

SyntaxTree::Node SyntaxWriter :: CurrentNode()
{
   return _syntaxTree->read(_current);
}

SyntaxTree::Node SyntaxWriter :: BookmarkNode()
{
   return _syntaxTree->read(_bookmarks.peek());
}

// --- SyntaxTree::Node ---

SyntaxTree::Node :: Node(SyntaxTree* tree, pos_t position, LexicalType type, ref_t argument, int strArgument)
{
   this->tree = tree;
   this->position = position;

   this->type = type;
   this->argument = argument;
   this->strArgument = strArgument;
}

// --- SyntaxTree ---

struct _NodeRecord
{
   pos_t       parent;
   pos_t       child;
   pos_t       next;
   LexicalType type;
   int         argument;
   pos_t       strArgument;
};

inline pos_t newChild(_Memory& body, pos_t position, LexicalType type, int argument, pos_t strArgument)
{
   MemoryWriter writer(&body);
   pos_t child = writer.Position();

   writer.writeDWord(position);
   writer.writeDWord(INVALID_REF);
   writer.writeDWord(INVALID_REF);
   writer.writeDWord(type);
   writer.writeDWord(argument);
   writer.writeDWord(strArgument);

   return child;
}

inline void appendChild(_Memory& body, pos_t parent, pos_t child)
{
   auto r = (_NodeRecord*)body.get(parent);
   if (r->child == INVALID_REF) {
      r->child = child;
   }
   else {
      auto ch = (_NodeRecord*)body.get(r->child);
      while (ch->next != INVALID_REF) {
         ch = (_NodeRecord*)body.get(ch->next);
      }

      ch->next = child;
   }
}

inline void insertChild(_Memory& body, pos_t parent, pos_t child)
{
   auto r = (_NodeRecord*)body.get(parent);
   if (r->child == INVALID_REF) {
      r->child = child;
   }
   else {
      auto nw = (_NodeRecord*)body.get(child);
      nw->next = r->child;
      r->child = child;
   }
}

inline void insertSibling(_Memory& body, pos_t prev, pos_t node)
{
   auto p = (_NodeRecord*)body.get(prev);
   auto nw = (_NodeRecord*)body.get(node);

   nw->next = p->next;
   p->next = node;
}

inline void updateParents(_Memory& body, pos_t node)
{
   auto r = (_NodeRecord*)body.get(node);
   pos_t child = r->child;
   while (child != INVALID_REF) {
      auto c = (_NodeRecord*)body.get(child);
      c->parent = node;

      child = c->next;
   }
}

inline void injectChild(_Memory& body, pos_t parent, pos_t child)
{
   auto r = (_NodeRecord*)body.get(parent);
   if (r->child == INVALID_REF) {
      r->child = child;
   }
   else {
      auto nw = (_NodeRecord*)body.get(child);
      nw->child = r->child;
      r->child = child;

      // HOTFIX : modify the parents of injected nodes
      updateParents(body, child);
   }
}

inline void injectSibling(_Memory& body, pos_t node, pos_t child)
{
   auto r = (_NodeRecord*)body.get(node);
   auto nw = (_NodeRecord*)body.get(child);
   nw->child = r->next;
   r->next = child;

   // HOTFIX : modify the parents of injected nodes
   updateParents(body, child);
}

//inline void clearChildren(_Memory& body, pos_t parent)
//{
//   auto r = (_NodeRecord*)body.get(parent);
//   r->child = INVALID_REF;
//}
//
//inline void clearSibling(_Memory& body, pos_t node)
//{
//   auto r = (_NodeRecord*)body.get(node);
//   r->next = INVALID_REF;
//}

inline pos_t readParent(_Memory& body, pos_t position)
{
   auto r = (_NodeRecord*)body.get(position);

   return r->parent;
}

inline pos_t readChild(_Memory& body, pos_t position)
{
   auto r = (_NodeRecord*)body.get(position);

   return r->child;
}

inline pos_t readNext(_Memory& body, pos_t position)
{
   auto r = (_NodeRecord*)body.get(position);

   return r->next;
}

inline bool read(_Memory& body, pos_t position, LexicalType& type, ref_t& arg, pos_t& strArgRef)
{
   auto r = (_NodeRecord*)body.get(position);
   if (r) {
      type = r->type;
      arg = r->argument;
      strArgRef = r->strArgument;

      return true;
   }
   else return false;
}

inline void save(_Memory& body, pos_t position, LexicalType type, ref_t arg, pos_t strArgRef)
{
   auto r = (_NodeRecord*)body.get(position);
   if (r) {
      r->type = type;
      r->argument = arg;
      r->strArgument = strArgRef;
   }
}

pos_t SyntaxTree :: newRoot(LexicalType type, ref_t argument, pos_t strArgumentRef)
{
   return ::newChild(_body, INVALID_REF, type, argument, strArgumentRef);
}

pos_t SyntaxTree :: appendChild(pos_t position, LexicalType type, ref_t argument, pos_t strArgumentRef)
{
   pos_t child = ::newChild(_body, position, type, argument, strArgumentRef);

   ::appendChild(_body, position, child);

   return child;
}

pos_t SyntaxTree :: insertChild(pos_t position, LexicalType type, ref_t argument, pos_t strArgumentRef)
{
   pos_t child = ::newChild(_body, position, type, argument, strArgumentRef);

   ::insertChild(_body, position, child);

   return child;
}

pos_t SyntaxTree :: insertSibling(pos_t position, LexicalType type, ref_t argument, pos_t strArgumentRef)
{
   pos_t parent = ::readParent(_body, position);
   pos_t child = ::newChild(_body, parent, type, argument, strArgumentRef);

   pos_t prev = getPrevious(position);
   if (prev != INVALID_REF) {
      ::insertSibling(_body, prev, child);
   }
   else {
      if (parent != INVALID_REF)
         ::insertChild(_body, parent, child);
   }

   return child;
}

pos_t SyntaxTree :: injectChild(pos_t position, LexicalType type, ref_t argument, pos_t strArgumentRef)
{
   pos_t parentNode = getParent(position);
   pos_t child = ::newChild(_body, parentNode, type, argument, strArgumentRef);

   ::injectChild(_body, parentNode, child);

   return child;
}

pos_t SyntaxTree :: injectSibling(pos_t position, LexicalType type, ref_t argument, pos_t strArgumentRef)
{
   pos_t parentNode = getParent(position);
   pos_t child = ::newChild(_body, parentNode, type, argument, strArgumentRef);

   ::injectSibling(_body, position, child);

   return child;
}

//void SyntaxTree :: clearChildren(pos_t position)
//{
//   ::clearChildren(_body, position);
//}
//
//void SyntaxTree :: clearSibling(pos_t position)
//{
//   ::clearSibling(_body, position);
//}

pos_t SyntaxTree :: getParent(pos_t position)
{
   return position == INVALID_REF ? INVALID_REF : ::readParent(_body, position);
}

pos_t SyntaxTree :: getChild(pos_t position)
{
   return position == INVALID_REF ? INVALID_REF : ::readChild(_body, position);
}

pos_t SyntaxTree :: getNext(pos_t position)
{
   return position == INVALID_REF ? INVALID_REF : ::readNext(_body, position);
}

pos_t SyntaxTree :: getPrevious(pos_t position)
{
   pos_t parent = ::readParent(_body, position);
   if (parent != INVALID_REF) {
      pos_t child = ::readChild(_body, parent);
      pos_t prev = INVALID_REF;
      while (child != position && child != INVALID_REF) {
         prev = child;

         child = ::readNext(_body, child);
      }

      return prev;
   }
   else return INVALID_REF;
}

pos_t SyntaxTree :: saveStrArgument(ident_t strArgument)
{
   MemoryWriter stringWriter(&_strings);

   pos_t position = stringWriter.Position();

   stringWriter.writeLiteral(strArgument, getlength(strArgument) + 1);

   return position;
}

SyntaxTree::Node SyntaxTree :: read(pos_t position)
{
   LexicalType type;
   ref_t arg;
   pos_t strArg;
   if (position != INVALID_REF && ::read(_body, position, type, arg, strArg)) {
      return Node(this, position, type, arg, strArg);
   }
   else return Node();
}

//LexicalType SyntaxTree :: getType(pos_t position)
//{
//   LexicalType type;
//   ref_t arg;
//   pos_t strArg;
//   if (position != INVALID_REF && ::read(_body, position, type, arg, strArg)) {
//      return type;
//   }
//   else return lxNone;
//}

void SyntaxTree :: refresh(SyntaxTree::Node& node)
{
   LexicalType type;
   ref_t arg;
   pos_t strArg;
   if (::read(_body, node.position, type, arg, strArg)) {
      node.type = type;
      node.argument = arg;
      node.strArgument = strArg;
   }
}

void SyntaxTree :: save(Node& node)
{
   ::save(_body, node.position, node.type, node.argument, node.strArgument);
}

SyntaxTree::Node SyntaxTree :: readRoot()
{
   return read(0u);
}

void SyntaxTree :: saveNode(Node node, _Memory* dump, bool inclusingNode)
{
   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxRoot);

   if (inclusingNode) {
      if (node.strArgument != INVALID_REF) {
         writer.newNode(node.type, node.identifier());
      }
      else writer.newNode(node.type, node.argument);
      copyNode(writer, node);
      writer.closeNode();
   }
   else copyNode(writer, node);

   writer.closeNode();

   tree.save(dump);
}

////void SyntaxTree :: loadNode(Node node, _Memory* dump)
////{
////   SyntaxTree tree(dump);
////
////   copyNode(tree.readRoot(), node);
////}
////
////void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer)
////{
////   SNode current = buffer.readRoot();
////   while (current != lxNone) {
////      if (current != lxIdle) {
////         if (current.strArgument != INVALID_REF) {
////            writer.newNode(current.type, current.identifier());
////         }
////         else writer.newNode(current.type, current.argument);
////
////         SyntaxTree::copyNode(writer, current);
////         writer.closeNode();
////
////         current = lxIdle;
////      }
////      current = current.nextNode();
////   }
////}

void SyntaxTree :: copyNode(Writer& writer, LexicalType type, Node owner)
{
   SyntaxTree::Node node = owner.findChild(type);
   if (node != lxNone) {
      writer.appendNode(type, node.argument);
   }
}

void SyntaxTree :: copyNode(SyntaxTree::Writer& writer, SyntaxTree::Node node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.strArgument != INVALID_REF) {
         writer.newNode(current.type, current.identifier());
      }
      else writer.newNode(current.type, current.argument);

      copyNode(writer, current);

      writer.closeNode();

      current = current.nextNode();
   }
}

SyntaxTree::Node SyntaxTree::insertNodeCopy(SyntaxTree::Node source, SyntaxTree::Node destination)
{
   Node insertedNode;
   if (source.strArgument != INVALID_REF) {
      if (source.tree == destination.tree) {
         // HOTFIX : literal argument could be corrupted by reallocating the string buffer,
         // so the special routine should be used
         insertedNode = destination.insertStrNode(source.type, source.strArgument);
      }
      else insertedNode = destination.insertNode(source.type, source.identifier());
   }
   else insertedNode = destination.insertNode(source.type, source.argument);

   copyNode(source, insertedNode);

   return insertedNode;
}

void SyntaxTree :: moveSiblingNodes(SyntaxTree::Node source, SyntaxTree::Node destination)
{
   SNode insertedNode;
   while (source != lxNone) {
      if (source != lxIdle) {
         if (source.strArgument != INVALID_REF) {
            if (source.tree == destination.tree) {
               // HOTFIX : literal argument could be corrupted by reallocating the string buffer,
               // so the special routine should be used
               insertedNode = destination.appendStrNode(source.type, source.strArgument);
            }
            else insertedNode = destination.appendNode(source.type, source.identifier());
         }
         else insertedNode = destination.appendNode(source.type, source.argument);

         copyNode(source, insertedNode);

         source = lxIdle;
      }

      source = source.nextNode();
   }
}

void SyntaxTree :: copyNode(SyntaxTree::Node source, SyntaxTree::Node destination)
{
   SNode current = source.firstChild();
   while (current != lxNone) {
      if (current.strArgument != INVALID_REF) {
         if (source.tree == destination.tree) {
            // HOTFIX : literal argument could be corrupted by reallocating the string buffer,
            // so the special routine should be used
            copyNode(current, destination.appendStrNode(current.type, current.strArgument));
         }
         else copyNode(current, destination.appendNode(current.type, current.identifier()));
      }
      else copyNode(current, destination.appendNode(current.type, current.argument));

      current = current.nextNode();
   }
}

//SyntaxTree::Node SyntaxTree :: findPattern(Node node, int counter, ...)
//{
//   va_list argptr;
//   va_start(argptr, counter);
//
//   size_t level = 1;
//   Node nodes[0x10];
//   nodes[0] = node;
//
//   for (int i = 0; i < counter; i++) {
//      // get the next pattern
//      NodePattern pattern = va_arg(argptr, NodePattern);
//
//      size_t newLevel = level;
//      for (size_t j = 0; j < level; j++) {
//         Node member = nodes[j].firstChild();
//
//         if (member != lxNone) {
//            // find the matched member
//            while (member != lxNone) {
//               if (pattern.match(member)) {
//                  nodes[newLevel] = member;
//                  newLevel++;
//               }
//
//               member = member.nextNode();
//            }
//         }
//      }
//
//      size_t oldLevel = level;
//      level = 0;
//      for (size_t j = oldLevel; j < newLevel; j++) {
//         nodes[level] = nodes[j];
//         level++;
//      }
//
//      if (level == 0) {
//         nodes[0] = Node();
//
//         break;
//      }
//
//   }
//
//   return nodes[0];
//}

SyntaxTree::Node SyntaxTree :: findTerminalInfo(SyntaxTree::Node node)
{
   if (node.existChild(lxRow))
      return node;

   SNode current = node.firstChild();
   while (current != lxNone) {
      SNode terminalNode = findTerminalInfo(current);
      if (terminalNode != lxNone)
         return terminalNode;

      current = current.nextNode();
   }

   return current;
}
