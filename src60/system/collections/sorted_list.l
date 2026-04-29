namespace collections
{
   // --- SortedList ---
   sealed class SortedListItem
   {
      object          Key  : rprop;
      object          Item : prop;
      SortedListItem? Next : prop;
   
      constructor(object key, object item)
      {
         Key := key;
         Item := item;
         Next := nil;
      }
   }

   public sealed class SortedList : Enumerable, info("A sorted generic list of key value pairs")
   {
      SortedListItem? _top;
      Reference<int>  _length;
   
      Func<object,object,int> _comparator;
   
      constructor new() : info("Creates a weak-typed list sorted by ascending value")
         <= new((object l,object r => (l < r) ? -1 : ((l == r) ? 0 : 1)))
      {
      }
   
      constructor new(Func<object,object,int> comparator) : info("Creates a weak-typed list with a custom sort function<br/>A function must return -1 if in the right order, 1 if the order is opposite and 0 if they are the same")
      {
         _comparator := comparator;
         _length := 0;
      }   

      int Length : info("Returns the number of elements in the collection")
         = *_length;

      internal SortedListItem? FirstItem
         = _top;

      append(object key, object item) : info("Appends a new key-value element to the end of the list<br/>The element position is defined by its sorted order")
      {
         auto item := new SortedListItem(key, item);
   
         SortedListItem? prev := nil;
         SortedListItem? current := _top;
         while (current != nil && _comparator(current.Key, key) < 0) {
            prev := current;
            current := current.Next;
         };
   
         if (prev == nil) {
            item.Next := _top;
            _top := item;
         }
         else {
            item.Next := prev;
            prev.Next := item;
         };

         _length++
      }
   
      private SortedListItem retrieve(int index)
      {
         int n := index;
         SortedListItem? current := _top;
         while (n > 1) {
            current := current!.Next;
            n--;
         };
   
         ^ current!
      }
   
      object at(int index) : info("Returns the value at a position <b>index</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         SortedListItem current := retrieve(index);
   
         ^ current.Item;
      }   
   
      setAt(int index, object item) : info("Sets the element value at a position <b>index</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         SortedListItem current := retrieve(index);
         
         current.Item := item;
      }

      int indexOfKey(object key) : info("Returns the element position based on its key value<br/>If there is no element with the specified key, it returns -1")
      {
         int index := 1;
         SortedListItem? current := _top;
         while (current != nil) {
            if (current.Key == key) {
               ^ index;
            };
            index++;
         };

         ^ -1
      }

      removeAt(int index) : info("Removes the element at the specified position")
      {
         int n := index;
         SortedListItem? prev := nil;
         SortedListItem? current := _top;
         while (n > 1) {
            prev := current;
            current := current!.Next;
            n--;
         };

         if (prev == nil) {
            _top := _top!.Next;
         }
         else {
            prev.Next := current!.Next
         };

         _length--
      }

      Enumerator enumerator() : info("Returns the collection enumerator")
         = new SortedListEnumerator(self);

      Indexer indexer() : info("Returns the collection indexer")
         = new SortedListIndexer(self);
   }

   // --- SortedListEnumerator ---
   public class SortedListEnumerator : Enumerator, info("A sorted collection enumerator")
   {
      SortedList      _list;
      SortedListItem? _current;
      bool            _started; 

      constructor(SortedList list) : info("Creates an enumerator of <b>list</b> sorted collection")
      {
         _list := list;
         _started := false;
      }

      Value : info("Returns the current element value")
         = _current?.Item;

      bool next() : info("Goes to the next enumeration member and returns true if it is not the last member")
      {
         if (!_started) {
            _current := _list.FirstItem
         }
         else _current := _current?.Next;

         ^ _current != nil
      }

      reset() : info("Resets the enumerator")
      {
         _started := false
      }

      enumerable() : info("returns the enumerating sorted collection")
         = _list;
   }
   
   // --- SortedListIndexer ---
   public class SortedListIndexer : Indexer, info("A sorted collection indexer")
   {      
      SortedList     _list;
      Reference<int> _index;

      constructor(SortedList list) : info("Creates an indexer of <b>list</b> sorted collection")
      {
         _list := list;
         _index := 0
      }

      bool Available : info("Returns true if there is an element with a given index in the corresponding collection<br/>Otherwise returns false")
         = *_index < _list.Length;

      Value : info("The indexed element value getter / setter")
      {
         get()
            = _list[*_index];

         set(value)
         {
            _list.setAt(*_index, value);
         }
      }

      int Index : info("Index property")
      {
         get()
            = *_index;

         set(value)
         {
            _index.Value := value
         }
      }

      int Length : info("Returns the length of the indexed sorted collection")
         = _list.Length;

      appendIndex(int index) : info("Increases the current index by parameter **index**")
      {
         _index.Value := *_index + index
      }
   }

   public sealed class SortedArrayList : Indexable, info("A sorted weak list of key-value pairs")
   {
      VarTuple<object, object>[] _array;
      Reference<int>             _length;
      Reference<int>             _capacity;

      Func<object,object,int>    _comparator;

      constructor() : info("Creates an empty list sorted by ascending key")
      {
         _length := 0;
         _capacity := 0;
         _comparator := (object l,object r => (l < r) ? -1 : ((l == r) ? 0 : 1));
      }

      constructor(Func<object,object,int> comparator) : info("Creates an empty list with a custom oredering function<br/>The function returns -1 if a pair of item keys is ordered, 0 if they are equal and 1 otherwise")
      {
         _length := 0;
         _capacity := 0;
         _comparator := comparator;
      }

      constructor allocate(int capacity) : info("Creates an empty list sorted by ascending key with pre-allocated slots")
         <= allocate(capacity, (object l,object r => (l < r) ? -1 : ((l == r) ? 0 : 1)));

      constructor allocate(int capacity, Func<object,object,int> comparator) : info("Creates an empty list with a custom oredering function with pre-allocated slots<br/>The function returns -1 if a pair of item keys is ordered, 0 if they are equal and 1 otherwise")
      {
         _length := 0;
         _capacity := capacity;
         _array := new VarTuple<object, object>[](capacity);
         _comparator := comparator;
      }

      constructor allocate() : info("Creates an empty list sorted by ascending key with 32 pre-allocated slots")
         <= allocate(32);

      int Length : info("Returns the number of elements in the list")
      {
         get()
         {
            int retVal := *_length;

            ^ retVal
         }
      }

      private expand()
      {
         int cap := *_capacity;
         int len := *_length;
         if (cap <= len)
         {
            _capacity.append(32);

            cap := *_capacity;
            auto newArray := new VarTuple<object, object>[](cap);
            for(int i := 0; i < len; i := i + 1)
            {
               newArray[i] := _array[i];
            };

            _array := newArray
         }
      }

      append(object key, object item) : info("Adds a key-value pair, the position of the element is defined by key using the given comparator")
      {
         expand();

         int len := *_length;

         auto item := new VarTuple<object, object>(key, item);
         int index := -1;
         for (int i := 0; i < len; i++) {
            if (_comparator(_array[i].Item1, key) < 0) {
               index := i;
               :break
            }
         };
         if (index != -1) {
            int i := len;
            while (i > index)
            {
               _array[i] := _array[i - 1];
   
               i--
            };
            _array[index] := item;
         }
         else {
            _array[len] := item;
         };

         _length.append(1);
      }

      at(int index) : info("Returns the value of a key-value pair at the specified position")
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         ^ _array[index].Item2
      }      

      setAt(int index, object o) : info("Sets the value of a key-value pair at the specified position")
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         auto item := _array[index];
         item.Item2 := o
      }

      remove(int index) : info("Removes a key-value pair at the specified position")
      {
         if (index >= *_length)
            { OutOfRangeException.raise() };
  
         _length.reduce(1);

         int i := index;
         int len := *_length;
         while (i < len)
         {
            _array.setAt(i, _array.at(i + 1));

            i := i + 1
         }
      }

      clear() : info("Removes all key-value paires")
      {
         _length.Value := 0
      }

      trim(int pos) : info("Removes the last <b>pos</b> key-value pairs of the collection")
      {
         if (pos > *_length)
            { OutOfRangeException.raise() };

         _length.Value := pos
      }

      Enumerator enumerator() : info("Returns the list enumerator") = new BaseEnumerator
      {
         this index := -1;
           
         this length := self.Length;

         get Value() = self.at(this index.Value);

         enumerable() = self;
           
         reset()
         {
            this index.Value := -1
         }
      };

      Indexer indexer() : info("Returns the list indexer") = new BaseIndexer
      {
         this index := 0;
           
         this length := 1;
           
         get Value()
            = self.at(this index);
               
         set Value(o)
         {
            self.setAt(this index, o)
         }
      };
   }

   // --- SortedList<TKey,TValue> ---                                                      
   public sealed class SortedList<TKey, TValue> : Enumerable<TValue>, info("A sorted strong typed list of key-value pairs")
   {
      VarTuple<TKey, TValue>[] _array;
      Reference<int>           _length;
      Reference<int>           _capacity;

      Func<TKey,TKey,int>    _comparator;

      constructor() : info("Creates an empty list sorted by an ascending key")
      {
         _length := 0;
         _capacity := 0;
         _comparator := (TKey l,TKey r => (l < r) ? -1 : ((l == r) ? 0 : 1));
      }

      constructor(Func<TKey,TKey,int> comparator) : info("Creates an empty list with a custom oredering function<br/>The function returns -1 if a pair of keys is ordered, 0 if they are equal and 1 otherwise")
      {
         _length := 0;
         _capacity := 0;
         _comparator := comparator;
      }

      constructor allocate(int capacity) : info("Creates an empty list sorted by ascending key with pre-allocated slots")
         <= allocate(capacity, (TKey l,TKey r => (l < r) ? -1 : ((l == r) ? 0 : 1)));

      constructor allocate(int capacity, Func<TKey,TKey,int> comparator) : info("Creates an empty list with a custom oredering function with pre-allocated slots<br/>The function returns -1 if a pair of items is ordered, 0 if they are equal and 1 otherwise")
      {
         _length := 0;
         _capacity := capacity;
         _array := new VarTuple<TKey, TValue>[](capacity);
         _comparator := comparator;
      }

      constructor allocate() : info("Creates an empty list sorted by ascending value with 32 pre-allocated slots")
         <= allocate(32);

      int Length : info("Returns the number of elements in the list")
      {
         get()
         {
            int retVal := *_length;

            ^ retVal
         }
      }

      private expand()
      {
         int cap := *_capacity;
         int len := *_length;
         if (cap <= len)
         {
            _capacity.append(32);

            cap := *_capacity;
            auto newArray := new VarTuple<TKey, TValue>[](cap);
            for(int i := 0; i < len; i := i + 1)
            {
               newArray[i] := _array[i];
            };

            _array := newArray
         }
      }

      append(TKey key, TValue item) : info("Adds a key-value pair, the position of the element is defined by key using the given comparator")
      {
         expand();

         int len := *_length;

         auto item := new VarTuple<TKey, TValue>(key, item);
         int index := -1;
         for (int i := 0; i < len; i++) {
            if (_comparator(_array[i].Item1, key) < 0) {
               index := i;
               :break
            }
         };
         if (index != -1) {
            int i := len;
            while (i > index)
            {
               _array[i] := _array[i - 1];
   
               i--
            };
            _array[index] := item;
         }
         else {
            _array[len] := item;
         };

         _length.append(1);
      }

      TValue at(int index) : info("Returns the value of a key-value pair at the specified position")
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         ^ _array[index].Item2
      }      

      setAt(int index, TValue o) : info("Sets the value of a key-value pair at the specified position")
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         auto item := _array[index];
         item.Item2 := o
      }

      remove(int index) : info("Removes a key-value pair at the specified position")
      {
         if (index >= *_length)
            { OutOfRangeException.raise() };
  
         _length.reduce(1);

         int i := index;
         int len := *_length;
         while (i < len)
         {
            _array.setAt(i, _array.at(i + 1));

            i := i + 1
         }
      }

      clear() : info("Removes all key-value paires")
      {
         _length.Value := 0
      }

      trim(int pos) : info("Removes the last <b>pos</b> key-value pairs of the collection")
      {
         if (pos > *_length)
            { OutOfRangeException.raise() };

         _length.Value := pos
      }

      Enumerator<TValue> enumerator() : info("Returns the list enumerator") = new BaseEnumerator<TValue>
      {
         this index := -1;
           
         this length := self.Length;

         get TValue Value() = self.at(this index.Value);

         enumerable() = self;
           
         reset()
         {
            this index.Value := -1
         }
      };

      Indexer<TValue> indexer() : info("Returns the list indexer") = new BaseIndexer<TValue>
      {
         this index := 0;
           
         this length := 1;
           
         get TValue Value()
            = self.at(this index);
               
         set Value(TValue o)
         {
            self.setAt(this index, o)
         }
      };
   }
}