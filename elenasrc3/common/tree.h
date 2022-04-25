//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Tree template classes
//
//                                                 (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TREE_H
#define TREE_H

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

      pos_t saveStrArgument(ustr_t strArgument)
      {
         MemoryWriter stringWriter(&_strings);

         pos_t position = stringWriter.position();

         stringWriter.writeString(strArgument, getlength_pos(strArgument) + 1);

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

         // inject a child node between the current one and its children
         Node injectNode(Key key, int argument = 0)
         {
            NodeRecord* record = (NodeRecord*)_tree->_body.get(_position);
            if (record->child != INVALID_POS) {
               NodeArg arg(argument, INVALID_POS);

               pos_t child = _tree->injectChild(_position, key, arg);

               return Node::read(_tree, child);
            }
            else return appendChild(key, argument);
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

   public:
      class Writer
      {
         Tree* _tree;
         pos_t _current;

      public:
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

         Node CurrentNode()
         {
            return Node::read(_tree, _current);
         }

         void clear()
         {
            _tree->clear();
            _current = INVALID_POS;
         }

         Writer(Tree& tree)
         {
            this->_tree = &tree;
            this->_current = INVALID_POS;
         }
         Writer(Node& node)
         {
            this->_tree = node._tree;
            this->_current = node._position;
         }
      };

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
         while (current == key) {
            if (value == current.arg.value)
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

      static bool ifChildExists(Node node, Key key, int value)
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

      Tree() = default;
   };
}

#endif
