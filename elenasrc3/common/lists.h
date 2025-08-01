//---------------------------------------------------------------------------
//              E L E N A   P r o j e c t:  ELENA Common Library
//
//              This header contains various ELENA Engine list templates
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LISTS_H
#define LISTS_H
#include <assert.h>

DISABLE_WARNING_PUSH
DISABLE_WARNING_NULLCONVERSION
DISABLE_WARNING_ADDRESS
DISABLE_WARNING_UNINITIALIZED_FIELD

namespace elena_lang
{
   template <class T, void(*FreeT)(T) = nullptr> class BListBase;
   template <class T, void(*FreeT)(T) = nullptr> class ListBase;

   // --- ItemBase ---
   template <class T> struct ItemBase
   {
      T         item;
      ItemBase* next;

      ItemBase(T item, ItemBase* next)
      {
         this->item = item;
         this->next = next;
      }
      ~ItemBase()
      {
      }

   };

   // --- BItemBase ---
   template <class T> struct BItemBase
   {
      T          item;
      BItemBase* previous;
      BItemBase* next;

      BItemBase(T item, BItemBase* previous, BItemBase* next)
      {
         this->item = item;
         this->previous = previous;
         this->next = next;
      }
      ~BItemBase()
      {
      }

   };

   // --- MapItemBase ---
   template <class Key, class T, Key(*AllocKey)(Key), void(*FreeKey)(Key)> struct MapItemBase
   {
      Key          key;
      T            item;
      MapItemBase* next;

      MapItemBase(Key key, T item, MapItemBase* next)
      {
         this->key = AllocKey != nullptr ? AllocKey(key) : key;
         this->item = item;
         this->next = next;
      }
      ~MapItemBase()
      {
         if (FreeKey) {
            FreeKey(this->key);
         }
      }
   };

   // --- MapItemBase ---

#pragma pack(push, 1)
   template <class Key, class T, Key(*GetKey)(MemoryDump*, pos_t)> struct MemoryMapItemBase
   {
      pos_t nextOffset; // offset from the memory dump begining
      pos_t keyOffset;
      T     item;

      Key readKey(MemoryDump* buffer) const
      {
         return GetKey(buffer, keyOffset);
      }

      MemoryMapItemBase(pos_t keyOffset, T item, pos_t nextOffset)
      {
         this->nextOffset = nextOffset;
         this->keyOffset = keyOffset;
         this->item = item;
      }
   };

   template <class T1, class T2, T1 def1 = 0, T2 def2 = 0> struct Pair
   {
      T1 value1;
      T2 value2;

      bool operator ==(Pair pair) const
      {
         return (this->value1 == pair.value1 && this->value2 == pair.value2);
      }

      Pair()
      {
         this->value1 = def1;
         this->value2 = def2;
      }
      Pair(T1 value1, T2 value2)
      {
         this->value1 = value1;
         this->value2 = value2;
      }
   };
#pragma pack(pop)

   // --- IteratorBase ---
   template <class T, class Item, void(*FreeT)(T)> class ListIteratorBase
   {
      Item* _current;

      friend class BListBase<T, FreeT>;
      friend class ListBase<T, FreeT>;

   public:
      bool operator ==(const ListIteratorBase& it)
      {
         return _current == it._current;
      }
      bool operator !=(const ListIteratorBase& it)
      {
         return _current != it._current;
      }

      ListIteratorBase& operator =(const ListIteratorBase& it)
      {
         this->_current = it._current;

         return *this;
      }

      ListIteratorBase& operator ++()
      {
         _current = _current->next;

         return *this;
      }
      ListIteratorBase operator ++(int)
      {
         ListIteratorBase tmp = *this;
         ++* this;

         return tmp;
      }
      ListIteratorBase& operator--()
      {
         _current = _current->previous;

         return *this;
      }
      ListIteratorBase operator--(int)
      {
         ListIteratorBase tmp = *this;
         --* this;

         return tmp;
      }

      T& operator*() const { return _current->item; }

      bool eof() const { return _current == nullptr; }

      bool last() const { return (_current->next == nullptr); }

      bool first() const { return (_current->previous == nullptr); }

      ListIteratorBase()
      {
         _current = nullptr;
      }
      ListIteratorBase(Item* item)
      {
         _current = item;
      }
   };

   // --- IteratorBase ---
   template <class Key, class T, class Item> class IteratorBase
   {
      Item* _current;

   public:
      bool operator ==(const IteratorBase& it)
      {
         return _current == it._current;
      }
      bool operator !=(const IteratorBase& it)
      {
         return _current != it._current;
      }

      IteratorBase& operator =(const IteratorBase& it)
      {
         this->_current = it._current;

         return *this;
      }

      IteratorBase& operator ++()
      {
         _current = _current->next;

         return *this;
      }
      IteratorBase operator ++(int)
      {
         IteratorBase tmp = *this;
         ++* this;

         return tmp;
      }

      T& operator*() const { return _current->item; }

      Key key() const { return _current->key; }

      bool eof() const { return _current == nullptr; }

      Item* _item() const { return _current; }

      IteratorBase()
      {
         _current = nullptr;
      }
      IteratorBase(Item* item)
      {
         _current = item;
      }
   };

   // --- IteratorBase ---
   template <class Key, class T, class Item> class MemoryIteratorBase
   {
      Item*       _current;
      MemoryDump* _buffer;

   public:
      bool operator ==(const MemoryIteratorBase& it)
      {
         return _current == it._current;
      }
      bool operator !=(const MemoryIteratorBase& it)
      {
         return _current != it._current;
      }

      MemoryIteratorBase& operator =(const MemoryIteratorBase& it)
      {
         this->_current = it._current;
         this->_buffer = it._buffer;

         return *this;
      }

      MemoryIteratorBase& operator ++()
      {
         if (_current->nextOffset) {
            _current = (Item*)_buffer->get(_current->nextOffset);
         }
         else _current = nullptr;

         return *this;
      }
      MemoryIteratorBase operator ++(int)
      {
         MemoryIteratorBase tmp = *this;
         ++* this;

         return tmp;
      }

      T& operator*() const { return _current->item; }

      Key key() const { return _current->readKey(_buffer); }

      bool eof() const { return _current == nullptr; }

      bool last()
      {
         MemoryIteratorBase tmp = *this;
         ++tmp;

         return tmp.eof();
      }

      MemoryIteratorBase(MemoryDump* buffer, Item* item)
      {
         _buffer = buffer;
         _current = item;
      }
      MemoryIteratorBase()
      {
         _current = nullptr;
         _buffer = nullptr;
      }
   };

   // --- MemoryListIterator ---
   template <class T> class MemoryListIterator
   {
      MemoryDump* _buffer;
      pos_t       _position, _length;
      T           _current;

   public:
      bool operator ==(const MemoryListIterator& it)
      {
         return _position == it._position;
      }
      bool operator !=(const MemoryListIterator& it)
      {
         return _position != it._position;
      }

      MemoryListIterator& operator =(const MemoryListIterator& it)
      {
         this->_position = it._position;
         this->_length = it._length;
         this->_buffer = it._buffer;

         return *this;
      }

      MemoryListIterator& operator ++()
      {
         if (_position < _length) {
            _position += sizeof(T);
         }

         return *this;
      }
      MemoryListIterator operator ++(int)
      {
         MemoryListIterator tmp = *this;
         ++* this;

         return tmp;
      }

      MemoryListIterator& operator --()
      {
         if (_position >= sizeof(T)) {
            _position -= sizeof(T);
         }

         return *this;
      }
      MemoryListIterator operator --(int)
      {
         MemoryListIterator tmp = *this;
         -- *this;

         return tmp;
      }

      T& operator*()
      {
         _buffer->read(_position, &_current, sizeof(T));

         return _current;
      }

      void flush()
      {
         _buffer->write(_position, &_current, sizeof(T));
      }

      bool eof() const { return _position >= _length; }

      MemoryListIterator(MemoryDump* buffer, pos_t length)
      {
         _buffer = buffer;
         _position = 0;
         _length = length;
      }
      MemoryListIterator()
      {
         _buffer = nullptr;
         _position = 0;
         _length = 0;
      }
   };

   // --- ListBase ---
   template <class T, void(*FreeT)(T)> class ListBase
   {
      typedef ItemBase<T> Item;

      pos_t _count;
      Item* _top, * _tale;
      T     _defaultValue;

   public:
      typedef ListIteratorBase<T, Item, FreeT> Iterator;

      Iterator start() { return Iterator(_top); }

      Iterator end() { return Iterator(_tale); }

      pos_t count() const { return _count; }

      T peek() const { return _top->item; }

      T DefaultValue() const { return _defaultValue; }

      T getAt(int index) const
      {
         if (!index)
            return _defaultValue;

         int currentIndex = 1;
         Item* current = _top;
         while (currentIndex < index) {
            current = current->next;
            currentIndex++;
         }

         return current != nullptr ? current->item : _defaultValue;
      }

      void addToTop(T item)
      {
         _top = new Item(item, _top);
         if (!_tale)
            _tale = _top;

         _count++;
      }
      void addToTale(T item)
      {
         if (_tale != nullptr) {
            _tale->next = new Item(item, nullptr);
            _tale = _tale->next;
         }
         else _top = _tale = new Item(item, nullptr);
         _count++;
      }

      void insert(int index, T item)
      {
         if (!index)
            return;

         int currentIndex = 1;
         Item* current = _top;
         Item* prev = nullptr;
         while (currentIndex < index) {
            prev = current;
            current = current->next;
            currentIndex++;
         }

         if (prev == nullptr) {
            addToTop(item);
         }
         else if (current != nullptr) {
            prev->next = new Item(item, prev->next);

            _count++;
         }
         else addToTale(item);
      }

      void insertBefore(Iterator& it, T item)
      {
         it._current->next = new Item(it._current->item, it._current->next);
         it._current->item = item;

         _count++;
         ++it;
      }

      void insertAfter(Iterator& it, T item)
      {
         it._current->next = new Item(item, it._current->next);

         _count++;
      }

      T cutTop()
      {
         Item* tmp = _top;
         _top = _top->next;
         _count--;

         if (tmp == _tale)
            _tale = nullptr;

         T item = tmp->item;
         freeobj(tmp);

         return item;
      }

      void cut(T item)
      {
         Item* tmp = nullptr;
         Item* previous = nullptr;

         if (!_top)
            return;

         if (item == _top->item)
            tmp = _top;
         else {
            previous = _top;
            while (previous->next) {
               if (previous->next->item == item) {
                  tmp = previous->next;
                  break;
               }
               previous = previous->next;
            }
         }
         if (tmp) {
            _count--;

            if (_top == _tale) {
               _top = _tale = nullptr;
            }
            else if (previous == nullptr) {
               _top = _top->next;
            }
            else if (_tale == tmp) {
               _tale = previous;
               previous->next = tmp->next;
            }
            else previous->next = tmp->next;

            if (FreeT)
               FreeT(tmp->item);

            delete tmp;
         }
      }

      void cut(Iterator& it)
      {
         Item* tmp = nullptr;
         Item* previous = nullptr;

         if (_top == it._current)
            tmp = _top;
         else {
            previous = _top;
            while (previous->next) {
               if (previous->next == it._current) {
                  tmp = previous->next;
                  break;
               }
               previous = previous->next;
            }
         }
         if (tmp) {
            _count--;

            if (_top == _tale) {
               _top = _tale = nullptr;
            }
            else if (previous == nullptr) {
               _top = _top->next;
            }
            else if (_tale == tmp) {
               _tale = previous;
               previous->next = tmp->next;
            }
            else previous->next = tmp->next;

            if (FreeT)
               FreeT(tmp->item);

            delete tmp;
         }
      }

      void clear()
      {
         while (_top) {
            Item* tmp = _top;
            _top = _top->next;

            if (FreeT)
               FreeT(tmp->item);

            delete tmp;
         }
         _count = 0;
         _top = _tale = nullptr;
      }

      ListBase(T defValue)
      {
         _count = 0;
         _tale = _top = nullptr;
         _defaultValue = defValue;
      }

      virtual ~ListBase()
      {
         clear();
      }
   };

   // --- BListBase ---
   template <class T, void(*FreeT)(T)> class BListBase
   {
      typedef BItemBase<T> Item;

      Item *_top, *_tale;
      pos_t _count;

   public:
      typedef ListIteratorBase<T, BItemBase<T>, FreeT> Iterator;

      pos_t count() const
      {
         return _count;
      }

      Iterator start() { return Iterator(_top); }

      Iterator end() { return Iterator(_tale); }

      void insertAfter(Iterator it, T item)
      {
         Item* nextItem = it._current->next;

         it._current->next = new Item(item, it._current, nextItem);

         if (nextItem) {
            nextItem->previous = it._current->next;
         }
         else _tale = it._current->next;

         _count++;
      }

      void insertBefore(Iterator it, T item)
      {
         Item* previousItem = it._current->previous;

         it._current->previous = new Item(item, previousItem, it._current);

         if (previousItem) {
            previousItem->next = it._current->previous;
         }
         else _top = it._current->previous;

         _count++;
      }

      void add(T value)
      {
         Item* item = new Item(value, _tale, nullptr);
         if (_tale != nullptr) {
            _tale->next = item;
            _tale = item;
         }
         else _top = _tale = item;

         _count++;
      }

      void clear()
      {
         while (_count > 0) {
            Item* tmp = _top;
            _top = _top->next;

            if (FreeT)
               FreeT(tmp->item);

            delete tmp;
            _count--;
         }
         _top = _tale = nullptr;
      }

      BListBase()
      {
         _top = _tale = nullptr;
         _count = 0;
      }
   };

   // --- List ---
   template <class T, void(*FreeT)(T) = nullptr> class List
   {
      ListBase<T, FreeT> _list;
      T                  _defaultItem;

   public:
      typedef ListIteratorBase<T, ItemBase<T>, FreeT> Iterator;

      T DefaultValue() const { return _list.DefaultValue(); }

      pos_t count() const { return _list.count(); }

      int count_int() const { return (int)_list.count(); }

      short count_short() const { return (short)_list.count(); }

      Iterator start()
      {
         return _list.start();
      }

      Iterator end()
      {
         return _list.end();
      }

      void add(T item)
      {
         _list.addToTale(item);
      }

      void insert(T item)
      {
         _list.addToTop(item);
      }

      T peek() const
      {
         return _list.peek();
      }

      T get(int index) const
      {
         return _list.getAt(index);
      }

      void cut(T item)
      {
         _list.cut(item);
      }

      void cut(Iterator it)
      {
         _list.cut(it);
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, T item))
      {
         auto it = start();
         while (!it.eof()) {
            lambda(arg, *it);

            ++it;
         }
      }

      template<class ArgT> T retrieve(ArgT arg, bool(*lambda)(ArgT arg, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            if (lambda(arg, *it))
               return *it;

            ++it;
         }

         return _defaultItem;
      }

      template<class ArgT> int retrieveIndex(ArgT arg, bool(*lambda)(ArgT arg, T item))
      {
         auto it = start();
         int index = 0;
         while (!it.eof())
         {
            if (lambda(arg, *it))
               return index;

            index++;

            ++it;
         }

         return -1;
      }

      void clear()
      {
         _list.clear();
      }

      List(T defValue)
         : _list(defValue)
      {
         _defaultItem = defValue;
      }
      virtual ~List() = default;
   };

   // --- SortedList ---
   template <class T, int(*SortT)(T,T), void(*FreeT)(T) = nullptr> class SortedList
   {
      ListBase<T, FreeT> _list;
      T                  _defaultItem;

   public:
      typedef ListIteratorBase<T, ItemBase<T>, FreeT> Iterator;

      T DefaultValue() const { return _list.DefaultValue(); }

      pos_t count() const { return _list.count(); }

      Iterator start()
      {
         return _list.start();
      }

      Iterator end()
      {
         return _list.end();
      }

      void add(T item)
      {
         int len = count();
         for (int i = 1; i <= len; i++) {
            if (SortT(_list.getAt(i), item) < 0) {
               _list.insert(i, item);

               return;
            }
         }

         _list.addToTale(item);
      }

      T peek() const
      {
         return _list.peek();
      }

      T get(int index) const
      {
         return _list.getAt(index);
      }

      void cut(T item)
      {
         _list.cut(item);
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, T item))
      {
         auto it = start();
         while (!it.eof()) {
            lambda(arg, *it);

            ++it;
         }
      }

      template<class ArgT> T retrieve(ArgT arg, bool(*lambda)(ArgT arg, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            if (lambda(arg, *it))
               return *it;

            ++it;
         }

         return _defaultItem;
      }

      template<class ArgT> int retrieveIndex(ArgT arg, bool(*lambda)(ArgT arg, T item))
      {
         auto it = start();
         int index = 0;
         while (!it.eof())
         {
            if (lambda(arg, *it))
               return index;

            index++;

            ++it;
         }

         return -1;
      }

      void clear()
      {
         _list.clear();
      }

      SortedList(T defValue)
         : _list(defValue)
      {
         _defaultItem = defValue;
      }
      virtual ~SortedList() = default;
   };

   // --- Stack ---
   template <class T> class Stack
   {
      ListBase<T> _list;
      T           _defaultItem;

   public:
      typedef ListIteratorBase<T, ItemBase<T>, nullptr> Iterator;

      Iterator start()
      {
         return _list.start();
      }

      Iterator end()
      {
         return _list.end();
      }

      T DefaultValue() const { return _list.DefaultValue(); }

      Iterator get(int index)
      {
         Iterator it = start();
         while (!it.eof() && index > 0) {
            index--;
            it++;
         }
         return it;
      }

      void insert(Iterator it, T item)
      {
         if (it.eof()) {
            _list.addToTale(item);
         }
         else _list.insertAfter(it, item);
      }

      void push(T item)
      {
         _list.addToTop(item);
      }

      T peek() const
      {
         if (_list.count() != 0) {
            return _list.peek();
         }
         return _defaultItem;
      }

      T pop()
      {
         if (_list.count() != 0) {
            return _list.cutTop();
         }
         return _defaultItem;
      }

      void clear()
      {
         _list.clear();
      }

      pos_t count() const
      {
         return _list.count();
      }

      Stack(T defItem)
         : _list(defItem)
      {
         _defaultItem = defItem;
      }
      ~Stack() = default;
   };

   // --- Queue ---
   template <class T> class Queue
   {
      ListBase<T> _list;
      T           _defaultItem;

   public:
      pos_t count() const { return  _list.count(); }

      T DefaultValue() const { return _list.DefaultValue(); }

      void insert(T item)
      {
         _list.addToTop(item);
      }

      void push(T item)
      {
         _list.addToTale(item);
      }

      T pop()
      {
         if (_list.count() != 0) {
            return _list.cutTop();
         }
         else return _defaultItem;
      }

      Queue(T defItem)
         : _list(defItem)
      {
         _defaultItem = defItem;
      }
      ~Queue() = default;
   };

   // --- BList ---
   template <class T, void(*FreeT)(T) = nullptr> class BList
   {
      BListBase<T, FreeT> _list;

   public:
      typedef ListIteratorBase<T, BItemBase<T>, FreeT> Iterator;

      pos_t count() const { return _list.count(); }

      Iterator start()
      {
         return _list.start();
      }

      Iterator end()
      {
         return _list.end();
      }

      void add(T value)
      {
         _list.add(value);
      }

      void insertAfter(Iterator it, T item)
      {
         if (it.eof()) {
            add(item);
         }
         else _list.insertAfter(it, item);
      }

      void clear()
      {
         _list.clear();
      }

      BList()
      {
      }
      ~BList()
      {
         clear();
      }
   };

   // --- Hash functions ---
   constexpr size_t cnUStrHashSize = 27;
   inline size_t mapUStrHash(ustr_t s)
   {
      char ch = s[0];

      if (ch >= 'a' && s[0] <= 'z') {
         return ch - 'a';
      }
      else if (ch >= 'A' && s[0] <= 'Z') {
         return ch - 'A';
      }
      return 26;
   }

   // --- Map ---
   inline ustr_t allocUStr(ustr_t key)
   {
      return key.clone();
   }

   inline void freeUStr(ustr_t key)
   {
      freestr((char*)key.str());
   }

   template <class Key, class T, Key(*AllocKey)(Key)=nullptr, void(*FreeKey)(Key) = nullptr, void(*FreeT)(T) = nullptr> class Map
   {
      typedef MapItemBase<Key, T, AllocKey, FreeKey> Item;

      Item* _top, * _tale;
      pos_t _count;

      T     _defaultItem;

   public:
      typedef IteratorBase<Key, T, Item> Iterator;

      T DefaultValue() const { return _defaultItem; }

      pos_t count() const { return _count; }

      int count_int() const { return (int)_count; }

      Iterator start() const
      {
         return Iterator(_top);
      }

      Iterator end() const
      {
         return Iterator(_tale);
      }

      void addToTop(Key key, T item)
      {
         _top = new Item(key, item, _top);
         if (!_tale)
            _tale = _top;

         _count++;
      }

      void add(Key key, T item)
      {
         if (_tale != nullptr) {
            _tale->next = new Item(key, item, nullptr);
            _tale = _tale->next;
         }
         else _top = _tale = new Item(key, item, nullptr);

         _count++;
      }
      bool add(Key key, T item, bool unique)
      {
         if (!unique || !exist(key, item)) {
            add(key, item);

            return true;
         }
         else return false;
      }

      Iterator getIt(Key key) const
      {
         Item* current = _top;
         while (current) {
            if (current->key == key) {
               return Iterator(current);
            }

            current = current->next;
         }

         return Iterator();
      }

      Iterator nextIt(Key key, Iterator it)
      {
         Item* current = it._item();
         current = current->next;
         while (current) {
            if (current->key == key) {
               return Iterator(current);
            }

            current = current->next;
         }

         return Iterator();
      }

      T get(Key key) const
      {
         Item* current = _top;
         while (current) {
            if (current->key == key) {
               return current->item;
            }

            current = current->next;
         }

         return _defaultItem;
      }

      bool exist(Key key) const
      {
         Item* current = _top;
         while (current) {
            if (current->key == key) {
               return true;
            }

            current = current->next;
         }
         return false;
      }
      bool exist(Key key, T value) const
      {
         Item* current = _top;
         while (current) {
            if (current->key == key && current->item == value) {
               return true;
            }

            current = current->next;
         }

         return false;
      }

      T exclude(Key key)
      {
         Item* tmp = nullptr;
         if (!_top);
         else if (_top->key == key) {
            tmp = _top;
            if (_top == _tale)
               _tale = nullptr;
            _top = _top->next;
         }
         else {
            Item* cur = _top;
            while (cur->next) {
               if (cur->next->key == key) {
                  if (cur->next == _tale)
                     _tale = cur;

                  tmp = cur->next;
                  cur->next = tmp->next;
                  break;
               }
               cur = cur->next;
            }
         }
         if (tmp) {
            _count--;

            return tmp->item;
         }
         else return _defaultItem;
      }

      void erase(Key key)
      {
         Item* tmp = nullptr;
         if (!_top);
         else if (_top->key == key) {
            tmp = _top;
            if (_top == _tale)
               _tale = nullptr;
            _top = _top->next;
         }
         else {
            Item* cur = _top;
            while (cur->next) {
               if (cur->next->key == key) {
                  if (cur->next == _tale)
                     _tale = cur;

                  tmp = cur->next;
                  cur->next = tmp->next;
                  break;
               }
               cur = cur->next;
            }
         }
         if (tmp) {
            if (FreeT)
               FreeT(tmp->item);

            delete tmp;

            _count--;
         }

      }

      void erase(Key key, T value)
      {
         Item* tmp = nullptr;
         if (!_top);
         else if (_top->key == key && _top->item == value) {
            tmp = _top;
            if (_top == _tale)
               _tale = nullptr;
            _top = _top->next;
         }
         else {
            Item* cur = _top;
            while (cur->next) {
               if (cur->next->key == key && cur->next->item == value) {
                  if (cur->next == _tale)
                     _tale = cur;

                  tmp = cur->next;
                  cur->next = tmp->next;
                  break;
               }
               cur = cur->next;
            }
         }
         if (tmp) {
            if (FreeT)
               FreeT(tmp->item);

            delete tmp;

            _count--;
         }
      }

      void clear()
      {
         while (_top) {
            Item* tmp = _top;
            _top = _top->next;

            if (FreeT)
               FreeT(tmp->item);

            delete tmp;
         }
         _count = 0;
         _top = _tale = nullptr;
      }

      template<class SumT> SumT sum(SumT initValue, SumT(*lambda)(T item))
      {
         SumT value = initValue;

         auto it = start();
         while (!it.eof())
         {
            value += lambda(*it);

            ++it;
         }

         return value;
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            lambda(arg, it.key(), *it);

            ++it;
         }
      }

      template<class ArgT> Key retrieve(Key defKey, ArgT arg, bool(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof()) {
            if (lambda(arg, it.key(), *it))
               return it.key();

            ++it;
         }

         return defKey;
      }

      Map(T devValue)
      {
         _top = _tale = nullptr;
         _count = 0;
         _defaultItem = devValue;
      }

      virtual ~Map() { clear(); }
   };

   // --- FixedMemoryMap ---

   template<class Key, class T, Key defKey = 0> class FixedMemoryMap
   {
      MemoryDump _buffer;
      T          _defValue;

   public:
      struct Iterator
      {
         void* _buffer;
         pos_t _position;
         pos_t _end;

         bool eof() const
         {
            return _position >= _end || _position < 0;
         }

         T& operator *() const
         {
            return *(T*)((char*)_buffer + _position + sizeof(Key));
         }

         Key& key() const
         {
            return *(Key*)((char*)_buffer + _position);
         }

         Iterator& operator ++()
         {
            _position += sizeof(Key) + sizeof(T);

            return *this;
         }
         Iterator operator ++(int)
         {
            Iterator tmp = *this;

            ++* this;

            return tmp;
         }

         Iterator(void* buffer, pos_t position)
         {
            _buffer = buffer;
            _position = position;
            _end = (*(pos_t*)_buffer) * (sizeof(T) + sizeof(Key));
         }
         Iterator(void* buffer)
            : Iterator(buffer, 4)
         {
         }
      };

      T DefaultValue() const { return _defValue; }

      void* Address() const
      {
         return _buffer.get(0);
      }

      Iterator start()
      {
         return Iterator(_buffer.get(0), 4);
      }

      pos_t sizeInMemory()
      {
         return _buffer.length();
      }

      pos_t count()
      {
         pos_t count = _buffer.getPos(0);

         return count;
      }

      void add(Key key, T item)
      {
         pos_t position = _buffer.length();
         _buffer.write(position, &key, sizeof(key));
         _buffer.write(position + sizeof(key), &item, sizeof(T));

         _buffer.writePos(0, _buffer.getPos(0) + 1);
      }

      T get(Key key)
      {
         pos_t count = _buffer.getPos(0);
         pos_t position = sizeof(pos_t);
         while (count != 0) {
            Key currentKey = defKey;
            _buffer.read(position, &currentKey, sizeof(Key));
            position += sizeof(Key);

            if (currentKey == key) {
               T value = _defValue;
               _buffer.read(position, &value, sizeof(T));

               return value;
            }

            count--;

            position += sizeof(T);
         }

         return _defValue;
      }

      bool exist(Key key)
      {
         pos_t count = _buffer.getPos(0);
         pos_t position = sizeof(pos_t);
         while (count != 0) {
            Key currentKey = defKey;
            _buffer.read(position, &currentKey, sizeof(Key));
            position += sizeof(Key);

            if (currentKey == key) {
               return true;
            }

            count--;

            position += sizeof(T);
         }

         return false;
      }

      T* getPtr(Key key)
      {
         pos_t count = _buffer.getPos(0);
         pos_t position = sizeof(pos_t);
         while (count != 0) {
            Key currentKey = defKey;
            _buffer.read(position, &currentKey, sizeof(Key));
            if (currentKey == key)
               return (T*)_buffer.get(position + sizeof(Key));

            count--;

            position += sizeof(Key) + sizeof(T);
         }

         return nullptr;
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, Key key, T item))
      {
         pos_t count = _buffer.getPos(0);
         pos_t position = sizeof(pos_t);
         while (count != 0) {
            Key currentKey = defKey;
            _buffer.read(position, &currentKey, sizeof(Key));
            position += sizeof(Key);

            T value = _defValue;
            _buffer.read(position, &value, sizeof(Key));
            position += sizeof(T);

            lambda(arg, currentKey, value);

            count--;
         }
      }

      FixedMemoryMap(T defValue)
      {
         _defValue = defValue;

         _buffer.writePos(0, 0);
      }
   };

   inline pos_t Map_StoreUInt(MemoryDump* dump, unsigned int mssg)
   {
      pos_t position = dump->length();

      dump->writeUInt(position, mssg);

      return position;
   }

   inline unsigned int Map_GetUInt(MemoryDump* dump, pos_t position)
   {
      unsigned int* keyPtr = (unsigned int*)dump->get(position);

      return *keyPtr;
   }

   inline pos_t Map_StoreInt(MemoryDump* dump, int n)
   {
      pos_t position = dump->length();

      dump->writeInt(position, n);

      return position;
   }

   inline int Map_GetInt(MemoryDump* dump, pos_t position)
   {
      int* keyPtr = (int*)dump->get(position);

      return *keyPtr;
   }

   inline pos_t Map_StoreAddr(MemoryDump* dump, addr_t addr)
   {
      pos_t position = dump->length();

      dump->write(position, &addr, sizeof(addr));

      return position;
   }

   inline addr_t Map_GetAddr(MemoryDump* dump, pos_t position)
   {
      addr_t addr = 0;
      dump->read(position, &addr, sizeof(addr));

      return addr;
   }

   inline pos_t Map_StoreUStr(MemoryDump* dump, ustr_t s)
   {
      pos_t position = dump->length();

      dump->write(position, s.str(), getlength_pos(s) + 1);

      return position;
   }

   inline ustr_t Map_GetUStr(MemoryDump* dump, pos_t position)
   {
      const char* s = (const char*)dump->get(position);

      return ustr_t(s);
   }

   // --- MemoryMap ---
   template<class Key, class T, pos_t(*StoreKey)(MemoryDump*, Key), Key(*GetKey)(MemoryDump*, pos_t), void(*FreeT)(T) = nullptr> class MemoryMap
   {
      typedef MemoryMapItemBase<Key, T, GetKey> Item;

      MemoryDump _buffer;
      pos_t      _top, _tale;
      pos_t      _count;

      T          _defValue;

   public:
      typedef MemoryIteratorBase<Key, T, Item> Iterator;

      T DefaultValue() const { return _defValue; }

      Iterator start() const
      {
         if (_count == 0) {
            return {};
         }
         else {
            return Iterator((MemoryDump*)&_buffer, (Item*)_buffer.get(_top));
         }
      }

      Iterator getIt(Key key) const
      {
         for (Iterator it = start(); !it.eof(); ++it) {
            if (it.key() == key)
               return it;
         }

         return {};
      }

      pos_t count() const { return _count; }

      bool exist(Key key)
      {
         if (_top) {
            pos_t currentOffset = _top;
            while (currentOffset) {
               Item* current = (Item*)_buffer.get(currentOffset);
               if (current->readKey(&_buffer) == key)
                  return true;

               currentOffset = current->nextOffset;
            }
         }

         return false;
      }
      bool exist(Key key, T value)
      {
         if (_top) {
            pos_t currentOffset = _top;
            while (currentOffset) {
               Item* current = (Item*)_buffer.get(currentOffset);
               if (current->readKey(&_buffer) == key && current->item == value)
                  return true;

               currentOffset = current->nextOffset;
            }
         }

         return false;
      }

      T get(Key key)
      {
         if (_top) {
            pos_t currentOffset = _top;
            while (currentOffset) {
               Item* current = (Item*)_buffer.get(currentOffset);

               if (current->readKey(&_buffer) == key)
                  return current->item;

               currentOffset = current->nextOffset;
            }
         }

         return _defValue;
      }

      void add(Key key, T value)
      {
         pos_t keyPosition = StoreKey(&_buffer, key);

         Item item(keyPosition, value, 0);

         pos_t position = _buffer.length();
         _buffer.write(position, &item, sizeof(item));

         if (_tale) {
            Item* taleItem = (Item*)_buffer.get(_tale);
            taleItem->nextOffset = position;
         }
         else _top = position;

         _tale = position;

         _count++;
      }

      bool add(Key key, T item, bool unique)
      {
         if (!unique || !exist(key)) {
            add(key, item);

            return true;
         }
         else return false;
      }

      T exclude(Key key)
      {
         if (_top) {
            pos_t currentOffset = _top;
            pos_t previousOffset = INVALID_POS;
            while (currentOffset) {
               Item* current = (Item*)_buffer.get(currentOffset);

               if (current->readKey(&_buffer) == key) {
                  if (previousOffset == INVALID_POS) {
                     _top = current->nextOffset;
                  }
                  else {
                     Item* previous = (Item*)_buffer.get(previousOffset);
                     previous->nextOffset = current->nextOffset;
                  }

                  if (_tale == currentOffset)
                     _tale = previousOffset;

                  _count--;
                  if (!_count)
                     _top = _tale = 0;

                  return current->item;
               }

               previousOffset = currentOffset;
               currentOffset = current->nextOffset;
            }
         }
         return _defValue;
      }

      T exclude(Key key, T value)
      {
         if (_top) {
            pos_t currentOffset = _top;
            pos_t previousOffset = -1;
            while (currentOffset) {
               Item* current = (Item*)_buffer.get(currentOffset);

               if (current->readKey(&_buffer) == key && current->item == value) {
                  if (previousOffset == INVALID_POS) {
                     _top = current->nextOffset;
                  }
                  else {
                     Item* previous = (Item*)_buffer.get(previousOffset);
                     previous->nextOffset = current->nextOffset;
                  }

                  if (_tale == currentOffset)
                     _tale = previousOffset;

                  _count--;
                  if (!_count)
                     _top = _tale = 0;

                  return current->item;
               }

               previousOffset = currentOffset;
               currentOffset = current->nextOffset;
            }
         }
         return _defValue;
      }

      void clear()
      {
         _buffer.clear();

         _count = 0;
         _top = _tale = 0;
      }

      template<class SumT> SumT sum(SumT initValue, SumT(*lambda)(T item))
      {
         SumT value = initValue;

         auto it = start();
         while (!it.eof())
         {
            value += lambda(*it);

            ++it;
         }

         return value;
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            lambda(arg, it.key(), *it);

            ++it;
         }
      }

      template<class ArgT> Key retrieve(Key defKey, ArgT arg, bool(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            if (lambda(arg, it.key(), *it))
               return it.key();

            ++it;
         }

         return defKey;
      }

      MemoryMap(T defValue)
      {
         _top = _tale = 0;
         _count = 0;
         _defValue = defValue;
      }
   };

   // --- Cache ---
   template<class Key, class T, size_t cacheSize> class Cache
   {
      struct Item
      {
         Key            key;
         T              item;
      };

      Item    _items[cacheSize];
      T       _defValue;
      size_t _top;
      size_t _tale;

      void incIndex(size_t& index) const
      {
         index++;

         if (index == cacheSize)
            index = 0;
      }

   public:
      void add(Key key, T value)
      {
         _items[_tale].key = key;
         _items[_tale].item = value;

         incIndex(_tale);
         if (_tale == _top) {
            incIndex(_top);
         }
      }

      T get(Key key)
      {
         size_t i = _top;
         while (i != _tale) {
            if (_items[i].key == key) {
               return _items[i].item;
            }
            incIndex(i);
         };

         return _defValue;
      }

      void clear()
      {
         _top = _tale = 0;
      }

      Cache(T defValue)
      {
         _top = _tale = 0;
         _defValue = defValue;
      }
   };

   // --- HashTable ---
   template <class Key, class T, pos_t(_scaleKey)(Key), pos_t hashSize, Key(*AllocKey)(Key) = nullptr, void(*FreeKey)(Key) = nullptr, void(*FreeT)(T) = nullptr> class HashTable
   {
      typedef MapItemBase<Key, T, AllocKey, FreeKey> Item;

      class HashTableIterator
      {
         friend class HashTable;

         Item*            _current;
         pos_t            _hashIndex;
         const HashTable* _hashTable;

         HashTableIterator(const HashTable* hashTable, pos_t hashIndex, Item* current)
         {
            _hashTable = hashTable;
            _hashIndex = hashIndex;
            _current = current;
         }
         HashTableIterator(const HashTable* hashTable)
         {
            _hashTable = hashTable;
            _current = nullptr;
            _hashIndex = hashSize;

            if (_hashTable->count() != 0) {
               for (_hashIndex = 0; !_hashTable->_table[_hashIndex] && _hashIndex < hashSize; _hashIndex++);

               _current = _hashTable->_table[_hashIndex];
            }
         }

      public:
         bool operator ==(const HashTableIterator& it)
         {
            return _current == it._current && _hashIndex == it._hashIndex;
         }
         bool operator !=(const HashTableIterator& it)
         {
            return _current != it._current || _hashIndex != it._hashIndex;
         }

         HashTableIterator& operator =(const HashTableIterator& it)
         {
            this->_current = it._current;
            this->_hashIndex = it._hashIndex;
            this->_hashTable = it._hashTable;

            return *this;
         }

         HashTableIterator& operator ++()
         {
            _current = _current->next;
            while (!_current && _hashIndex < (hashSize - 1)) {
               _current = _hashTable->_table[++_hashIndex];
            }
            return *this;
         }
         HashTableIterator operator ++(int)
         {
            HashTableIterator tmp = *this;
            ++* this;

            return tmp;
         }

         T& operator*() const { return _current->item; }

         Key key() const { return _current->key; }

         bool eof() const { return _current == nullptr; }

         HashTableIterator()
         {
            _current = nullptr;
            _hashTable = hashSize;
            _hashIndex = 0;
         }
      };

      friend class HashTableIterator;

      size_t _count;
      Item*  _table[hashSize];

      T      _defValue;

   public:
      typedef HashTableIterator    Iterator;

      T DefaultValue() const { return _defValue; }

      pos_t count() const { return _count; }

      Iterator start() const
      {
         return Iterator(this);
      }

      Iterator end() const
      {
         return Iterator(this, hashSize, nullptr);
      }

      void add(Key key, T item)
      {
         pos_t index = _scaleKey(key);
         if (index >= hashSize)
            index = hashSize - 1;

         if (_table[index] && _table[index]->key <= key) {
            Item* current = _table[index];
            while (current->next && (current->next)->key <= key) {
               current = current->next;
            }
            current->next = new Item(key, item, current->next);
         }
         else _table[index] = new Item(key, item, _table[index]);

         _count++;
      }
      bool add(Key key, T item, bool unique)
      {
         if (!unique || !exist(key, item)) {
            add(key, item);

            return true;
         }
         else return false;
      }

      Iterator getIt(Key key) const
      {
         pos_t index = _scaleKey(key);
         if (index >= hashSize)
            index = hashSize - 1;

         Item* current = _table[index];
         while (current && (current->key < key))
            current = current->next;

         if (current && (current->key != key)) {
            return Iterator(this, hashSize, nullptr);
         }
         else return Iterator(this, index, current);
      }

      Iterator nextIt(Key key, Iterator it)
      {
         pos_t index = it._hashIndex;

         Item* current = it._current->next;

         if (current && (current->key != key)) {
            return Iterator(this, hashSize, nullptr);
         }
         else return Iterator(this, index, current);
      }

      T get(Key key) const
      {
         Iterator it = getIt(key);

         return it.eof() ? _defValue : *it;
      }

      bool exist(Key key) const
      {
         Iterator it = getIt(key);

         return !it.eof();
      }
      bool exist(Key key, T value) const
      {
         Iterator it = getIt(key);
         while (!it.eof() && it.key() == key) {
            if (*it == value)
               return true;

            ++it;
         }
         return false;
      }

      template<class SumT> SumT sum(SumT initValue, SumT(*lambda)(T item))
      {
         SumT value = initValue;

         auto it = start();
         while (!it.eof())
         {
            value += lambda(*it);

            ++it;
         }

         return value;
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            lambda(arg, it.key(), *it);

            ++it;
         }
      }

      template<class ArgT> Key retrieve(Key defKey, ArgT arg, bool(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof()) {
            if (lambda(arg, it.key(), *it))
               return it.key();

            ++it;
         }

         return defKey;
      }

      void clear()
      {
         for (size_t i = 0; i < hashSize; i++) {
            while (_table[i]) {
               Item* tmp = _table[i];
               _table[i] = _table[i]->next;

               if (FreeT)
                  FreeT(tmp->item);

               delete tmp;
            }
            _table[i] = nullptr;
         }
         _count = 0;
      }

      HashTable(T defaultItem)
      {
         _defValue = defaultItem;
         _count = 0;

         for (size_t i = 0; i < hashSize; i++) {
            _table[i] = nullptr;
         }
      }
      ~HashTable()
      {
         clear();
      }
   };

   // --- MemoryHashTable template ---
   template <class Key, class T, pos_t(_scaleKey)(Key), pos_t hashSize, pos_t(*StoreKey)(MemoryDump*, Key), Key(*GetKey)(MemoryDump*, pos_t), void(*FreeT)(T) = nullptr> class MemoryHashTable
   {
      typedef MemoryMapItemBase<Key, T, GetKey> Item;

      class MemoryHashTableIterator
      {
         friend class MemoryHashTable;

         const MemoryDump* _buffer;
         pos_t             _position;
         Item*             _current;
         pos_t             _hashIndex;

         MemoryHashTableIterator(const MemoryDump* buffer, unsigned int hashIndex, unsigned int position, Item* current)
         {
            _buffer = buffer;
            _hashIndex = hashIndex;
            _current = current;
            _position = position;
         }
         MemoryHashTableIterator(const MemoryDump* buffer)
         {
            _buffer = buffer;
            _current = nullptr;

            if (buffer->length() > 0) {
               for (_hashIndex = 0; _hashIndex < hashSize && !(*buffer)[_hashIndex << 2]; _hashIndex++);

               if (_hashIndex < hashSize) {
                  _position = _buffer->getPos(_hashIndex << 2);
                  _current = (Item*)_buffer->get(_position);
               }
               else _current = nullptr;
            }
         }

      public:
         MemoryHashTableIterator& operator =(const MemoryHashTableIterator& it)
         {
            this->_current = it._current;
            this->_position = it._position;
            this->_buffer = it._buffer;
            this->_hashIndex = it._hashIndex;

            return *this;
         }

         MemoryHashTableIterator& operator++()
         {
            if (_current->next != 0) {
               _position = _current->next;

               if (_position != 0) {
                  _current = (Item*)_buffer->get(_position);
               }
               else _current = NULL;
            }
            else _current = NULL;

            while (!_current && _hashIndex < (hashSize - 1)) {
               _hashIndex++;

               _position = _buffer->getPos(_hashIndex << 2);
               if (_position != 0)
                  _current = (Item*)_buffer->get(_position);
            }
            return *this;
         }

         MemoryHashTableIterator operator++(int)
         {
            MemoryHashTableIterator tmp = *this;
            ++* this;

            return tmp;
         }

         T operator*() const { return _current->item; }

         Key key() const
         {
            return _current->readKey(_buffer);
         }

         bool eof() const { return (_current == nullptr); }

         MemoryHashTableIterator()
         {
            _current = nullptr;
            _position = 0;
            _hashIndex = hashSize;
            _buffer = nullptr;
         }
      };
      friend class MemoryHashTableIterator;

      MemoryDump _buffer;
      pos_t      _count;

      T          _defaultItem;

   public:
      typedef MemoryHashTableIterator Iterator;

      pos_t count() const { return _count; }

      T DefaultValue() const { return _defaultItem; }

      Iterator start() const
      {
         return Iterator(&_buffer);
      }

      Iterator getIt(Key key) const
      {
         pos_t beginning = (pos_t)_buffer.get(0);

         pos_t index = _scaleKey(key);
         if (index > hashSize)
            index = hashSize - 1;

         pos_t position = _buffer.getPos(index << 2);
         Item* current = (position != 0) ? (Item*)(beginning + position) : nullptr;
         while (current && current->readKey((MemoryDump*)&_buffer) < key) {
            if (current->nextOffset != 0) {
               position = current->nextOffset;
               current = (Item*)(beginning + position);
            }
            else current = nullptr;
         }

         if (current && (current->readKey((MemoryDump*)&_buffer) != key)) {
            return Iterator(&_buffer, index, 0, nullptr);
         }
         else return Iterator(&_buffer, index, position, current);
      }

      T get(Key key) const
      {
         Iterator it = getIt(key);

         return it.eof() ? _defaultItem : *it;
      }

      bool exist(Key key) const
      {
         Iterator it = getIt(key);
         if (!it.eof()) {
            return true;
         }
         else return false;
      }

      void add(Key key, T value)
      {
         pos_t keyPosition = StoreKey(&_buffer, key);

         Item item(keyPosition, value, 0);

         pos_t index = _scaleKey(key);
         if (index > hashSize)
            index = hashSize - 1;

         pos_t position = _buffer.getPos(index << 2);

         pos_t tale = _buffer.length();

         _buffer.write(tale, &item, sizeof(item));

         size_t beginning = (size_t)_buffer.get(0);

         Item* current = (position != 0) ? (Item*)(beginning + position) : nullptr;
         if (current && current->readKey(&_buffer) <= key) {
            while (current->nextOffset != 0 && ((Item*)(beginning + current->nextOffset))->readKey(&_buffer) <= key) {
               position = current->nextOffset;
               current = (Item*)(beginning + position);
            }
            _buffer.writePos(tale, current->nextOffset);
            current->nextOffset = tale;
         }
         else {
            if (position == 0) {
               _buffer.writePos(tale, 0);
            }
            else _buffer.writePos(tale, position);

            _buffer.writePos(index << 2, tale);
         }

         _count++;
      }

      template<class SumT> SumT sum(SumT initValue, SumT(*lambda)(T item))
      {
         SumT value = initValue;

         auto it = start();
         while (!it.eof())
         {
            value += lambda(*it);

            ++it;
         }

         return value;
      }

      template<class ArgT> void forEach(ArgT arg, void(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            lambda(arg, it.key(), *it);

            ++it;
         }
      }

      template<class ArgT> Key retrieve(Key defKey, ArgT arg, bool(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof()) {
            if (lambda(arg, it.key(), *it))
               return it.key();

            ++it;
         }

         return defKey;
      }

      void clear()
      {
         _buffer.clear();
         _count = 0;

         _buffer.writeBytes(0, 0, hashSize << 2);
      }

      MemoryHashTable(T defaultItem)
      {
         _defaultItem = defaultItem;
         _count = 0;

         _buffer.writeBytes(0, 0, hashSize << 2);
      }
      ~MemoryHashTable()
      {
         clear();
      }
   };

   //// --- Memory32HashTable template ---

   //template <class Key, class T, unsigned int(_scaleKey)(Key), unsigned int hashSize> class Memory32HashTable
   //{
   //   typedef _Memory32MapItem<Key, T> Item;

   //   class Memory32HashTableIterator
   //   {
   //      friend class Memory32HashTable;

   //      const MemoryDump* _buffer;
   //      unsigned int      _position;
   //      Item* _current;
   //      unsigned int      _hashIndex;

   //      Memory32HashTableIterator(const MemoryDump* buffer, unsigned int hashIndex, unsigned int position, Item* current)
   //      {
   //         _buffer = buffer;
   //         _hashIndex = hashIndex;
   //         _current = current;
   //         _position = position;
   //      }
   //      Memory32HashTableIterator(const MemoryDump* buffer)
   //      {
   //         _buffer = buffer;
   //         _current = NULL;

   //         if (buffer->Length() > 0) {
   //            for (_hashIndex = 0; _hashIndex < hashSize && !(*buffer)[_hashIndex << 2]; _hashIndex++);

   //            if (_hashIndex < hashSize) {
   //               _position = (*_buffer)[_hashIndex << 2];
   //               _current = (Item*)_buffer->get(_position);
   //            }
   //            else _current = NULL;
   //         }
   //      }

   //   public:
   //      Memory32HashTableIterator& operator =(const Memory32HashTableIterator& it)
   //      {
   //         this->_current = it._current;
   //         this->_position = it._position;
   //         this->_buffer = it._buffer;
   //         this->_hashIndex = it._hashIndex;

   //         return *this;
   //      }

   //      Memory32HashTableIterator& operator++()
   //      {
   //         if (_current->next != 0) {
   //            _position = _current->next;

   //            if (_position != 0) {
   //               _current = (Item*)_buffer->get(_position);
   //            }
   //            else _current = NULL;
   //         }
   //         else _current = NULL;

   //         while (!_current && _hashIndex < (hashSize - 1)) {
   //            _hashIndex++;

   //            _position = (*_buffer)[_hashIndex << 2];
   //            if (_position != 0)
   //               _current = (Item*)_buffer->get(_position);
   //         }
   //         return *this;
   //      }

   //      Memory32HashTableIterator operator++(int)
   //      {
   //         Memory32HashTableIterator tmp = *this;
   //         ++* this;

   //         return tmp;
   //      }

   //      T operator*() const { return _current->item; }

   //      Key key() const
   //      {
   //         return _current->getKey(Key());
   //      }

   //      bool Eof() const { return (_current == NULL); }

   //      Memory32HashTableIterator()
   //      {
   //         _current = NULL;
   //         _position = 0;
   //         _hashIndex = hashSize;
   //         _buffer = NULL;
   //      }
   //   };
   //   friend class Memory32HashTableIterator;

   //   MemoryDump   _buffer;
   //   unsigned int _count;

   //   T            _defaultItem;

   //public:
   //   typedef Memory32HashTableIterator Iterator;

   //   unsigned int Count() const { return _count; }

   //   T DefaultValue() const { return _defaultItem; }

   //   Iterator start() const
   //   {
   //      return Iterator(&_buffer);
   //   }

   //   Iterator getIt(Key key) const
   //   {
   //      size_t beginning = (size_t)_buffer.get(0);

   //      unsigned int index = _scaleKey(key);
   //      if (index > hashSize)
   //         index = hashSize - 1;

   //      unsigned int position = _buffer[index << 2];
   //      Item* current = (position != 0) ? (Item*)(beginning + position) : NULL;
   //      while (current && *current < key) {
   //         if (current->next != 0) {
   //            position = current->next;
   //            current = (Item*)(beginning + position);
   //         }
   //         else current = NULL;
   //      }

   //      if (current && (*current != key)) {
   //         return Iterator((const MemoryDump*)&_buffer, index, 0, NULL);
   //      }
   //      else return Iterator((const MemoryDump*)&_buffer, index, position, current);
   //   }

   //   T get(Key key) const
   //   {
   //      Iterator it = getIt(key);

   //      return it.Eof() ? _defaultItem : *it;
   //   }

   //   bool exist(const int key, T item) const
   //   {
   //      Iterator it = getIt(key);
   //      while (!it.Eof() && it.key() == key) {
   //         if (*it == item)
   //            return true;

   //         it++;
   //      }
   //      return false;
   //   }

   //   bool exist(Key key) const
   //   {
   //      Iterator it = getIt(key);
   //      if (!it.Eof()) {
   //         return true;
   //      }
   //      else return false;
   //   }

   //   pos_t storeKey(unsigned int, ref_t key)
   //   {
   //      return key;
   //   }

   //   pos_t storeKey(unsigned int position, ident_t key)
   //   {
   //      pos_t offset = _buffer.Length();

   //      _buffer.writeLiteral(offset, key);

   //      return offset - position;
   //   }

   //   pos_t storeKey(unsigned int position, ref64_t key)
   //   {
   //      pos_t offset = _buffer.Length();

   //      _buffer.writeQWord(offset, key);

   //      return offset - position;
   //   }

   //   void add(Key key, T value)
   //   {
   //      Item item(0, value, 0);

   //      unsigned int index = _scaleKey(key);
   //      if (index > hashSize)
   //         index = hashSize - 1;

   //      int position = _buffer[index << 2];

   //      unsigned int tale = _buffer.Length();

   //      _buffer.write(tale, &item, sizeof(item));

   //      // save stored key
   //      pos_t storedKey = storeKey(tale, key);
   //      _buffer.writeDWord(tale + 4, storedKey);

   //      size_t beginning = (size_t)_buffer.get(0);

   //      Item* current = (position != 0) ? (Item*)(beginning + position) : NULL;
   //      if (current && *current <= key) {
   //         while (current->next != 0 && *(Item*)(beginning + current->next) <= key) {
   //            position = current->next;
   //            current = (Item*)(beginning + position);
   //         }
   //         _buffer[tale] = current->next;
   //         current->next = tale;
   //      }
   //      else {
   //         if (position == 0) {
   //            _buffer[tale] = 0;
   //         }
   //         else _buffer[tale] = position;
   //         _buffer[index << 2] = tale;
   //      }

   //      _count++;
   //   }

   //   bool add(int key, T item, bool unique)
   //   {
   //      if (!unique || !exist(key, item)) {
   //         add(key, item);

   //         return true;
   //      }
   //      else return false;
   //   }

   //   void write(StreamWriter* writer)
   //   {
   //      writer->writeDWord(_buffer.Length());
   //      writer->writeDWord(_count);

   //      MemoryReader reader(&_buffer);
   //      writer->read(&reader, _buffer.Length());
   //   }

   //   void read(StreamReader* reader)
   //   {
   //      _buffer.clear();

   //      int length = reader->getDWord();
   //      if (length > 0) {
   //         _buffer.reserve(length);

   //         _count = reader->getDWord();

   //         MemoryWriter writer(&_buffer);
   //         writer.read(reader, length);
   //      }
   //   }

   //   void clear()
   //   {
   //      _buffer.clear();
   //      _count = 0;

   //      _buffer.writeBytes(0, 0, hashSize << 2);
   //   }

   //   Memory32HashTable(T defaultItem)
   //   {
   //      _defaultItem = defaultItem;
   //      _count = 0;

   //      _buffer.writeBytes(0, 0, hashSize << 2);
   //   }
   //   ~Memory32HashTable()
   //   {
   //      clear();
   //   }
   //};

   template <class Key> Key GetKey(MemoryDump* dump, pos_t position)
   {
      Key* keyPtr = (Key*)dump->get(position);

      return *keyPtr;
   }

   template <class Key> pos_t StoreKey(MemoryDump* dump, Key key)
   {
      pos_t position = dump->length();

      dump->write(position, &key, sizeof(key));

      return position;
   }

   // --- CachedMap ---
   // NOTE : Key type should be a simple key
   template <class Key, class T, size_t cacheSize,
      void(*FreeT)(T) = nullptr>
   class CachedMemoryMap
   {
      typedef MemoryMapItemBase<Key, T, GetKey> Item;
      typedef MemoryIteratorBase<Key, T, Item>  MemoryIterator;

      struct CachedItem
      {
         Key key;
         T   item;

         CachedItem() = default;
      };

      class CachedMemoryMapIterator
      {
         friend class CachedMemoryMap;

         CachedMemoryMap* _cachedMap;
         size_t           _index;
         MemoryIterator   _iterator;

         CachedMemoryMapIterator(CachedMemoryMap* cachedMap, size_t index)
         {
            this->_cachedMap = cachedMap;
            this->_index = index;
         }
         CachedMemoryMapIterator(MemoryIterator iterator)
         {
            _cachedMap = nullptr;
            _iterator = iterator;
         }

      public:
         CachedMemoryMapIterator& operator =(const CachedMemoryMapIterator& it)
         {
            this->_iterator = it._iterator;
            this->_cachedMap = it._cachedMap;
            this->_index = it._index;

            return *this;
         }

         CachedMemoryMapIterator& operator++()
         {
            if (_cachedMap && _cachedMap->_cached) {
               if (this->_index < _cachedMap->_count) {
                  _index++;
               }
            }
            else _iterator++;

            return *this;
         }
         CachedMemoryMapIterator operator ++(int)
         {
            CachedMemoryMapIterator tmp = *this;
            ++* this;

            return tmp;
         }

         T& operator*() const
         {
            if (_cachedMap && _cachedMap->_cached) {
               return _cachedMap->_cache[_index].item;
            }
            else return *_iterator;
         }

         Key key() const
         {
            if (_cachedMap && _cachedMap->_cached) {
               return _cachedMap->_cache[_index].key;
            }
            else return _iterator.key();
         }

         bool eof() const
         {
            if (_cachedMap && _cachedMap->_cached) {
               return _index >= _cachedMap->_count;
            }
            else return _iterator.eof();
         }

         CachedMemoryMapIterator()
         {
            this->_cachedMap = nullptr;
            this->_index = 0;
         }
      };
      friend class CachedMemoryMapIterator;

      MemoryMap<Key, T, StoreKey, GetKey, FreeT> _map;

      bool                                       _cached;
      CachedItem                                 _cache[cacheSize];
      size_t                                     _count;

   public:
      typedef CachedMemoryMapIterator Iterator;

      pos_t count()
      {
         if (_cached) {
            return (pos_t)_count;
         }
         else return _map.count();
      }

      Iterator start()
      {
         if (_cached) {
            return Iterator(this, 0);
         }
         else return Iterator(_map.start());
      }

      T get(Key key)
      {
         if (_cached) {
            for (size_t i = 0; i < _count; i++) {
               if (_cache[i].key == key)
                  return _cache[i].item;
            }

            return _map.DefaultValue();
         }
         else return _map.get(key);
      }

      Iterator getIt(Key key)
      {
         if (_cached) {
            for (size_t i = 0; i < _count; i++) {
               if (_cache[i].key == key)
                  return Iterator(this, i);
            }
            return Iterator(this, _count + 1);
         }
         else return Iterator(_map.getIt(key));
      }

      bool exist(Key key)
      {
         return !getIt(key).eof();
      }

      void add(Key key, T value)
      {
         if (_cached) {
            if (cacheSize == _count) {
               _cached = false;

               for (size_t i = 0; i < _count; i++)
                  _map.add(_cache[i].key, _cache[i].item);

               _map.add(key, value);
            }
            else {
               _cache[_count].key = key;
               _cache[_count].item = value;

               _count++;
            }
         }
         else _map.add(key, value);
      }

      T exclude(Key key)
      {
         if (_cached) {
            for (size_t i = 0; i < _count; i++) {
               if (_cache[i].key == key) {
                  T item = _cache[i].item;

                  for (size_t j = i + 1; j < _count; j++) {
                     _cache[i] = _cache[j];
                  }

                  _count--;

                  return item;
               }
            }
            return _map.DefaultValue();
         }
         else return _map.exclude(key);
      }

      T exclude(Key key, T item)
      {
         if (_cached) {
            for (size_t i = 0; i < _count; i++) {
               if (_cache[i].key == key && _cache[i].item == item) {
                  T item = _cache[i].item;

                  for (size_t j = i + 1; j < _count; j++) {
                     _cache[i] = _cache[j];
                  }

                  _count--;

                  return item;
               }
            }
            return _map.DefaultValue();
         }
         else return _map.exclude(key, item);
      }

      void erase(Key key)
      {
         T item = exclude(key);
         if (item != _map.DefaultValue()) {
            if (FreeT)
               FreeT(item);
         }
      }

      void erase(Key key, T item)
      {
         T itemToDelete = exclude(key, item);
         if (itemToDelete != _map.DefaultValue()) {
            if (FreeT)
               FreeT(itemToDelete);
         }
      }

      void clear()
      {
         _map.clear();

         _cached = true;
         _count = 0;
      }

      template<class ArgT> Key retrieve(Key defKey, ArgT arg, bool(*lambda)(ArgT arg, Key key, T item))
      {
         auto it = start();
         while (!it.eof())
         {
            if (lambda(arg, it.key(), *it))
               return it.key();

            ++it;
         }

         return defKey;
      }

      CachedMemoryMap(T defValue)
         : _map(defValue)
      {
         _cached = true;
         _count = 0;
      }
      virtual ~CachedMemoryMap()
      {
         clear();
      }

   };

   // --- CachedList ---
   template <class T, size_t cacheSize> class CachedList
   {
      T      _cached[cacheSize];
      T*     _allocated;
      size_t _length;
      size_t _allocatedSize;

   public:
      class CachedListIterator
      {
         friend class CachedList;

         CachedList* owner;
         int         index;

         CachedListIterator(CachedList* owner)
         {
            this->owner = owner;
            this->index = 0;
         }
      public:
         bool operator ==(const CachedListIterator& it)
         {
            return index == it.index;
         }
         bool operator !=(const CachedListIterator& it)
         {
            return index != it.index;
         }

         CachedListIterator& operator =(const CachedListIterator& it)
         {
            this->owner = it.owner;
            this->index = it.index;

            return *this;
         }

         CachedListIterator& operator ++()
         {
            index++;

            return *this;
         }
         CachedListIterator operator ++(int)
         {
            CachedListIterator tmp = *this;
            ++* this;

            return tmp;
         }
         CachedListIterator& operator--()
         {
            index--;

            return *this;
         }
         CachedListIterator operator--(int)
         {
            CachedListIterator tmp = *this;
            --* this;

            return tmp;
         }

         T& operator*()
         {
            return owner->get(index);
         }

         bool eof()
         {
            return index == owner->count_int();
         }
      };

      typedef CachedListIterator Iterator;

      Iterator start()
      {
         return { this };
      }

      T& operator[](size_t index)
      {
         if (index < cacheSize) {
            return _cached[index];
         }
         else return _allocated[index - cacheSize];
      }

      size_t count() const
      {
         return _length;
      }

      pos_t count_pos() const
      {
         return (pos_t)_length;
      }

      int count_int() const
      {
         return (int)_length;
      }

      T& get(size_t index)
      {
         if (index < cacheSize) {
            return _cached[index];
         }
         else return _allocated[index - cacheSize];
      }

      bool exist(T& item)
      {
         for (size_t i = 0; i < _length; i++) {
            if (i < cacheSize && _cached[i] == item) {
               return true;
            }
            else if (i >= cacheSize && _allocated[i - cacheSize] == item) {
               return true;
            }
         }

         return false;
      }

      void add(T item)
      {
         if (_length < cacheSize) {
            _cached[_length] = item;
         }
         else {
            if (_length - cacheSize >= _allocatedSize) {
               _allocatedSize += 10;

               void* ptr = (T*)realloc(_allocated, _allocatedSize * sizeof(T));

               assert(ptr != nullptr);

               if (ptr) {
                  _allocated = (T*)ptr;
               }
            }

            _allocated[_length - cacheSize] = item;
         }

         _length++;
      }

      void insert(pos_t index, T item)
      {
         if (_length < cacheSize) {
            for (size_t i = _length; i > index; i--)
               _cached[i] = _cached[i - 1];

            _cached[index] = item;
         }
         else {
            if (_length - cacheSize >= _allocatedSize) {
               _allocatedSize += 10;

               void* ptr = realloc(_allocated, _allocatedSize * sizeof(T));

               assert(ptr != nullptr);

               if (ptr) {
                  _allocated = (T*)ptr;
               }
            }

            if (index < cacheSize) {
               for (size_t i = _length; i > cacheSize; i--)
                  _allocated[i - cacheSize] = _allocated[i - cacheSize - 1];

               _allocated[0] = _cached[cacheSize - 1];
               for (size_t i = cacheSize - 1; i > index; i--)
                  _cached[i] = _cached[i - 1];

               _cached[index] = item;
            }
            else {
               for (size_t i = _length; i > index; i--)
                  _allocated[i - cacheSize] = _allocated[i - cacheSize - 1];

               _allocated[index] = item;
            }
         }

         _length++;
      }

      void clear()
      {
         freeobj(_allocated);
         _allocated = nullptr;
         _allocatedSize = _length = 0;
      }

DISABLE_WARNING_PUSH
DISABLE_WARNING_UNINITIALIZED_FIELD

      CachedList()
      {
         _allocated = nullptr;
         _allocatedSize = _length = 0;
      }

DISABLE_WARNING_POP

      ~CachedList()
      {
         freeobj(_allocated);
      }
   };

   // --- MemoryList ---
   template <class T> class MemoryList
   {
      MemoryDump _buffer;
      T          _defValue;
      pos_t      _position;

   public:
      typedef MemoryListIterator<T> Iterator;

      Iterator start() const { return Iterator((MemoryDump*)&_buffer, _position); }

      void add(T item)
      {
         _buffer.write(_position, &item, sizeof(item));

         _position += sizeof(item);
      }

      MemoryList(T defValue)
      {
         _defValue = defValue;
         _position = 0;
      }
      ~MemoryList() = default;
   };

   // --- MemoryTrie ---
   template <class T> class MemoryTrie
   {
   private:
      T          _defValue;
      MemoryDump _buffer;

   public:
      struct Node
      {
         T     value;
         pos_t parent;
         pos_t firstChildLink;

         Node(T defValue)
         {
            value = defValue;
            parent = firstChildLink = 0;
         }
         Node(T value, pos_t parent)
         {
            this->value = value;
            this->parent = parent;
            this->firstChildLink = 0;
         }
      };

      struct ChildLink
      {
         pos_t node;
         pos_t nextChildLink;

         ChildLink()
         {
            node = nextChildLink = 0;
         }
         ChildLink(pos_t node, pos_t nextChildLink)
         {
            this->node = node;
            this->nextChildLink = nextChildLink;
         }
      };

      Node getNode(pos_t position)
      {
         Node node = { _defValue };
         if (position != INVALID_POS)
            _buffer.read(position, &node, sizeof(Node));

         return node;
      }

      pos_t getNextLink(pos_t linkPosition)
      {
         ChildLink link = {};
         _buffer.read(linkPosition, &link, sizeof(ChildLink));

         return link.nextChildLink;
      }

      pos_t getLinkNodePosition(pos_t linkPosition)
      {
         if (linkPosition != 0) {
            ChildLink link = {};
            _buffer.read(linkPosition, &link, sizeof(ChildLink));

            return link.node;
         }
         else return 0;
      }

      pos_t addRootNode(T item)
      {
         pos_t position = _buffer.length();
         Node node = { item, INVALID_POS };

         _buffer.write(position, &node, sizeof(node));

         return position;
      }

      pos_t addNode(pos_t parent, T value)
      {
         // add new node
         pos_t position = _buffer.length();
         Node node(value, parent);
         _buffer.write(position, &node, sizeof(Node));

         addChildLink(parent, position);

         return position;
      }

      void addChildLink(pos_t parent, pos_t child)
      {
         // add new child link
         pos_t linkPosition = _buffer.length();
         ChildLink link = { child, 0 };
         _buffer.write(linkPosition, &link, sizeof(ChildLink));

         // update parent
         Node parentNode = { _defValue };
         _buffer.read(parent, &parentNode, sizeof(Node));

         if (parentNode.firstChildLink == 0) {
            parentNode.firstChildLink = linkPosition;

            _buffer.write(parent, &parentNode, sizeof(Node));
         }
         else {
            ChildLink link = { 0, parentNode.firstChildLink };
            pos_t position = parentNode.firstChildLink;
            do {
               position = link.nextChildLink;

               _buffer.read(position, &link, sizeof(ChildLink));
            } while (link.nextChildLink != 0);

            link.nextChildLink = linkPosition;
            _buffer.write(position, &link, sizeof(ChildLink));
         }
      }

      void save(StreamWriter* writer)
      {
         writer->writePos(_buffer.length());

         MemoryReader reader(&_buffer);
         writer->copyFrom(&reader, _buffer.length());
      }

      void load(StreamReader* reader)
      {
         pos_t length = reader->getPos();
         _buffer.reserve(length);

         MemoryWriter writer(&_buffer);
         writer.copyFrom(reader, length);
      }

      MemoryTrie(T defValue)
      {
         _defValue = defValue;
      }
      ~MemoryTrie() = default;
   };

   // --- MemoryTrieNode ---
   template <class T> class MemoryTrieNode
   {
      pos_t          _position;
      MemoryTrie<T>* _trie;

   public:
      typedef typename MemoryTrie<T>::Node Node;

      struct ChildEnumerator
      {
         friend class MemoryTrieNode;

      private:
         MemoryTrie<T>* _trie;
         pos_t          _linkPosition;

         ChildEnumerator(MemoryTrie<T>* trie, pos_t linkPosition)
         {
            _trie = trie;
            _linkPosition = linkPosition;
         }

      public:
         bool eof()
         {
            return (_linkPosition == 0);
         }

         MemoryTrieNode Node()
         {
            pos_t position = _trie->getLinkNodePosition(_linkPosition);

            return MemoryTrieNode(_trie, position);
         }

         ChildEnumerator& operator ++()
         {
            _linkPosition = _trie->getNextLink(_linkPosition);

            return *this;
         }
         ChildEnumerator operator ++(int)
         {
            ChildEnumerator tmp = *this;
            ++* this;

            return tmp;
         }

         ChildEnumerator()
         {
            _linkPosition = 0;
         }
      };

      bool operator ==(MemoryTrieNode& node) const
      {
         return (this->_position == node._position);
      }

      bool operator !=(MemoryTrieNode& node) const
      {
         return (this->_position != node._position);
      }

      pos_t position() { return _position; }

      T Value()
      {
         Node node = _trie->getNode(_position);

         return node.value;
      }

      MemoryTrieNode FirstChild()
      {
         Node node = _trie->getNode(_position);

         pos_t childPosition = _trie->getLinkNodePosition(node.firstChildLink);
         if (childPosition) {
            return MemoryTrieNode(_trie, childPosition);
         }
         return { _trie, INVALID_POS };
      }

      ChildEnumerator Children()
      {
         Node node = _trie->getNode(_position);

         return ChildEnumerator(_trie, node.firstChildLink);
      }

      ChildEnumerator find(T value)
      {
         ChildEnumerator children = Children();
         while (!children.eof() && children.Node().Value() != value)
            ++children;

         return children;
      }

      void link(MemoryTrieNode& node)
      {
         _trie->addChildLink(_position, node.position());
      }

      MemoryTrieNode()
      {
         _trie = nullptr;
         _position = 0;
      }
      MemoryTrieNode(MemoryTrie<T>* trie)
      {
         _trie = trie;
         _position = 0;
      }
      MemoryTrieNode(MemoryTrie<T>* trie, pos_t position)
      {
         _trie = trie;
         _position = position;
      }
   };

   // --- MemoryTrieBuilder ---
   template <class T> struct MemoryTrieBuilder
   {
   public:
      MemoryTrie<T> trie;

      typedef typename MemoryTrieNode<T>::ChildEnumerator ChildEnumerator;

      void addRoot(T item)
      {
         trie.addRootNode(item);
      }

      pos_t find(pos_t position, T item)
      {
         MemoryTrieNode<T> node(&trie, position);

         auto children_it = node.Children();
         while (!children_it.eof()) {
            auto child = children_it.Node();
            if (child.Value() == item)
               return child.position();

            ++children_it;
         }

         return 0;
      }

      pos_t add(pos_t parentPosition, T item)
      {
         pos_t position = find(parentPosition, item);
         if (position == 0) {
            position = trie.addNode(parentPosition, item);
         }
         return position;
      }

      void scanTrie(MemoryTrieNode<T>& current, ChildEnumerator nodes, MemoryTrieNode<T>& failedNode, bool(*isTerminal)(T))
      {
         while (!nodes.eof()) {
            auto child = nodes.Node();
            if (!isTerminal(child.Value())/*child.Value() != terminalNode*/ && child.position() != current.position()) {
               if (current.Value() == child.Value()) {
                  auto next = current.Children();
                  while (!next.eof()) {
                     auto node = next.Node();
                     scanTrie(node, child.Children(), child, isTerminal);

                     ++next;
                  }
               }
               else if (failedNode.position() != 0) {
                  auto it = failedNode.find(current.Value());
                  if (it.eof()) {
                     failedNode.link(current);
                  }
               }

               // skip terminator
               if (/*child.Value() != terminalNode*/!isTerminal(child.Value())) // !! useless condition?
                  scanTrie(current, child.Children(), failedNode, isTerminal);
            }

            ++nodes;
         }
      }

      void prepare(bool(*isTerminal)(T))
      {
         MemoryTrieNode<T> emptyNode;
         MemoryTrieNode<T> node(&trie);

         auto children = node.Children();
         while (!children.eof()) {
            auto node = children.Node();
            scanTrie(node, node.Children(), emptyNode, isTerminal);

            ++children;
         }
      }

      void save(StreamWriter* writer)
      {
         trie.save(writer);
      }

      MemoryTrieBuilder(T defValue)
         : trie(defValue)
      {

      }
   };

   // --- shift routine ---
   template <class Iterator, class T> void shift(Iterator it, T minValue, const int displacement)
   {
      while (!it.eof()) {
         if ((*it) >= minValue)
            *it += displacement;

         it++;
      }
   }

   // --- mapKey routine ---
   template<class Map, class Key, class T> T mapKey(Map& map, Key key, T newValue)
   {
      T value = map.get(key);
      if (map.DefaultValue() == value) {
         map.add(key, newValue);

         return newValue;
      }
      else return value;
   }

   template<class Key> pos_t Map_StoreKey(MemoryDump* dump, Key key)
   {
      pos_t position = dump->length();

      dump->write(position, &key, sizeof(key));

      return position;
   }

   template<class Key> Key Map_GetKey(MemoryDump* dump, pos_t position)
   {
      Key key;
      dump->read(position, &key, sizeof(key));

      return key;
   }
}

DISABLE_WARNING_POP

#endif
