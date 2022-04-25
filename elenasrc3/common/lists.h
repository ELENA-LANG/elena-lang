//---------------------------------------------------------------------------
//              E L E N A   P r o j e c t:  ELENA Common Library
//
//              This header contains various ELENA Engine list templates
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LISTS_H
#define LISTS_H

namespace elena_lang
{
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
   template <class T, class Item> class ListIteratorBase
   {
      Item* _current;

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
   template <class T, void(*FreeT)(T) = nullptr> class ListBase
   {
      typedef ItemBase<T> Item;

      pos_t _count;
      Item* _top, * _tale;
      T     _defaultValue;

   public:
      typedef ListIteratorBase<T, Item> Iterator;

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

            if (_tale == tmp)
               _tale = previous;
            else if (previous == nullptr) {
               _top = _top->next;
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
   template <class T, void(*FreeT)(T) = nullptr> class BListBase
   {
      typedef BItemBase<T> Item;

      Item *_top, *_tale;
      pos_t _count;

   public:
      typedef ListIteratorBase<T, BItemBase<T>> Iterator;

      pos_t count() const
      {
         return _count;
      }

      Iterator start() { return Iterator(_top); }

      Iterator end() { return Iterator(_tale); }

      BListBase()
      {
         _top = _tale = nullptr;
         _count = 0;
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
   };

   // --- List ---
   template <class T, void(*FreeT)(T) = nullptr> class List
   {
      ListBase<T, FreeT> _list;
      T                  _defaultItem;

   public:
      typedef ListIteratorBase<T, ItemBase<T>> Iterator;

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

      List(T defValue)
         : _list(defValue)
      {
         _defaultItem = defValue;
      }
      virtual ~List() = default;
   };

   // --- Stack ---
   template <class T> class Stack
   {
      ListBase<T> _list;
      T           _defaultItem;

   public:
      typedef ListIteratorBase<T, ItemBase<T>> Iterator;

      Iterator start()
      {
         return _list.start();
      }

      Iterator end()
      {
         return _list.end();
      }

      T DefaultValue() const { return _list.DefaultValue(); }

      void push(T item)
      {
         _list.addToTop(item);
      }

      T peek() const
      {
         return _list.peek();
      }

      T pop()
      {
         if (_list.count() != 0) {
            return _list.cutTop();
         }
         else return _defaultItem;
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
         : _list(_defaultItem)
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
      typedef ListIteratorBase<T, BItemBase<T>> Iterator;

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
               _buffer.read(position, &value, sizeof(Key));

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
            pos_t previousOffset = -1;
            while (currentOffset) {
               Item* current = (Item*)_buffer.get(currentOffset);

               if (current->readKey(&_buffer) == key) {
                  if (previousOffset == -1) {
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
         _tale = 0;
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
   template <class Key, class T, size_t(_scaleKey)(Key), size_t hashSize, Key(*AllocKey)(Key) = nullptr, void(*FreeKey)(Key) = nullptr, void(*FreeT)(T) = nullptr> class HashTable
   {
      typedef MapItemBase<Key, T, AllocKey, FreeKey> Item;

      class HashTableIterator
      {
         friend class HashTable;

         Item*            _current;
         size_t           _hashIndex;
         const HashTable* _hashTable;

         HashTableIterator(const HashTable* hashTable, size_t hashIndex, Item* current)
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
         size_t index = _scaleKey(key);
         if (index > hashSize)
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
         size_t index = _scaleKey(key);
         if (index > hashSize)
            index = hashSize - 1;

         Item* current = _table[index];
         while (current && (current->key < key))
            current = current->next;

         if (current && (current->key != key)) {
            return Iterator(this, hashSize, NULL);
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
      };

      class CachedMemoryMapIterator
      {
         friend class CachedMemoryMap;

         CachedMemoryMap* _cachedMap;
         size_t           _index;
         MemoryIterator   _iterator;

         CachedMemoryMapIterator(CachedMemoryMap* cachedMap, unsigned int index)
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
         }
      };
      friend class CachedMemoryMapIterator;

      MemoryMap<Key, T, StoreKey, GetKey, FreeT> _map;

      bool                                       _cached;
      CachedItem                                 _cache[cacheSize];
      size_t                                     _count;

   public:
      typedef CachedMemoryMapIterator Iterator;

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

                  for (unsigned int j = i + 1; j < _count; j++) {
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

      CachedMemoryMap(T defValue)
         : _map(defValue)
      {
         _cached = true;
         _count = 0;
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

      void add(T item)
      {
         if (_length < cacheSize) {
            _cached[_length] = item;
         }
         else {
            if (_length - cacheSize >= _allocatedSize) {
               _allocatedSize += 10;

               _allocated = (T*)realloc(_allocated, _allocatedSize * sizeof(T));
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

               _allocated = (T*)realloc(_allocated, _allocatedSize * sizeof(T));
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

      CachedList()
      {
         _allocated = nullptr;
         _allocatedSize = _length = 0;
      }
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

#endif
