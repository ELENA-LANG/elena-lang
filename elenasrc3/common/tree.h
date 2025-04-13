//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Tree template classes
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TREE_H
#define TREE_H

#ifdef _MSC_VER

#pragma warning( push )
#pragma warning( disable : 4458 )

#endif

namespace elena_lang
{
   // --- Tree ---
   template <class Key, Key defKey> class Tree
   {
   public:
#pragma pack(push, 1)
      struct NodeArg
      {
         union
         {
            int    value;
            ref_t  reference;
         };

         pos_t     strArgPosition;

         NodeArg()
         {
            reference = 0;
            strArgPosition = INVALID_POS;
         }
         NodeArg(ref_t reference, pos_t strArgPosition)
         {
            this->reference = reference;
            this->strArgPosition = strArgPosition;
         }
      };
      struct NodeRecord
      {
         pos_t       parent;
         pos_t       child;
         pos_t       next;
         Key         key;
         NodeArg     arg;

         NodeRecord()
         {
            key = defKey;
            parent = child = next = 0;
         }
      };
#pragma pack(pop)

   private:
      pos_t newChild(pos_t position, Key key, NodeArg& arg)
      {
         MemoryWriter writer(&_body);
         pos_t child = writer.position();

         NodeRecord r;
         r.parent = position;
         r.child = INVALID_POS;
         r.next = INVALID_POS;
         r.key = key;
         r.arg = arg;

         writer.write(&r, sizeof(r));

         return child;
      }

      pos_t injectChild(pos_t parent, Key key, NodeArg& arg)
      {
         MemoryWriter writer(&_body);
         pos_t child = writer.position();

         NodeRecord nw;
         nw.next = INVALID_POS;
         nw.key = key;
         nw.arg = arg;

         auto r = (NodeRecord*)_body.get(parent);
         if (r->child == INVALID_POS) {
            r->child = child;
            nw.parent = parent;
         }
         else {
            auto ch = (NodeRecord*)_body.get(r->child);
            ch->parent = child;

            nw.parent = parent;
            nw.child = r->child;

            r->child = child;
         }

         writer.write(&nw, sizeof(nw));

         return child;
      }

      pos_t injectSibling(pos_t position, Key key, NodeArg& arg)
      {
         MemoryWriter writer(&_body);
         pos_t child = writer.position();

         NodeRecord nw;
         nw.next = INVALID_POS;
         nw.child = INVALID_POS;
         nw.key = key;
         nw.arg = arg;

         auto r = (NodeRecord*)_body.get(position);
         nw.next = r->next;
         r->next = child;
         nw.parent = r->parent;

         writer.write(&nw, sizeof(nw));

         return child;
      }

      pos_t saveStrArgument(ustr_t strArgument)
      {
         MemoryWriter stringWriter(&_strings);

         pos_t position = stringWriter.position();

         pos_t len = getlength_pos(strArgument);
         if (len > 0) {
            stringWriter.writeString(strArgument, len + 1);
         }
         else stringWriter.writeByte(0);

         return position;
      }

      void appendChild(pos_t parent, pos_t child)
      {
         auto r = (NodeRecord*)_body.get(parent);
         if (r->child == INVALID_POS) {
            r->child = child;
         }
         else {
            auto ch = (NodeRecord*)_body.get(r->child);
            while (ch->next != INVALID_POS) {
               ch = (NodeRecord*)_body.get(ch->next);
            }

            ch->next = child;
         }
      }

      void insertChild(pos_t parent, pos_t child)
      {
         auto r = (NodeRecord*)_body.get(parent);
         if (r->child == INVALID_POS) {
            r->child = child;
         }
         else {
            auto nw = (NodeRecord*)_body.get(child);
            nw->next = r->child;
            r->child = child;
         }
      }

      pos_t readParent(pos_t position)
      {
         if (position == INVALID_POS)
            return INVALID_POS;

         auto r = (NodeRecord*)_body.get(position);

         return r->parent;
      }
      pos_t readChild(pos_t position)
      {
         if (position == INVALID_POS)
            return INVALID_POS;

         auto r = (NodeRecord*)_body.get(position);

         return r->child;
      }
      pos_t readNext(pos_t position)
      {
         if (position == INVALID_POS)
            return INVALID_POS;

         auto r = (NodeRecord*)_body.get(position);

         return r->next;
      }
      pos_t readPrevious(pos_t position)
      {
         pos_t parent = readParent(position);
         if (parent == INVALID_POS)
            return INVALID_POS;

         auto r = (NodeRecord*)_body.get(parent);
         auto child = r->child;
         pos_t prev = INVALID_POS;
         while (child != position && child != INVALID_POS) {
            prev = child;

            child = readNext(child);
         }

         return prev;
      }

      void save(pos_t position, Key key, NodeArg& arg)
      {
         auto r = (NodeRecord*)_body.get(position);
         r->key = key;
         r->arg = arg;
      }

   public:
      class Node
      {
      public:
         friend class Tree;

         Key     key;
         NodeArg arg;

      private:
         Tree*   _tree;
         pos_t   _position;

         Node(Tree* tree, pos_t position, Key key, NodeArg arg)
         {
            this->_tree = tree;
            this->_position = position;

            this->key = key;
            this->arg = arg;
         }

      public:
         bool operator == (Key key) const
         {
            return this->key == key;
         }
         bool operator != (Key key) const
         {
            return this->key != key;
         }
         bool operator == (Node node) const
         {
            return this->_position == node._position;
         }
         bool operator != (Node node) const
         {
            return this->_position != node._position;
         }

         bool compare(Key key1, Key key2)
         {
            return this->key == key1 || this->key == key2;
         }

         ustr_t identifier()
         {
            if (arg.strArgPosition != INVALID_POS) {
               return (const char*)(_tree->_strings.get(arg.strArgPosition));
            }
            else return nullptr;
         }

         static Node read(Tree* tree, pos_t position)
         {
            if (position != INVALID_POS)
            {
               NodeRecord* record = (NodeRecord*)tree->_body.get(position);

               return Node(tree, position, record->key, record->arg);
            }
            else return Node();
         }

         void setKey(Key key)
         {
            this->key = key;

            _tree->save(_position, key, arg);
         }

         void setArgumentReference(ref_t reference)
         {
            this->arg.reference = reference;
            this->arg.strArgPosition = INVALID_POS;

            _tree->save(_position, key, arg);
         }
         void setArgumentValue(int value)
         {
            this->arg.value = value;
            this->arg.strArgPosition = INVALID_POS;

            _tree->save(_position, key, arg);
         }
         void setStrArgument(ustr_t value)
         {
            this->arg.value = 0;
            this->arg.strArgPosition = _tree->saveStrArgument(value);

            _tree->save(_position, key, arg);
         }

         Node parentNode()
         {
            if (_tree != nullptr) {
               return read(_tree, _tree->readParent(_position));
            }
            else return Node();
         }

         Node firstChild() const
         {
            if (_tree != nullptr) {
               return read(_tree, _tree->readChild(_position));
            }
            else return Node();
         }
         Node firstChild(Key mask) const
         {
            Node current = firstChild();
            while (current != defKey && !test((unsigned int)current.key, (unsigned int)mask)) {
               current = current.nextNode();
            }

            return current;
         }

         bool existChild(Key key)
         {
            Node current = firstChild();
            while (current != defKey && current.key != key) {
               current = current.nextNode();
            }

            return current != defKey;
         }

         bool existChild(Key key1, Key key2)
         {
            Node current = firstChild();
            while (current != defKey && current.key != key1 && current.key != key2) {
               current = current.nextNode();
            }

            return current != defKey;
         }

         Node lastChild() const
         {
            Node current = firstChild();
            if (current != defKey) {
               while (current.nextNode() != defKey) {
                  current = current.nextNode();
               }
            }
            return current;
         }
         Node lastChild(Key mask) const
         {
            Node current = firstChild(mask);
            if (current != defKey) {
               Node nextNode = current.nextNode(mask);
               while (nextNode != defKey) {
                  current = nextNode;

                  nextNode = current.nextNode(mask);
               }
            }
            return current;
         }

         Node nextNode()
         {
            if (_tree != nullptr) {
               return read(_tree, _tree->readNext(_position));
            }
            else return Node();
         }
         Node nextNode(Key mask) const
         {
            if (_tree != nullptr) {
               Node current = read(_tree, _tree->readNext(_position));
               while (current != defKey && !test((unsigned int)current.key, (unsigned int)mask)) {
                  current = current.nextNode();
               }

               return current;
            }
            else return Node();
         }

         Node prevNode() const
         {
            if (_tree != nullptr) {
               return read(_tree, _tree->readPrevious(_position));
            }
            else return Node();
         }

         Node findChild(Key key) const
         {
            Node current = firstChild();
            while (current != defKey) {
               if (current == key)
                  return current;

               current = current.nextNode();
            }

            return current;
         }

         Node findChild(Key key1, Key key2) const
         {
            Node current = firstChild();
            while (current != defKey) {
               if (current == key1 || current == key2)
                  return current;

               current = current.nextNode();
            }

            return current;
         }
         Node findChild(Key key1, Key key2, Key key3) const
         {
            Node current = firstChild();
            while (current != defKey) {
               if (current == key1 || current == key2 || current == key3)
                  return current;

               current = current.nextNode();
            }

            return current;
         }
         Node findChild(Key key1, Key key2, Key key3, Key key4) const
         {
            Node current = firstChild();
            while (current != defKey) {
               if (current == key1 || current == key2 || current == key3 || current == key4)
                  return current;

               current = current.nextNode();
            }

            return current;
         }

         Node appendChild(Key key, ustr_t argument)
         {
            pos_t strPos = _tree->saveStrArgument(argument);
            pos_t child = _tree->appendChild(_position, key, 0, strPos);

            return Node::read(_tree, child);
         }
         Node appendChild(Key key, ref_t reference)
         {
            pos_t child = _tree->appendChild(_position, key, reference, INVALID_POS);

            return Node::read(_tree, child);
         }
         Node appendChild(Key key)
         {
            pos_t child = _tree->appendChild(_position, key, 0, INVALID_POS);

            return Node::read(_tree, child);
         }

         Node insertNode(Key key, ref_t argument = 0)
         {
            pos_t child = _tree->insertChild(_position, key, argument, INVALID_POS);

            return Node::read(_tree, child);
         }

         // inject a child node between the current one and its children
         Node injectNode(Key key, int argument = 0)
         {
            NodeRecord* record = (NodeRecord*)_tree->_body.get(_position);
            if (record->child != INVALID_POS) {
               NodeArg arg((ref_t)argument, INVALID_POS);

               pos_t child = _tree->injectChild(_position, key, arg);

               return Node::read(_tree, child);
            }
            else return appendChild(key, argument);
         }

         // enclose the node with a new one
         Node encloseNode(Key key, int argument = 0)
         {
            //NodeRecord* record = (NodeRecord*)_tree->_body.get(_position);

            _tree->injectChild(_position, this->key, this->arg);

            this->key = key;
            this->arg = { (ref_t)argument, INVALID_POS };

            _tree->save(_position, this->key, this->arg);

            return *this;
         }

         Node mergeNodes(Node& child)
         {
            NodeRecord* targetRecord = (NodeRecord*)_tree->_body.get(_position);
            NodeRecord* sourceRecord = (NodeRecord*)_tree->_body.get(child._position);

            // exclude the child from the children
            Node previous = child.prevNode();
            NodeRecord* prevRecord = previous.eol() ? nullptr : (NodeRecord*)_tree->_body.get(previous._position);
            if (!prevRecord) {
               Node childParent = child.parentNode();
               NodeRecord* childParentRecord = (NodeRecord*)_tree->_body.get(childParent._position);

               childParentRecord->child = sourceRecord->next;
            }
            else prevRecord->next = sourceRecord->next;

            // append the source to the current node
            sourceRecord->parent = _position;
            sourceRecord->next = INVALID_POS;

            Node lastChild = this->lastChild();
            if (!lastChild.eol()) {
               NodeRecord* lastRecord = (NodeRecord*)_tree->_body.get(lastChild._position);
               lastRecord->next = child._position;
            }
            else targetRecord->child = child._position;

            return *this;
         }

         bool eol() const
         {
            return _position == INVALID_POS;
         }

         Node()
         {
            key = defKey;
            _tree = nullptr;
            _position = INVALID_POS;
         }
      };

   protected:
      MemoryDump _body;
      MemoryDump _strings;

      pos_t newRoot(Key key, ref_t reference, pos_t strArgPosition)
      {
         NodeArg arg(reference, strArgPosition);

         return newChild(INVALID_POS, key, arg);
      }

      pos_t appendChild(pos_t position, Key key, ref_t reference, pos_t strArgPosition)
      {
         NodeArg arg(reference, strArgPosition);

         pos_t child = newChild(position, key, arg);

         appendChild(position, child);

         return child;
      }
      pos_t insertChild(pos_t position, Key key, ref_t reference, pos_t strArgPosition)
      {
         NodeArg arg(reference, strArgPosition);

         pos_t child = newChild(position, key, arg);

         insertChild(position, child);

         return child;
      }

   public:
      class Writer
      {
         Tree*          _tree;
         pos_t          _current;

         Stack<pos_t>   _bookmarks;

         void updateBookmarks(Stack<pos_t>& bookmarks, pos_t oldPos, pos_t newPos)
         {
            for (auto it = bookmarks.start(); !it.eof(); ++it) {
               if (*it == oldPos)
                  *it = newPos;
            }
         }

         void inject(pos_t position, Key type, ref_t argument, pos_t strArgRef)
         {
            NodeArg arg(argument, strArgRef);

            if (position != INVALID_REF) {
               _current = _tree->injectSibling(position, type, arg);
            }
            else _current = _tree->injectChild(_current, type, arg);

            updateBookmarks(_bookmarks, position, _current);
         }

      public:
         pos_t newBookmark()
         {
            Node lastNode = CurrentNode().lastChild();

            _bookmarks.push(lastNode._position);

            return _bookmarks.count();
         }
         void removeBookmark()
         {
            _bookmarks.pop();
         }

         void newNode(Key key, ref_t arg)
         {
            if (_current == INVALID_POS) {
               _current = _tree->newRoot(key, arg, INVALID_POS);
            }
            else _current = _tree->appendChild(_current, key, arg, INVALID_POS);
         }
         void newNode(Key key, ustr_t argument)
         {
            pos_t strPos = _tree->saveStrArgument(argument);

            if (_current == INVALID_POS) {
               _current = _tree->newRoot(key, 0, strPos);
            }
            else _current = _tree->appendChild(_current, key, 0, strPos);
         }
         void newNode(Key key)
         {
            if (_current == INVALID_POS) {
               _current = _tree->newRoot(key, 0, INVALID_POS);
            }
            else _current = _tree->appendChild(_current, key, 0, INVALID_POS);
         }
         void closeNode()
         {
            _current = _tree->readParent(_current);
         }

         void appendNode(Key key, ref_t arg)
         {
            newNode(key, arg);
            closeNode();
         }
         void appendNode(Key key, ustr_t arg)
         {
            newNode(key, arg);
            closeNode();
         }
         void appendNode(Key key)
         {
            newNode(key, 0);
            closeNode();
         }

         void inject(Key type, ustr_t argument)
         {
            inject(_bookmarks.peek(), type, 0, _tree->saveStrArgument(argument));
         }
         void inject(Key type, ref_t argument)
         {
            inject(_bookmarks.peek(), type, argument, INVALID_POS);
         }
         void inject(Key type)
         {
            inject(_bookmarks.peek(), type, 0, INVALID_POS);
         }

         Node CurrentNode()
         {
            return Node::read(_tree, _current);
         }

         Tree* getOwner()
         {
            return _tree;
         }

         void clear()
         {
            _tree->clear();
            _current = INVALID_POS;
            _bookmarks.clear();
         }

         Writer(Tree& tree)
            : _bookmarks(INVALID_POS)
         {
            this->_tree = &tree;
            this->_current = INVALID_POS;
         }
         Writer(Node& node)
            : _bookmarks(INVALID_POS)
         {
            this->_tree = node._tree;
            this->_current = node._position;
         }
      };


      bool isTreeNode(Node node)
      {
         return this == node._tree;
      }

      Node readRoot()
      {
         return _body.length() != 0 ? Node::read(this, 0u) : Node();
      }

      void clear()
      {
         _body.clear();
         _strings.clear();
      }

      static Node gotoChild(Node node, Key key, ustr_t value)
      {
         Node current = node.findChild(key);
         while (current == key) {
            if (value.compare(current.identifier()))
               return current;

            current = current.nextNode();
         }

         return Node();
      }

      static Node gotoChild(Node node, Key key, int value)
      {
         Node current = node.findChild(key);
         while (current != defKey) {
            if ( current == key && value == current.arg.value)
               return current;

            current = current.nextNode();
         }

         return Node();
      }

      static Node gotoNode(Node current, Key key)
      {
         while (current != defKey) {
            if (current.key == key)
               return current;

            current = current.nextNode();
         }

         return Node();
      }
      static Node gotoNode(Node current, Key key, int value)
      {
         while (current != defKey) {
            if (current.key == key && current.arg.value == value)
               return current;

            current = current.nextNode();
         }

         return Node();
      }

      static bool ifChildExists(Node node, Key key, int value)
      {
         return gotoChild(node, key, value) == key;
      }

      static bool ifChildExists(Node node, Key key, ref_t value)
      {
         return gotoChild(node, key, value) == key;
      }

      static int countChild(Node node, Key key)
      {
         int counter = 0;

         Node current = node.firstChild();
         while (!current.eol()) {
            if (current.key == key)
               counter++;

            current = current.nextNode();
         }

         return counter;
      }

      static int countSibling(Node current, Key key)
      {
         int counter = 0;
         while (current.key == key) {
            counter++;

            current = current.nextNode();
         }

         return counter;
      }

      static void serialize(int level, Node& node, void(*encoder)(int level, TextWriter<char>&, Key, ustr_t, int, void*), TextWriter<char>& writer, void* arg, List<Key>* filters)
      {
         encoder(level, writer, node.key, node.identifier(), node.arg.value, arg);
         Node current = node.firstChild();
         while (current != defKey) {
            if (filters && filters->template retrieveIndex<Key>(current.key, [](Key arg, Key current)
               {
                  return current == arg;
               }) == -1)
            {
               serialize(level + 1, current, encoder, writer, arg, filters);
            }

            current = current.nextNode();
         }

         encoder(level, writer, defKey, nullptr, 0, nullptr);
      }

      static void deserialize(Node root, bool(*reader)(Key&, IdentifierString&, int&, void*), void* arg)
      {
         Key              key = defKey;
         IdentifierString strArg;
         int              intArg;

         Node current = {};
         while (reader(key, strArg, intArg, arg)) {
            if (strArg.length() > 0) {
               current = root.appendChild(key, *strArg);
            }
            else current = root.appendChild(key, intArg);

            deserialize(current, reader, arg);
         }
      }

      Tree() = default;
   };
}

#ifdef _MSC_VER

#pragma warning( pop )

#endif

#endif
