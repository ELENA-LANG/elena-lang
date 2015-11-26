//---------------------------------------------------------------------------
//              E L E N A   P r o j e c t:  ELENA Common Library
//
//              This header contains various ELENA Engine list templates
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef listsH
#define listsH 1

namespace _ELENA_
{

// --- Pair template ---

template <class T1, class T2> struct Pair
{
   T1 value1;
   T2 value2;

   Pair()
   {
      this->value1 = 0;
      this->value2 = 0;
   }
   Pair(T1 value1, T2 value2)
   {
      this->value1 = value1;
      this->value2 = value2;
   }
};


// --- template forward declarations ---
template <class T> class _BList;
template <class T> class _List;

// --- Item template ---

template <class T> class _Item
{
public:
   T      item;
   _Item* next;

   _Item(T item, _Item* next)
   {
      this->item = item;
      this->next = next;
   }
};

// --- BItem structure ---

template <class T> class _BItem
{
public:
   T       item;
   _BItem* previous;
   _BItem* next;

   _BItem(T item, _BItem* previous, _BItem* next)
   {
      this->item = item;
      this->previous = previous;
      this->next = next;
   }
};

// --- MapItem structure ---

template <class Key, class T, bool KeyStored = true> struct _MapItem
{
   Key       key;
   T         item;
   _MapItem* next;

   bool operator ==(int key) const
   {
      return (this->key == key);
   }

   bool operator !=(int key) const
   {
      return (this->key != key);
   }

   bool operator <=(int key) const
   {
      return (this->key <= key);
   }

   bool operator <(int key) const
   {
      return (this->key < key);
   }

   bool operator >=(int key) const
   {
      return (this->key >= key);
   }

   bool operator >(int key) const
   {
      return (this->key > key);
   }

   bool operator ==(size_t key) const
   {
      return (this->key == key);
   }

   bool operator !=(size_t key) const
   {
      return (this->key != key);
   }

   bool operator <=(size_t key) const
   {
      return (this->key <= key);
   }

   bool operator <(size_t key) const
   {
      return (this->key < key);
   }

   bool operator >=(size_t key) const
   {
      return (this->key >= key);
   }

   bool operator >(size_t key) const
   {
      return (this->key > key);
   }

   bool operator ==(const wide_c* key) const
   {
      return StringHelper::compare(this->key, key);
   }

   bool operator !=(const wide_c* key) const
   {
      return !StringHelper::compare(this->key, key);
   }

   bool operator <=(const wide_c* key) const
   {
      return !StringHelper::greater(this->key, key);
   }

   bool operator <(const wide_c* key) const
   {
      return StringHelper::greater(key, this->key);
   }

   bool operator >=(const wide_c* key) const
   {
      return StringHelper::greater(this->key, key) || compstr(this->key, key);
   }

   bool operator >(const wide_c* key) const
   {
      return StringHelper::greater(this->key, key);
   }

   bool operator ==(const char* key) const
   {
      return StringHelper::compare(this->key, key);
   }

   bool operator !=(const char* key) const
   {
      return !StringHelper::compare(this->key, key);
   }

   bool operator <=(const char* key) const
   {
      return !StringHelper::greater(this->key, key);
   }

   bool operator <(const char* key) const
   {
      return StringHelper::greater(key, this->key);
   }

   bool operator >=(const char* key) const
   {
      return StringHelper::greater(this->key, key) || compstr(this->key, key);
   }

   bool operator >(const char* key) const
   {
      return StringHelper::greater(this->key, key);
   }

//   void rename(const wchar16_t* key)
//   {
//      if (KeyStored) {
//         freeKey(this->key);
//
//         this->key = StringHelper::clone(key);
//      }
//      else this->key = key;
//   }
//
//   void rename(const char* key)
//   {
//      if (KeyStored) {
//         freeKey(this->key);
//
//         this->key = StringHelper::clone(key);
//      }
//      else this->key = key;
//   }
//
//   void freeKey(int key) { key = 0; }
   void freeKey(size_t key) { key = 0; }
   void freeKey(const wide_c* key) { freestr((wide_c*)key); }
   void freeKey(const char* key) { freestr((char*)key); }

//   _MapItem(int key, T item, _MapItem* next)
//   {
//      this->key = key;
//      this->item = item;
//      this->next = next;
//   }
   _MapItem(size_t key, T item, _MapItem* next)
   {
      this->key = key;
      this->item = item;
      this->next = next;
   }
   _MapItem(const wide_c* key, T item, _MapItem* next)
   {
      if (KeyStored) {
         this->key = StringHelper::clone(key);
      }
      else this->key = key;

      this->item = item;
      this->next = next;
   }
   _MapItem(const char* key, T item, _MapItem* next)
   {
      if (KeyStored) {
         this->key = StringHelper::clone(key);
      }
      else this->key = key;

      this->item = item;
      this->next = next;
   }
   ~_MapItem()
   {
      if (KeyStored) {
         freeKey(this->key);
      }
   }
};

// --- MemoryMapItem structure ---

template <class Key, class T, bool KeyStored> struct _MemoryMapItem
{
   size_t next;      // offset from the memory dump begining
   Key    key;       // for Key=TCHAR* if keyStored is true, key is an offset in the map buffer
   T      item;

   int getKey(int key) const
   {
      return key;
   }

   bool operator ==(int key) const
   {
      return (this->key == key);
   }

   bool operator !=(int key) const
   {
      return (this->key != key);
   }

   bool operator <=(int key) const
   {
      return (this->key <= key);
   }

   bool operator <(int key) const
   {
      return (this->key < key);
   }

   bool operator >=(int key) const
   {
      return (this->key >= key);
   }

   bool operator >(int key) const
   {
      return (this->key > key);
   }

   size_t getKey(size_t key) const
   {
      return key;
   }

   bool operator ==(size_t key) const
   {
      return (this->key == key);
   }

   bool operator !=(size_t key) const
   {
      return (this->key != key);
   }

   bool operator <=(size_t key) const
   {
      return (this->key <= key);
   }

   bool operator <(size_t key) const
   {
      return (this->key < key);
   }

   bool operator >=(size_t key) const
   {
      return (this->key >= key);
   }

   bool operator >(size_t key) const
   {
      return (this->key > key);
   }

   Pair<size_t, int> getKey(Pair<size_t, int> key) const
   {
      return key;
   }

   bool operator ==(Pair<size_t, int> key) const
   {
      return (this->key.value1 == key.value1 && this->key.value2 == key.value2);
   }

   bool operator !=(Pair<size_t, int> key) const
   {
      return (this->key.value1 != key.value1 || this->key.value2 != key.value2);
   }

   bool operator <=(Pair<size_t, int> key) const
   {
      return (this->key.value1 <= key.value1);
   }

   bool operator <(Pair<size_t, int> key) const
   {
      return (this->key.value1 < key.value1);
   }

   bool operator >=(Pair<size_t, int> key) const
   {
      return (this->key.value1 >= key.value1);
   }

   bool operator >(Pair<size_t, int> key) const
   {
      return (this->key.value1 > key.value1);
   }

   ident_t getKey(ident_t key) const
   {
      if (KeyStored) {
         return (ident_c*)((int)this + (int)this->key);
      }
      else return key;
   }

   bool operator ==(ident_t key) const
   {
      if (KeyStored) {
         return StringHelper::compare((ident_c*)((int)this + (int)this->key), key);
      }
      else return StringHelper::compare(this->key, key);
   }

   bool operator !=(ident_t key) const
   {
      if (KeyStored) {
         return !StringHelper::compare((ident_c*)((int)this + (int)this->key), key);
      }
      else return !StringHelper::compare(this->key, key);
   }

   bool operator <=(ident_t key) const
   {
      if (KeyStored) {
         return !StringHelper::greater((ident_c*)((int)this + (int)this->key), key);
      }
      else return !StringHelper::greater(this->key, key);
   }

   bool operator <(ident_t key) const
   {
      if (KeyStored) {
         return StringHelper::greater(key, (ident_c*)((int)this + (int)this->key));
      }
      else return StringHelper::greater(key, this->key);
   }

   bool operator >=(ident_t key) const
   {
      return (this > key || this == key);
   }

   bool operator >(ident_t key) const
   {
      if (KeyStored) {
         return StringHelper::greater((ident_c*)((int)this + (int)this->key), key);
      }
      else return StringHelper::greater(this->key, key);
   }

   _MemoryMapItem()
   {
      next = 0;
   }
   _MemoryMapItem(Key key, T item, size_t next)
   {
      this->key = key;
      this->item = item;
      this->next = next;
   }
};

//struct _IntItem
//{
//   size_t key;
//   size_t value;
//};

// --- Iterator template ---

template <class T, class Item, class Key = int> class _Iterator
{
   friend class _BList<T>;
   friend class _List<T>;

   Item* _current;

public:
   _Iterator& operator =(const _Iterator& it)
   {
      this->_current = it._current;

      return *this;
   }

   bool operator ==(const _Iterator& it)
   {
      return (this->_current == it._current);
   }

   bool operator !=(const _Iterator& it)
   {
      return (this->_current != it._current);
   }

   _Iterator& operator ++()
   {
      _current = _current->next;

      return *this;
   }
   _Iterator operator ++(int)
   {
      _Iterator tmp = *this;
      ++*this;

      return tmp;
   }

   _Iterator& operator--()
   {
      _current = _current->previous;

      return *this;
   }
   _Iterator operator--(int)
   {
      _Iterator tmp = *this;
      --*this;

      return tmp;
   }

   bool Eof() const { return _current == NULL; }

   bool First() const { return (_current->previous == NULL); }

   bool Last() const { return (_current->next == NULL); }

   T& operator*() const { return _current->item; }

   Key key() const { return _current->key; }

   Item* _item() const { return _current; }

//   void rename(Key key)
//   {
//      _current->rename(key);
//   }

   _Iterator(Item* current)
   {
      _current = current;
   }

   _Iterator()
   {
      this->_current = NULL;
   }
};

// --- MemoryIterator template ---

template <class T, class Item, class Map, class Key = int> class _MemoryIterator
{
protected:
   const Map* _map;
   size_t     _position;
   Item*      _current;

public:
   _MemoryIterator& operator =(const _MemoryIterator& it)
   {
      this->_current = it._current;
      this->_position = it._position;
      this->_map = it._map;

      return *this;
   }

   _MemoryIterator& operator++()
   {
      if (_current->next!=0) {
         _position = _current->next;

         if (_position != 0) {
            _current = (Item*)(_map->getAddress(_position));
         }
         else _current = NULL;
      }
      else _current = NULL;

      return *this;
   }

   _MemoryIterator operator++(int)
   {
      _MemoryIterator tmp = *this;
      ++*this;

      return tmp;
   }

   T& operator*() const { return _current->item; }

   Key key() const
   {
      return _current->getKey(_current->key);
   }

   bool Eof() const { return (_current == NULL); }

   size_t Position() const { return _position; }

   _MemoryIterator(const Map* map, size_t position, Item* current)
   {
      _map = map;
      _position = position;
      _current = current;
   }

   _MemoryIterator()
   {
      _current = NULL;
   }
};

// --- Base list template ---

template <class T> class _List
{
   typedef _Item<T> Item;

   size_t _count;
   Item  *_top, *_tale;

   void  (*_freeT)(T);

public:
   typedef _Iterator<T, Item> Iterator;

   bool Eof() const { return _top==NULL; }

   size_t Count() const { return _count; }

   Iterator start() { return Iterator(_top); }

   Iterator end() { return Iterator(_tale); }

   void set(Iterator& it, T item)
   {
      if (_freeT)
         _freeT(it._current->item);

      it._current->item = item;
   }

   void insertBefore(Iterator& it, T item)
   {
      it._current->next = new Item(it._current->item, it._current->next);
      it._current->item = item;

      _count++;
      it++;
   }

   void insertAfter(Iterator& it, T item)
   {
      it._current->next = new Item(item, it._current->next);

      _count++;
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
      if (_tale!=NULL) {
         _tale->next = new Item(item, NULL);
         _tale = _tale->next;
      }
      else _top = _tale = new Item(item, NULL);

      _count++;
   }

   T peek()
   {
      return _top->item;
   }

   T cutTop()
   {
      Item* tmp = _top;
      _top = _top->next;
      _count--;

      if (tmp==_tale)
         _tale = NULL;

      T item = tmp->item;
      freeobj(tmp);

      return item;
   }

   void cut(T item)
   {
      Item* tmp = NULL;
      Item* previous = NULL;

      if (_top->item==item)
         tmp = _top;
      else {
         previous = _top;
         while (previous->next) {
            if (previous->next->item==item) {
               tmp = previous->next;
               break;
            }
            previous = previous->next;
         }
      }
      if (tmp) {
         _count--;

         if (tmp==_tale)
            _tale = previous;
         if (previous==NULL) {
            _top = _top->next;
         }
         else previous->next = tmp->next;

         if (_freeT)
            _freeT(tmp->item);

         delete tmp;
      }
   }

   void cut(Iterator it)
   {
      Item* tmp = NULL;
      Item* previous = NULL;

      if (_top==it._current)
         tmp = _top;
      else {
         previous = _top;
         while (previous->next) {
            if (previous->next==it._current) {
               tmp = previous->next;
               break;
            }
            previous = previous->next;
         }
      }
      if (tmp) {
         _count--;

         if (tmp==_tale)
            _tale = previous;
         if (previous==NULL) {
            _top = _top->next;
         }
         else previous->next = tmp->next;

         if (_freeT)
            _freeT(tmp->item);

         delete tmp;
      }
   }

   void clear()
   {
      while (_top) {
         Item* tmp = _top;
         _top = _top->next;

         if (_freeT)
            _freeT(tmp->item);

         delete tmp;
      }
      _count = 0;
      _top = _tale = NULL;
   }

   _List(void(*freeT)(T))
   {
      _top = _tale = NULL;
      _count = 0;
      _freeT = freeT;
   }

   ~_List()
   {
      clear();
   }
};

// --- Base BList template ---

template <class T> class _BList
{
   typedef _BItem<T>    Item;

   size_t _count;
   Item *_top, *_tale;

   void (*_freeT)(T);

public:
   typedef _Iterator<T, Item> Iterator;

   size_t Count() const { return _count; }

   Iterator start() { return Iterator(_top); }

   Iterator end() { return Iterator(_tale); }

   void set(Iterator& it, T item)
   {
      if (_freeT)
         _freeT(it.current->item);

      it.current->item = item;
   }

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

   void addToTale(T item)
   {
      if (_top != NULL) {
         _tale->next = new Item(item, _tale, NULL);
         _tale = _tale->next;
      }
      else _top = _tale = new Item(item, NULL, NULL);

      _count++;
   }

   void circle(T item)
   {
      if (_top != NULL) {
         _top->previous = new Item(item, _top->previous, _top);
         _tale->next = _top->previous;

         _tale = _top->previous;
      }
      else {
         _top = _tale = new Item(item, NULL, NULL);
         _top->next = _top->previous = _top;
      }
      _count++;
   }

   void shiftNext()
   {
      if (_top) {
         _top = _top->next;
         _tale = _top->previous;
      }
   }

   void shiftPrevious()
   {
      if (_top) {
         _top = _top->previous;
         _tale = _top->previous;
      }
   }

   T cut(T item)
   {
      Iterator it = start();
      for (int i = 0 ; i < _count ; i++) {
         if (*it==item) {
            return cut(it);
         }
         it++;
      }
   }

   T cut(Iterator& it)
   {
      Item* tmp = it._current;
      it++;

      if (tmp==_top) {
         Item* previous = _top->previous;

         _top = _top->next;
         _top->previous = previous;
         previous->next = _top;
         _tale = _top->previous;
         if (_count==1) {
            _top = _tale = NULL;
         }
      }
      else {
         if (tmp->next) {
            tmp->next->previous = tmp->previous;
            tmp->previous->next = tmp->next;
         }
         else _tale = tmp->previous;

         if (tmp->previous) {
            tmp->previous->next = tmp->next;
            if (tmp->next)
               tmp->next->previous = tmp->previous;
         }
         else _top = tmp->next;
      }
      T item = tmp->item;
      freeobj(tmp);
      _count--;

      return item;
   }

   void clear()
   {
      while (_count > 0) {
         Item* tmp = _top;
         _top = _top->next;

         if (_freeT)
            _freeT(tmp->item);

         delete tmp;
         _count--;
      }
      _top = _tale = NULL;
   }

   _BList(void(*freeT)(T))
   {
      _top = _tale = NULL;
      _count = 0;
      _freeT = freeT;
   }
   ~_BList()
   {
      clear();
   }
};

// --- List template ---

template <class T> class List
{
   _List<T> _list;

public:
   typedef _Iterator<T, _Item<T> >    Iterator;

   size_t Count() const { return _list.Count(); }

   Iterator start()
   {
      return _list.start();
   }

   Iterator end()
   {
      return _list.end();
   }

   void set(Iterator& it, T item)
   {
      _list.set(it, item);
   }


   void insert(T item)
   {
      _list.addToTop(item);
   }

   void insertBefore(Iterator& it, T item)
   {
      if (it.Eof()) {
         add(item);
      }
      else _list.insertBefore(it, item);
   }

   void insertAfter(Iterator it, T item)
   {
      if (it.Eof()) {
         add(item);
      }
      else _list.insertAfter(it, item);
   }

   void add(T item)
   {
      _list.addToTale(item);
   }

   void cut(T item)
   {
      _list.cut(item);
   }

   void cut(Iterator it)
   {
      _list.cut(it);
   }

   Iterator get(int index)
   {
      Iterator it = start();
      while (!it.Eof() && index > 0) {
         index--;
         it++;
      }
      return it;
   }

   void clear()
   {
      _list.clear();
   }

   List()
      : _list(NULL)
   {
   }
   List(T, void(*freeT)(T))
      : _list(freeT)
   {
   }
};

// --- Stack template ---

template <class T> class Stack
{
   _List<T> _list;
   T        _defaultItem;

public:
   typedef _Iterator<T, _Item<T> >    Iterator;

   Iterator start()
   {
      return _list.start();
   }

   Iterator end()
   {
      return _list.end();
   }

   Iterator get(int index)
   {
      Iterator it = start();
      while (!it.Eof() && index > 0) {
         index--;
         it++;
      }
      return it;
   }

   size_t Count() const { return _list.Count(); }

   T peek()
   {
      if (!_list.Eof()) {
         return _list.peek();
      }
      else return _defaultItem;
   }

   void push (T item)
   {
      _list.addToTop(item);
   }

   T pop()
   {
      if (!_list.Eof()) {
         return _list.cutTop();
      }
      else return _defaultItem;
   }

   void cut(Iterator it)
   {
      _list.cut(it);
   }

   void insert(Iterator it, T item)
   {
      if (it.Eof()) {
         _list.addToTale(item);
      }
      else _list.insertAfter(it, item);
   }

   void clear()
   {
      _list.clear();
   }

   Stack()
      : _list(NULL)
   {
      this->_defaultItem = NULL;
   }
   Stack(T defaultItem)
      : _list(NULL)
   {
      _defaultItem = defaultItem;
   }
   ~Stack()
   {
   }
};

//// --- Queue template ---
//
//template <class T> class Queue
//{
//   _List<T> _list;
//   T        _defaultItem;
//
//public:
//   typedef _Iterator<T, _Item<T> >    Iterator;
//
//   Iterator start()
//   {
//      return _list.start();
//   }
//
//   Iterator end()
//   {
//      return _list.end();
//   }
//
//   size_t Count() const { return _list.Count(); }
//
//   void push (T item)
//   {
//      _list.addToTale(item);
//   }
//
//   T pop()
//   {
//      if (!_list.Eof()) {
//         return _list.cutTop();
//      }
//      else return _defaultItem;
//   }
//
//   Iterator get(int index)
//   {
//      Iterator it = start();
//      while (!it.Eof() && index > 0) {
//         index--;
//         it++;
//      }
//      return it;
//   }
//
//   void cut()
//   {
//      _list.clear();
//   }
//
//   void clear()
//   {
//      _list.clear();
//   }
//
//   Queue()
//      : _list(NULL)
//   {
//      _defaultItem = 0;
//   }
//   Queue(T defaultItem)
//      : _list(NULL)
//   {
//      _defaultItem = defaultItem;
//   }
//   Queue(T defaultItem, void(*freeT)(T))
//      : _list(freeT)
//   {
//      defaultItem = defaultItem;
//   }
//};

// --- BList template ---

template <class T> class BList
{
   _BList<T>    _list;

public:
   typedef _Iterator<T, _BItem<T> >     Iterator;

   size_t Count() const { return _list.Count(); }

   Iterator start() { return _list.start(); }

   Iterator end() { return _list.end(); }

   void set(Iterator& it, T item)
   {
      _list.set(it, item);
   }

   void insertAfter(Iterator it, T item)
   {
      if (it.Eof()) {
         add(item);
      }
      else _list.insertAfter(it, item);
   }

   void insertBefore(Iterator it, T item)
   {
      if (it.Eof()) {
         add(item);
      }
      else _list.insertBefore(it, item);
   }

   void cut(Iterator& it)
   {
      _list.cut(it);
   }

   void add(T item)
   {
      _list.addToTale(item);
   }

   void clear()
   {
      _list.clear();
   }

   BList()
      : _list(NULL)
   {
   }
   BList(T defaultItem, void(*freeT)(T))
      : _list(freeT)
   {
   }
};

// --- CList template ---

template <class T> class CList
{
   _BList<T> _list;

public:
   typedef _Iterator<T, _BItem<T> >     Iterator;

   size_t Count() const { return _list.Count(); }

   Iterator start() { return _list.start(); }

   Iterator end() { return _list.end(); }

   void shiftNext()
   {
      _list.shiftNext();
   }

   void shiftPrevious()
   {
      _list.shiftPrevious();
   }

   void set(Iterator& it, T item)
   {
      _list.set(it, item);
   }

   void insertAfter(Iterator it, T item)
   {
      if (it.Eof()) {
         add(item);
      }
      else _list.insertAfter(it, item);
   }

   void insertBefore(Iterator it, T item)
   {
      if (it.Eof()) {
         add(item);
      }
      else _list.insertBefore(it, item);
   }

   void add(T item)
   {
      _list.circle(item);
   }

   T cut(T item)
   {
      return _list.cut(item);
   }

   T cut(Iterator& it)
   {
      return _list.cut(it);
   }

   void clear()
   {
      _list.clear();
   }

   CList()
      : _list(NULL)
   {
   }
   CList(T defaultItem, void(*freeT)(T))
      : _list(freeT)
   {
   }
};

// --- Map template ---

template <class Key, class T, bool KeyStored = true> class Map
{
   typedef _MapItem<Key, T, KeyStored> Item;

   Item *_top, *_tale;
   size_t _count;

   T    _defaultItem;
   void (*_freeT)(T);

public:
   typedef _Iterator<T, Item, Key> Iterator;

   T DefaultValue() const { return _defaultItem; }

   size_t Count() const { return _count; }

   Iterator start() const
   {
      return Iterator(_top);
   }
   Iterator end() const
   {
      return Iterator(_tale);
   }

   Iterator getIt(Key key) const
   {
      Item* current = _top;
      while (current) {
         if (*current == key)
            return Iterator(current);

         current = current->next;
      }
      return Iterator();
   }

   T get(Key key) const
   {
      Item* current = _top;
      while (current) {
         if (*current == key)
            return current->item;

         current = current->next;
      }
      return _defaultItem;
   }

   bool exist(Key key) const
   {
      Item* current = _top;
      while (current) {
         if (*current == key)
            return true;

         current = current->next;
      }
      return false;
   }

   bool exist(Key key, T item) const
   {
      Item* current = _top;
      while (current) {
         if (*current == key && current->item == item)
            return true;

         current = current->next;
      }
      return false;
   }

   void addToTop(Key key, T item)
   {
      if (_top != NULL) {
         _top = new Item(key, item, _top);
      }
      else _top = _tale = new Item(key, item, NULL);

      _count++;
   }

   void add(Key key, T item)
   {
      if (_tale != NULL) {
         _tale->next = new Item(key, item, NULL);
         _tale = _tale->next;
      }
      else _top = _tale = new Item(key, item, NULL);

      _count++;
   }

   bool add(Key key, T item, const bool unique)
   {
      if (!unique || !exist(key)) {
         add(key, item);

         return true;
      }
      else return false;
   }

// !! temporal
   //void add(Map<Key, T>& map)
   //{
   //   Iterator it = map.start();
   //   while (!it.Eof()) {
   //      add(it.key(), *it);

   //      it++;
   //   }
   //}

   T exclude(Key key)
   {
      Item* tmp = NULL;
      if (!_top);
      else if (*_top==key) {
         tmp = _top;
         if (_top==_tale)
            _tale = NULL;
         _top = _top->next;
      }
      else {
         Item* cur = _top;
         while (cur->next) {
            if (*cur->next == key) {
               if (cur->next==_tale)
                  _tale = cur;

               tmp = cur->next;
               cur->next = tmp->next;
               break;
            }
            cur = cur->next;
         }
      }
      if (tmp) {
         T item = tmp->item;

         delete tmp;

         _count--;
         return item;
      }
     else return _defaultItem;
   }

   void erase(Key key)
   {
      Item* tmp = NULL;
      if (!_top);
      else if (*_top==key) {
         tmp = _top;
         if (_top==_tale)
            _tale = NULL;
         _top = _top->next;
      }
      else {
         Item* cur = _top;
         while (cur->next) {
            if (*cur->next == key) {
               if (cur->next==_tale)
                  _tale = cur;

               tmp = cur->next;
               cur->next = tmp->next;
               break;
            }
            cur = cur->next;
         }
      }
      if (tmp) {
         if (_freeT)
            _freeT(tmp->item);

         delete tmp;

         _count--;
      }
   }

   void erase(Iterator it)
   {
      if (_top==it._item()) {
         if (_top==_tale)
            _tale = NULL;
         _top = _top->next;
      }
      else {
         Item* cur = _top;
         while (cur->next) {
            if (cur->next == it._item()) {
               if (cur->next==_tale)
                  _tale = cur;

               cur->next = cur->next->next;
               break;
            }
            cur = cur->next;
         }
      }
      if (_freeT)
         _freeT(*it);

      freeobj(it._item());

      _count--;
   }

//   void write(StreamWriter* writer)
//   {
//      writer->writeDWord(_count);
//      Iterator it = start();
//      while (!it.Eof()) {
//         _writeIterator(writer, it.key(), *it);
//
//         it++;
//      }
//   }
//
//   void read(StreamReader* reader)
//   {
//      Key key;
//      T   value = _defaultItem;
//
//      size_t counter = 0;
//      reader->readDWord(counter);
//      _readToMap(reader, this, counter, key, value);
//   }

   void clear()
   {
      while (_top) {
         Item* tmp = _top;
         _top = _top->next;

         if (_freeT)
            _freeT(tmp->item);

         delete tmp;
      }
      _count = 0;
      _top = _tale = NULL;
   }

   Map()
   {
      _top = _tale = NULL;
      _count = 0;
      _defaultItem = 0;
      _freeT = NULL;
   }
   Map(T defaultItem)
   {
      _top = _tale = NULL;
      _count = 0;
      _defaultItem = defaultItem;
      _freeT = NULL;
   }
   Map(T defaultItem, void(*freeT)(T))
   {
      _top = _tale = NULL;
      _count = 0;
      _defaultItem = defaultItem;
      _freeT = freeT;
   }
   ~Map() { clear(); }
};

// --- MemoryDump template ---

template <class Key, class T, bool KeyStored = true> class MemoryMap
{
   typedef _MemoryMapItem<Key, T, KeyStored> Item;

   //friend class _MemoryIterator<T, Item, Key, KeyStored>;

   MemoryDump _buffer;

   size_t _tale;
   size_t _count;

   T      _defaultItem;

public:
   typedef _MemoryIterator<T, Item, MemoryMap<Key, T, KeyStored>, Key> Iterator;

   T DefaultValue() const { return _defaultItem; }

   size_t Count() const { return _count; }

   Iterator start() const
   {
      if (_count == 0) {
         return Iterator();
      }
      else {
         size_t position = _buffer[0];

         return Iterator(this, position, (Item*)_buffer.get(position));
      }
   }

   void* getAddress(size_t position) const
   {
      return _buffer.get(position);
   }

   Iterator getIt(Key key) const
   {
      size_t beginning = (size_t)_buffer.get(0);

      if (_buffer.Length() > 0) {
         // get top item position
         size_t position = _buffer[0];
         Item* current = NULL;
         while (position != 0) {
            current = (Item*)(beginning + position);

            if (*current == key)
               return Iterator(this, position, current);

            // offset is used instead of pointer due to possible buffer relocation
            position = current->next;
         }
      }
      return Iterator();
   }

   Iterator getNextIt(Key key, Iterator it) const
   {
      it++;

      size_t beginning = (size_t)_buffer.get(0);

      if (!it.Eof()) {
         // get top item position
         size_t position = it.Position();
         Item* current = NULL;
         while (position != 0) {
            current = (Item*)(beginning + position);

            if (*current == key)
               return Iterator(this, position, current);

            // offset is used instead of pointer due to possible buffer relocation
            position = current->next;
         }
      }
      return Iterator();
   }

   T get(Key key) const
   {
      size_t beginning = (size_t)_buffer.get(0);

      if (_buffer.Length() > 0) {
         // get top item position
         size_t position = _buffer[0];
         Item* current = NULL;
         while (position != 0) {
            current = (Item*)(beginning + position);

            if (*current == key)
               return current->item;

            // offset is used instead of pointer due to possible buffer relocation
            position = current->next;
         }
      }
      return _defaultItem;
   }

   T exclude(Key key)
   {
      size_t beginning = (size_t)_buffer.get(0);
      size_t previous = (size_t)-1;

      if (_buffer.Length() > 0) {
         // get top item position
         size_t position = _buffer[0];
         Item* current = NULL;
         while (position != 0) {
            current = (Item*)(beginning + position);

            if (*current == key) {
               // if it is top item
               if (previous == -1) {
                  _buffer[0] = current->next;

                  if (_count == 1)
                     _tale = 0;
               }
               else {
                  Item* prevItem = (Item*)(beginning + previous);
                  prevItem->next = current->next;

                  if (_tale == position)
                     _tale = previous;
               }

               _count--;

               return current->item;
            }

            // offset is used instead of pointer due to possible buffer relocation
            previous = position;
            position = current->next;
         }
      }
      return _defaultItem;
   }

   bool exist(Key key) const
   {
      return !getIt(key).Eof();
   }

   bool exist(Key key, T item) const
   {
      size_t beginning = (size_t)_buffer.get(0);

      if (_buffer.Length() > 0) {
         // get top item position
         size_t position = _buffer[0];
         Item* current = NULL;
         while (position != 0) {
            current = (Item*)(beginning + position);

            if (*current == key && current->item == item)
               return true;

            // offset is used instead of pointer due to possible buffer relocation
            position = current->next;
         }
      }
      return false;
   }

   size_t storeKey(size_t, size_t key)
   {
      return key;
   }

   ref_t storeKey(size_t position, Pair<size_t, int>)
   {
      return position;
   }

   ident_t storeKey(size_t position, ident_t key)
   {
      if (KeyStored) {
         size_t keyPos = _buffer.Length();

         _buffer.writeLiteral(keyPos, key);

         return (ident_t)(keyPos - position);
      }
      else return key;
   }

   void add(Key key, T value)
   {
      Item item(key, value, 0);

      int position = _buffer.Length();
      if (_tale == 0) {
         // save top
         if (position == 0) {
            _buffer.writeDWord(0, 4);
            position = 4;
         }
         else _buffer[0] = position;
      }

      _buffer.write(position, &item, sizeof(item));

      if (KeyStored) {
         // save stored key
         ref_t storedKey = (ref_t)storeKey(position, key);
         _buffer.writeDWord(position + 4, storedKey);
      }

      size_t beginning = (size_t)_buffer.get(0);

      if (_tale != 0) {
         Item* tale = (Item*)(beginning + _tale);

         // offset is used instead of pointer due to possible buffer relocation
         tale->next = position;
      }
      _tale = position;

      _count++;
   }

   bool add(Key key, T item, const bool unique)
   {
      if (!unique || !exist(key)) {
         add(key, item);

         return true;
      }
      else return false;
   }

   void add(MemoryMap<Key, T, KeyStored>& map)
   {
      Iterator it = map.start();
      while (!it.Eof()) {
         add(it.key(), *it);

         it++;
	  }
   }

   void write(StreamWriter* writer)
   {
      writer->writeDWord(_buffer.Length());
      writer->writeDWord(_count);
      writer->writeDWord(_tale);

      MemoryReader reader(&_buffer);
      writer->read(&reader, _buffer.Length());
   }

   void read(StreamReader* reader)
   {
      int length = reader->getDWord();
      _buffer.reserve(length);

      _count = reader->getDWord();
      _tale = reader->getDWord();

      MemoryWriter writer(&_buffer);
      writer.read(reader, length);
   }

   void clear()
   {
      _buffer.clear();

      _count = 0;
      _tale = 0;
   }

   MemoryMap()
      : _buffer(0)
   {
      _count = 0;
      _tale = 0;
      _defaultItem = 0;
   }
   MemoryMap(T defaultItem)
      : _buffer(0)
   {
      _count = 0;
      _tale = 0;
      _defaultItem = defaultItem;
   }
   ~MemoryMap() { clear(); }
};

// --- CachedMemoryDump template ---

template <class Key, class T, int cacheSize> class CachedMemoryMap
{
   typedef _MemoryMapItem<Key, T, false> Item;

private:
   typedef _MemoryIterator<T, Item, MemoryMap<Key, T, false>, Key> MemoryIterator;

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
         _cachedMap = NULL;
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
         ++*this;

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

      bool Eof() const
      {
         if (_cachedMap && _cachedMap->_cached) {
            return _index >= _cachedMap->_count;
         }
         else return _iterator.Eof();
      }

      CachedMemoryMapIterator()
      {
         this->_cachedMap = NULL;
      }
   };
   friend class CachedMemoryMapIterator;

   MemoryMap<Key, T, false> _map;

   bool   _cached;
   Item   _cache[cacheSize];
   size_t _count;

public:
   typedef CachedMemoryMapIterator Iterator;

   T DefaultValue() const { return _map.DefaultValue(); }

   size_t Count() const { return _cached ? _count : _map.Count(); }

   Iterator start()
   {
      if (_cached) {
         return Iterator(this, 0);
      }
      else return Iterator(_map.start());
   }

   Iterator getIt(Key key)
   {
      if (_cached) {
         for (size_t i = 0 ; i < _count ; i++) {
            if (_cache[i].key == key)
               return Iterator(this, i);
         }
         return Iterator(this, _count + 1);
      }
      else return Iterator(_map.getIt(key));
   }

   T get(Key key) const
   {
      if (_cached) {
         for (size_t i = 0 ; i < _count ; i++) {
            if (_cache[i].key == key)
               return _cache[i].item;
         }

         return _map.DefaultValue();
      }
      else return _map.get(key);
   }

   bool exist(Key key)
   {
      return !getIt(key).Eof();
   }

   bool exist(Key key, T item) const
   {
      if (_cached) {
         for (size_t i = 0 ; i < _count ; i++) {
            if (_cache[i].key == key)
            if (_cache[i].key == key && _cache[i].item == item)
               return true;
         }
      }
      else _map.exist(key, item);

      return false;
   }

   void add(Key key, T value)
   {
      if (_cached) {
         if (cacheSize == _count) {
            _cached = false;

            for (size_t i = 0 ; i < _count ; i++)
               _map.add(_cache[i].key, _cache[i].item);

            _map.add(key, value);
         }
         else {
            _cache[_count].key = key;
            _cache[_count].item = value;
            _cache[_count].next = 0;

            _count++;
            return;
         }
      }
      else _map.add(key, value);
   }

   bool add(Key key, T item, const bool unique)
   {
      if (!unique || !exist(key)) {
         add(key, item);

         return true;
      }
      else return false;
   }

   T exclude(Key key)
   {
      if (_cached) {
         for (size_t i = 0 ; i < _count ; i++) {
            if (_cache[i].key == key) {
               T item = _cache[i].item;

               for (size_t j = i + 1 ; j < _count ; j++) {
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

   void write(StreamWriter* writer)
   {
      writer->writeDWord(_cached ? -1 : 0);

      if (_cached) {
         writer->writeDWord(_count);
         writer->write(&_cache, sizeof(Item) * _count);
      }
      else _map.write(writer);
   }

   void read(StreamReader* reader)
   {
      _cached = (reader->getDWord() == -1);

      if(_cached) {
         _count = reader->getDWord();
         reader->read(&_cache, sizeof(Item) * _count);
      }
      else _map.read(reader);
   }

   void clear()
   {
      _map.clear();

      _count = 0;
      _cached = true;
   }

   CachedMemoryMap()
   {
      _count = 0;

      _cached = true;
   }
   CachedMemoryMap(T defaultItem)
      : _map(defaultItem)
   {
      _count = 0;

      _cached = true;
   }
   ~CachedMemoryMap() { clear(); }
};

// --- Hashtable template ---

template <class Key, class T, int(_scaleKey)(Key), int hashSize> class HashTable
{
   typedef _MapItem<Key, T> Item;

   class HashTableIterator
   {
      friend class HashTable;

      Item*            _current;
      int              _hashIndex;
      const HashTable* _hashTable;

      HashTableIterator(const HashTable* hashTable, int hashIndex, Item* current)
      {
         _hashTable = hashTable;
         _hashIndex = hashIndex;
         _current = current;
      }
      HashTableIterator(const HashTable* hashTable)
      {
         _hashTable = hashTable;
         _current = NULL;

         if (_hashTable->_count!=0) {
            for(_hashIndex = 0; !_hashTable->_table[_hashIndex] && _hashIndex < hashSize ; _hashIndex++);

            _current = _hashTable->_table[_hashIndex];
         }
      }

   public:
      HashTableIterator& operator =(const HashTableIterator& it)
      {
         this->_current = it._current;
         this->_hashIndex = it._hashIndex;
         this->_hashTable = it._hashTable;

         return *this;
      }

      HashTableIterator& operator++()
      {
         _current = _current->next;
         while (!_current && _hashIndex < (hashSize - 1)) {
            _current = _hashTable->_table[++_hashIndex];
         }
         return *this;
      }

      HashTableIterator operator++(int)
      {
         HashTableIterator tmp = *this;
         ++*this;

         return tmp;
      }

      T operator*() const { return _current->item; }

      Key key() const { return _current->key; }

      bool Eof() const { return (_current == NULL); }

      HashTableIterator()
      {
         _current = NULL;
         _hashIndex = hashSize;
         _hashTable = NULL;
      }
   };
   friend class HashTableIterator;

   size_t  _count;
   Item* _table[hashSize];
   T     _defaultItem;

   void  (*_freeT)(T);

public:
   typedef HashTableIterator    Iterator;

   T DefaultValue() const { return _defaultItem; }

   size_t Count() const { return _count; }

   Iterator start() const
   {
      return Iterator(this);
   }

   T get(Key key) const
   {
      Iterator it = getIt(key);

      return it.Eof() ? _defaultItem : *it;
   }

   Iterator getIt(Key key) const
   {
      int index = _scaleKey(key);
      if (index > hashSize)
         index = hashSize - 1;

      Item* current = _table[index];
      while (current && (*current < key))
         current = current->next;

      if (current && (*current != key)) {
         return Iterator(this, hashSize, NULL);
      }
      else return Iterator(this, index, current);
   }

   bool exist(Key key) const
   {
      Iterator it = getIt(key);

      return !it.Eof();
   }

   bool exist(const int key, T item) const
   {
      Iterator it = getIt(key);
      while (!it.Eof() && it.key() == key) {
         if (*it == item)
            return true;

         it++;
      }
      return false;
   }

   void add(Key key, T item)
   {
      int index = _scaleKey(key);
      if (index > hashSize)
         index = hashSize - 1;

      if (_table[index] && *_table[index] <= key) {
         Item* current = _table[index];
         while (current->next && *(current->next) <= key) {
            current = current->next;
         }
         current->next = new Item(key, item, current->next);
      }
      else _table[index] = new Item(key, item, _table[index]);

      _count++;
   }

   bool add(int key, T item, bool unique)
   {
      if (!unique || !exist(key, item)) {
         add(key, item);

         return true;
      }
      else return false;
   }

   void write(StreamWriter* writer)
   {
      writer->writeDWord(_count);
      Iterator it = start();
      while (!it.Eof()) {
         _writeIterator(writer, it.key(), *it);

         it++;
      }
   }

   void read(StreamReader* reader)
   {
      Key key;
      T   value = _defaultItem;

      size_t counter = 0;
      reader->readDWord(counter);
      _readToMap(reader, this, counter, key, value);
   }

   void clear()
   {
      for(int i = 0 ; i < hashSize ; i++) {
         while (_table[i]) {
            Item* tmp = _table[i];
            _table[i] = _table[i]->next;

            if (_freeT)
               _freeT(tmp->item);

            delete tmp;
         }
         _table[i] = NULL;
      }
      _count = 0;
   }

   HashTable(T defaultItem)
   {
      _defaultItem = defaultItem;
      _freeT = NULL;
      _count = 0;

      for(int i = 0 ; i < hashSize ; i++) {
         _table[i] = NULL;
      }
   }
   ~HashTable()
   {
      clear();
   }
};

// --- MemoryHashTable template ---

template <class Key, class T, size_t(_scaleKey)(Key), size_t hashSize, bool KeyStored = true> class MemoryHashTable
{
   typedef _MemoryMapItem<Key, T, KeyStored> Item;

   class MemoryHashTableIterator
   {
      friend class MemoryHashTable;

      const MemoryDump* _buffer;
      size_t      _position;
      Item*       _current;
      size_t      _hashIndex;

      MemoryHashTableIterator(const MemoryDump* buffer, size_t hashIndex, size_t position, Item* current)
      {
         _buffer = buffer;
         _hashIndex = hashIndex;
         _current = current;
         _position = position;
      }
      MemoryHashTableIterator(const MemoryDump* buffer)
      {
         _buffer = buffer;
         _current = NULL;

         if (buffer->Length() > 0) {
            for(_hashIndex = 0; _hashIndex < hashSize && !(*buffer)[_hashIndex << 2]; _hashIndex++);

            if (_hashIndex < hashSize) {
               _position = (*_buffer)[_hashIndex << 2];
               _current = (Item*)_buffer->get(_position);
            }
            else _current = NULL;
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
         if (_current->next!=0) {
            _position = _current->next;

            if (_position != 0) {
               _current = (Item*)_buffer->get(_position);
            }
            else _current = NULL;
         }
         else _current = NULL;

         while (!_current && _hashIndex < (hashSize - 1)) {
            _hashIndex++;

            _position = (*_buffer)[_hashIndex << 2];
            if (_position != 0)
               _current = (Item*)_buffer->get(_position);
         }
         return *this;
      }

      MemoryHashTableIterator operator++(int)
      {
         MemoryHashTableIterator tmp = *this;
         ++*this;

         return tmp;
      }

      T operator*() const { return _current->item; }

      Key key() const
      {
         return _current->getKey(_current->key);
      }

      bool Eof() const { return (_current == NULL); }

      MemoryHashTableIterator()
      {
         _current = NULL;
         _position = 0;
         _hashIndex = hashSize;
         _buffer = NULL;
      }
   };
   friend class MemoryHashTableIterator;

   MemoryDump _buffer;
   size_t     _count;

   T     _defaultItem;

public:
   typedef MemoryHashTableIterator Iterator;

   size_t Count() const { return _count; }

   T DefaultValue() const { return _defaultItem; }

   Iterator start() const
   {
      return Iterator(&_buffer);
   }

   Iterator getIt(Key key) const
   {
      size_t beginning = (size_t)_buffer.get(0);

      size_t index = _scaleKey(key);
      if (index > hashSize)
         index = hashSize - 1;

      size_t position = _buffer[index << 2];
      Item* current = (position != 0) ? (Item*)(beginning + position) : NULL;
      while (current && *current < key) {
         if (current->next != 0) {
            position = current->next;
            current = (Item*)(beginning + position);
         }
         else current = NULL;
      }

      if (current && (*current != key)) {
         return Iterator((const MemoryDump*)&_buffer, index, 0, NULL);
      }
      else return Iterator((const MemoryDump*)&_buffer, index, position, current);
   }

   T get(Key key) const
   {
      Iterator it = getIt(key);

      return it.Eof() ? _defaultItem : *it;
   }

   bool exist(const int key, T item) const
   {
      Iterator it = getIt(key);
      while (!it.Eof() && it.key() == key) {
         if (*it == item)
            return true;

         it++;
      }
      return false;
   }

   bool exist(Key key) const
   {
      Iterator it = getIt(key);
      if (!it.Eof()) {
         return true;
      }
      else return false;
   }

   ref_t storeKey(size_t, ref_t key)
   {
      return key;
   }

   ident_t storeKey(size_t position, ident_t key)
   {
      if (KeyStored) {
         size_t keyPos = _buffer.Length();

         _buffer.writeLiteral(keyPos, key);

         return (ident_t)(keyPos - position);
      }
      else return key;
   }

   void add(Key key, T value)
   {
      Item item(key, value, 0);

      size_t index = _scaleKey(key);
      if (index > hashSize)
         index = hashSize - 1;

      int position = _buffer[index << 2];

      size_t tale = _buffer.Length();

      _buffer.write(tale, &item, sizeof(item));
      if (KeyStored) {
         // save stored key
         ref_t storedKey = (ref_t)storeKey(tale, key);
         _buffer.writeDWord(tale + 4, storedKey);
      }

      size_t beginning = (size_t)_buffer.get(0);

      Item* current = (position != 0) ? (Item*)(beginning + position) : NULL;
      if (current && *current <= key) {
         while (current->next != 0 && *(Item*)(beginning + current->next) <= key) {
            position = current->next;
            current = (Item*)(beginning + position);
         }
         _buffer[tale] = current->next;
         current->next = tale;
      }
      else {
         if (position==0) {
            _buffer[tale] = 0;
         }
         else _buffer[tale] = position;
         _buffer[index << 2] = tale;
      }

      _count++;
   }

   bool add(int key, T item, bool unique)
   {
      if (!unique || !exist(key, item)) {
         add(key, item);

         return true;
      }
      else return false;
   }

   void write(StreamWriter* writer)
   {
      writer->writeDWord(_buffer.Length());
      writer->writeDWord(_count);

      MemoryReader reader(&_buffer);
      writer->read(&reader, _buffer.Length());
   }

   void read(StreamReader* reader)
   {
      _buffer.clear();

      int length = reader->getDWord();
      if (length > 0) {
         _buffer.reserve(length);

         _count = reader->getDWord();

         MemoryWriter writer(&_buffer);
         writer.read(reader, length);
      }
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

// --- Cache ---

template<class Key, class T, size_t cacheSize> class Cache
{
   struct Item
   {
      Key key;
      T   value;

      bool operator ==(int key) const
      {
         return (this->key == key);
      }

      bool operator !=(int key) const
      {
         return (this->key != key);
      }

      bool operator ==(size_t key) const
      {
         return (this->key == key);
      }

      bool operator !=(size_t key) const
      {
         return (this->key != key);
      }

      bool operator ==(ident_t key) const
      {
         return StringHelper::compare(this->key, key);
      }

      bool operator !=(ident_t key) const
      {
         return !StringHelper::compare(this->key, key);
      }
   };

   Item _items[cacheSize];
   T   _defaultValue;
   int _top;
   int _tale;

   void incIndex(int& index) const
   {
      index++;

      if (index==cacheSize)
         index = 0;
   }

public:
   T get(Key key) const
   {
      int i = _top;
      while (i != _tale) {
         if (_items[i] == key) {
            return _items[i].value;
         }
         incIndex(i);
      };

      return _defaultValue;
   }

   void exist(Key key) const
   {
      int i = _top;
      while (i != _tale) {
         if (_items[i] == key) {
            return true;
         }
         incIndex(i);
      };

      return false;
   }

   void add(Key key, T value)
   {
      _items[_tale].key = key;
      _items[_tale].value = value;

      incIndex(_tale);
      if (_tale==_top) {
         incIndex(_top);
      }
   }

   void clear()
   {
      _top = _tale = 0;
   }

   Cache()
   {
      _top = _tale = 0;
      _defaultValue = NULL;
   }
   Cache(T defaultValue)
   {
      _top = _tale = 0;
      _defaultValue = defaultValue;
   }
};

// --- IntFixedMap ---

template<class T> class IntFixedMap
{
   MemoryDump _buffer;
   T          _defaultValue;

public:
   struct Iterator
   {
      char* _array;
      int   _position;
      int   _end;

      Iterator& operator =(const Iterator& it)
      {
         this->_array = it._array;
         this->_position = it._position;
         this->_end = it._end;

         return *this;
      }

      bool operator ==(const Iterator& it)
      {
         return (this->_position == it._position && this->_array == it._array);
      }

      bool operator !=(const Iterator& it)
      {
         return !(*this == it);
      }

      Iterator& operator ++()
      {
         _position += (4 + sizeof(T));

         return *this;
      }
      Iterator operator ++(int)
      {
         Iterator tmp = *this;
         ++*this;

         return tmp;
      }

      Iterator& operator--()
      {
         _position -= (4 + sizeof(T));

         return *this;
      }
      Iterator operator--(int)
      {
         Iterator tmp = *this;
         --*this;

         return tmp;
      }

      bool Eof() const { return (_position >= _end || _position < 0); }

      T& operator*() const { return *(T*)(_array + _position + 4); }

      size_t key() const { return *(size_t*)(_array + _position); }

      Iterator(void* buffer)
      {
         _array = (char*)buffer;
         _position = 4;
         _end = (*(int*)_array) << 3;  // _array starts with the number of items
      }

      Iterator(char* buffer, int position)
      {
         _array = buffer;
         _position = position;
         _end = (*(int*)_array) << 3;  // _array starts with the number of items
      }
   };

   T DefaultValue() const { return _defaultValue; }

   size_t Count() const { return _buffer[0]; }

   size_t Size() const { return _buffer.Length(); }

   void* Array() const { return _buffer.get(0); }

   Iterator start() const
   {
      return Iterator((char*)_buffer.get(0), 4); // _buffer starts with the number of items
   }

   Iterator getIt(size_t key) const
   {
      int count = _buffer[0];
      int position = 4;
      while (count > 0) {
         if (_buffer[position]==key)
            return Iterator((char*)_buffer.get(0), position);

         count--;
         position += (4 + sizeof(T));
      }
      return Iterator((char*)_buffer.get(0), -1);
   }

   T get(size_t key) const
   {
      Iterator it = getIt(key);
      return it.Eof() ? _defaultValue : *it;
   }

   T& operator[](size_t index)
   {
      return *(T*)(_buffer.get(8 + (sizeof(T) + 4) * index));
   }

   T* getPtr(size_t key) const
   {
      int count = _buffer[0];
      int position = 4;
      while (count > 0) {
         if (_buffer[position]==key)
            return (T*)_buffer.get(position + 4);

         count--;
         position += (4 + sizeof(T));
      }
      return NULL;
   }

   bool exist(size_t key) const
   {
      int count = _buffer[0];
      int position = 4;
      while (count > 0) {
         if (_buffer[position]==key)
            return true;

         count--;
         position += (4 + sizeof(T));
      }
      return false;
   }

   void add(size_t key, T item)
   {
      int position = _buffer.Length();
      _buffer.writeDWord(position, key);
      _buffer.write(position + 4, &item, sizeof(T));

      _buffer[0] = (_buffer[0] + 1);
   }

   bool add(size_t key, size_t item, const bool unique)
   {
      if (!unique || !exist(key)) {
         add(key, item);

         return true;
      }
      else return false;
   }

   void write(StreamWriter* writer)
   {
      writer->write(_buffer.get(0), _buffer.Length());
   }

   void read(StreamReader* reader)
   {
      size_t count = reader->getDWord();

      _buffer.clear();
      _buffer.writeDWord(0, count);

      if (count > 0)
         _buffer.load(reader, count << 3);
   }

   void clear()
   {
      _buffer.clear();
      _buffer.writeDWord(0, 0);
   }

   IntFixedMap(T defValue, int reserved)
      : _buffer(reserved)
   {
      _defaultValue = defValue;
      _buffer.writeDWord(0, 0);
   }
   IntFixedMap(T defValue)
   {
      _defaultValue = defValue;
      _buffer.writeDWord(0, 0);
   }
   IntFixedMap()
   {
      _defaultValue = (T)NULL;
      _buffer.writeDWord(0, 0);
   }
};

// --- Dictionary2D ---

// !! should be declared inside Dictionary2D
enum ValueType
{
   stDWORD,
   stString,
   stMap
};

template<class VItem> void freeVItem(VItem item)
{
   if (item.type == stString) {
      freestr((ident_c*)item.value.literal);
   }
   else if (item.type == stMap) {
      freeobj(item.value.map);
   }
}

template <class Key, class SubKey> class Dictionary2D
{
public:
   struct VItem
   {
      union Value
      {
         ident_t             literal;
         int                 number;
         bool                flag;
         size_t              size;
         Map<SubKey, VItem>* map;
      };

      ValueType type;
      Value     value;

      operator int() const { return value.number; }

//      operator wchar16_t*() const { return (wchar16_t*)value.literal16; }

      operator ident_t() const { return value.literal; }

//      operator char*() const { return (char*)value.literal8; }
//
//      operator const char*() const { return value.literal8; }

      operator Map<SubKey, VItem>*() const { return type==stMap ? value.map : NULL; }

      VItem()
      {
         value.number = 0;
         type = stDWORD;
      }
      VItem(int number)
      {
         value.number = number;
         type = stDWORD;
      }
      VItem(size_t number)
      {
         value.size = number;
         type = stDWORD;
      }
      VItem(ident_c* literal)
      {
         value.literal = literal;
         type = stString;
      }
      VItem(ident_t literal)
      {
         value.literal = literal;
         type = stDWORD;
      }
//      VItem(const wchar16_t* literal)
//      {
//         value.literal16 = literal;
//         type = stDWORD;
//      }
//      VItem(char* literal)
//      {
//         value.literal8 = literal;
//         type = stString8;
//      }
//      VItem(const char* literal)
//      {
//         value.literal8 = literal;
//         type = stDWORD;
//      }
      VItem(Map<SubKey, VItem>* map)
      {
         value.map = map;
         type = stMap;
      }
   };

protected:
   Map<Key, VItem> _items;
   bool            _allowDuplicates;

public:
   template<class Value> void add(Key key, Value value)
   {
      if (!_allowDuplicates)
         _items.erase(key);

      _items.add(key, VItem(value));
   }
   template<class Value> void add(Key key, SubKey subKey, Value value)
   {
      Map<SubKey, VItem>* map = _items.get(key);
      if (map==NULL) {
         map = new Map<SubKey, VItem>(VItem(0), freeVItem);

         _items.add(key, VItem(map));
      }
      if (!_allowDuplicates)
         map->erase(subKey);

      map->add(subKey, VItem(value));
   }

   _Iterator<VItem, _MapItem<Key, VItem>, Key> start()
   {
      return _items.start();
   }

   _Iterator<VItem, _MapItem<SubKey, VItem>, SubKey> getIt(Key key)
   {
      Map<SubKey, VItem>* setting = _items.get(key);
      if (setting) {
         return setting->start();
      }
      else return _Iterator<VItem, _MapItem<SubKey, VItem>, SubKey>();
   }

   int getCount(Key key)
   {
      Map<SubKey, VItem>* setting = _items.get(key);

      return setting ? setting->Count() : 0;
   }

   template<class Value> Value get(Key key, Value defaultValue)
   {
      if (_items.exist(key)) {
	      return _items.get(key);
      }
      else return defaultValue;
   }

   bool exist(Key key)
   {
      return _items.exist(key);
   }

   template<class Value> Value get(Key key, SubKey subKey, Value defaultValue)
   {
      Map<SubKey, VItem>* map = _items.get(key);
      if (map && map->exist(subKey)) {
         return map->get(subKey);
      }
      else return defaultValue;
   }

   void clear(Key key, SubKey subKey)
   {
      Map<SubKey, VItem>* map = _items.get(key);
      if (map) {
	      map->erase(subKey);
      }
   }

   void clear(Key key)
   {
      _items.erase(key);
   }

   void clear()
   {
      _items.clear();
   }

   Dictionary2D()
      : _items(VItem(0), freeVItem)
   {
      _allowDuplicates = false;
   }
   Dictionary2D(bool allowDuplicates)
      : _items(VItem(0), freeVItem)
   {
      _allowDuplicates = allowDuplicates;
   }
   ~Dictionary2D() {}
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
      T      value;
      size_t parent;
      size_t firstChildLink;

      Node()
      {
      }
      Node(T defValue)
      {
         value = defValue;
         firstChildLink = parent = 0;
      }
      Node(T value, size_t parent)
      {
         this->value = value;
         this->parent = parent;
         this->firstChildLink = 0;
      }
   };

   struct ChildLink
   {
      size_t node;
      size_t nextChildLink;

      ChildLink()
      {
      }
      ChildLink(size_t node, size_t nextChildLink)
      {
         this->node = node;
         this->nextChildLink = nextChildLink;
      }
   };

   size_t addRootNode(T value)
   {
      // add new node
      size_t position = _buffer.Length();
      Node node(value, -1);
      _buffer.write(position, &node, sizeof(Node));

      return position;
   }

   size_t addNode(size_t parent, T value)
   {
      // add new node
      size_t position = _buffer.Length();
      Node node(value, parent);
      _buffer.write(position, &node, sizeof(Node));

      addChildLink(parent, position);

      return position;
   }

   void addChildLink(size_t parent, size_t child)
   {
      // add new child link
      size_t linkPosition = _buffer.Length();
      ChildLink link(child, 0);
      _buffer.write(linkPosition, &link, sizeof(ChildLink));

      // update parent
      Node parentNode;
      _buffer.read(parent, &parentNode, sizeof(Node));

      if (parentNode.firstChildLink == 0) {
         parentNode.firstChildLink = linkPosition;

         _buffer.write(parent, &parentNode, sizeof(Node));
      }
      else {
         ChildLink link(0, parentNode.firstChildLink);
         size_t position = parentNode.firstChildLink;
         do {
            position = link.nextChildLink;

            _buffer.read(position, &link, sizeof(ChildLink));
         } while (link.nextChildLink != 0);

         link.nextChildLink = linkPosition;
         _buffer.write(position, &link, sizeof(ChildLink));
      }
   }

   Node getNode(size_t position)
   {
      Node node(_defValue);
      _buffer.read(position, &node, sizeof(Node));

      return node;
   }

   size_t getLinkNodePosition(size_t linkPosition)
   {
      if (linkPosition != 0) {
         ChildLink link(0, 0);
         _buffer.read(linkPosition, &link, sizeof(ChildLink));

         return link.node;
      }
      else return 0;
   }

   size_t getNextLink(size_t linkPosition)
   {
      ChildLink link;
      _buffer.read(linkPosition, &link, sizeof(ChildLink));

      return link.nextChildLink;
   }


   void save(StreamWriter* writer)
   {
      writer->writeDWord(_buffer.Length());

      MemoryReader reader(&_buffer);
      writer->read(&reader, _buffer.Length());
   }

   void load(StreamReader* reader)
   {
      int length = reader->getDWord();
      _buffer.reserve(length);

      MemoryWriter writer(&_buffer);
      writer.read(reader, length);
   }

   MemoryTrie(T defValue)
   {
      _defValue = defValue;
   }
   ~MemoryTrie()
   {
   }
};

// --- MemoryTrieNode ---

template <class T> class MemoryTrieNode
{
   size_t         _position;
   MemoryTrie<T>* _trie;

public:
   typedef typename MemoryTrie<T>::Node Node;

   MemoryTrie<T>* Trie() { return _trie; }

   size_t Position() { return _position; }

   T Value()
   {
      Node node = _trie->getNode(_position);

      return node.value;
   }

   void link(MemoryTrieNode& node)
   {
      _trie->addChildLink(_position, node.Position());
   }

   MemoryTrieNode Parent()
   {
      Node node = _trie->getNode(_position);

      return MemoryTrieNode(_trie, node.parent);
   }

   MemoryTrieNode FirstNode()
   {
      Node node = _trie->getNode(_position);

      ref_t childPosition = _trie->getLinkNodePosition(node.firstChildLink);

      return MemoryTrieNode(_trie, childPosition);
   }

   bool operator ==(MemoryTrieNode& node) const
   {
      return (this->_position == node._position);
   }

   bool operator !=(MemoryTrieNode& node) const
   {
      return (this->_position != node._position);
   }

   MemoryTrieNode()
   {
      _trie = NULL;
      _position = 0;
   }
   MemoryTrieNode(MemoryTrie<T>* trie)
   {
      _trie = trie;
      _position = 0;
   }
   MemoryTrieNode(MemoryTrie<T>* trie, size_t position)
   {
      _trie = trie;
      _position = position;
   }

   struct ChildEnumerator
   {
      friend class MemoryTrieNode;

   private:
      MemoryTrie<T>* _trie;
      size_t         _linkPosition;

      ChildEnumerator(MemoryTrie<T>* trie, size_t linkPosition)
      {
         _trie = trie;
         _linkPosition = linkPosition;
      }

   public:
      MemoryTrieNode Node()
      {
         size_t position = _trie->getLinkNodePosition(_linkPosition);

         return MemoryTrieNode(_trie, position);
      }

      bool Eof()
      {
         return (_linkPosition == 0);
      }

      ChildEnumerator& operator ++()
      {
         _linkPosition = _trie->getNextLink(_linkPosition);

         return *this;
      }
      ChildEnumerator operator ++(int)
      {
         ChildEnumerator tmp = *this;
         ++*this;

         return tmp;
      }

      ChildEnumerator()
      {
         _linkPosition = 0;
      }
   };

   ChildEnumerator Children()
   {
      Node node = _trie->getNode(_position);

      return ChildEnumerator(_trie, node.firstChildLink);
   }

   ChildEnumerator find(T value)
   {
      ChildEnumerator children = Children();
      while (!children.Eof() && children.Node().Value() != value)
         children++;

      return children;
   }
};

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

// --- shift routine ---

template <class Iterator, class T> void shift(Iterator it, T minValue, const int displacement)
{
   while (!it.Eof()) {
      if ((*it) >= minValue)
         *it += displacement;

      it++;
   }
}

// --- retrieveKey routine ---

template<class Key, class T, class Iterator> Key goToKey(Iterator& it, T value, Key defaultKey)
{
   while (!it.Eof()) {
      if (*it == value)
         return it.key();

      it++;
   }
   return defaultKey;
}

template<class Key, class T, class Iterator> Key retrieveKey(Iterator it, T value, Key defaultKey)
{
   while (!it.Eof()) {
      if (*it == value)
         return it.key();

      it++;
   }
   return defaultKey;
}

template<class Iterator> Iterator retrieveIt(Iterator it, int value)
{
   while (!it.Eof()) {
      if (*it == value)
         return it;

      it++;
   }
   return it;
}

template<class Iterator> const char* retrieve(Iterator it, const char* value, const char* defaultValue)
{
   while (!it.Eof()) {
      if (StringHelper::compare(*it, value))
         return *it;

      it++;
   }
   return defaultValue;
}

template<class Iterator> const wide_c* retrieve(Iterator it, const wide_c* value, const wide_c* defaultValue)
{
   while (!it.Eof()) {
      if (StringHelper::compare(*it, value))
         return *it;

      it++;
   }
   return defaultValue;
}

//template<class Iterator, class T> int retrieveIndex(Iterator it, T value)
//{
//   int index = 0;
//   while (!it.Eof()) {
//      if (*it == value)
//         return index;
//
//      index++;
//      it++;
//   }
//
//   return -1;
//}

} // _ELENA_

#endif // listsH
