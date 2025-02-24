namespace collections
{
   // --- SortedList ---
   sealed class SortedListItem
   {
      object         Key  : rprop;
      object         Item : prop;
      SortedListItem Next : prop;
   
      constructor(object key, object item)
      {
         Key := key;
         Item := item;
         Next := nil;
      }
   }

   public sealed class SortedList : Enumerable
   {
      SortedListItem _top;
      Reference<int> _length;
   
      Func<object,object,int> _comparator;
   
      constructor new()
         <= new((object l,object r => l < r ? -1 : (l == r ? 0 : 1)))
      {
      }
   
      constructor new(Func<object,object,int> comparator)
      {
         _comparator := comparator;
         _length := 0;
      }   

      int Length
         = *_length;

      internal SortedListItem FirstItem
         = _top;

      append(object key, object item)
      {
         auto item := new SortedListItem(key, item);
   
         SortedListItem prev := nil;
         SortedListItem current := _top;
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
         SortedListItem current := _top;
         while (n > 1) {
            current := current.Next;
            n--;
         };
   
         ^ current
      }
   
      object at(int index)
      {
         SortedListItem current := retrieve(index);
   
         ^ current?.Item;
      }   
   
      setAt(int index, object item)
      {
         SortedListItem current := retrieve(index);
         
         current.Item := item;
      }

      int indexOfKey(object key)
      {
         int index := 1;
         SortedListItem current := _top;
         while (current != nil) {
            if (current.Key == key) {
               ^ index;
            };
            index++;
         };

         ^ -1
      }

      removeAt(int index)
      {
         int n := index;
         SortedListItem prev := nil;
         SortedListItem current := _top;
         while (n > 1) {
            prev := current;
            current := current.Next;
            n--;
         };

         if (prev == nil) {
            _top := _top.Next;
         }
         else {
            prev.Next := current.Next
         };

         _length--
      }

      Enumerator enumerator() = new SortedListEnumerator(self);

      Indexer indexer() = new SortedListIndexer(self);
   }

   // --- SortedListEnumerator ---
   public class SortedListEnumerator : Enumerator
   {
      SortedList     _list;
      SortedListItem _current;
      bool           _started; 

      constructor(SortedList list)
      {
         _list := list;
         _started := false;
      }

      Value
         = _current.Item;

      bool next()
      {
         if (!_started) {
            _current := _list.FirstItem
         }
         else _current := _current.Next;

         ^ _current != nil
      }

      reset()
      {
         _started := false
      }

      enumerable()
         = _list;
   }
   
   // --- SortedListIndexer ---
   public class SortedListIndexer : Indexer
   {      
      SortedList     _list;
      Reference<int> _index;

      constructor(SortedList list)
      {
         _list := list;
         _index := 0
      }

      bool Available
         = *_index < _list.Length;

      Value
      {
         get()
            = _list[*_index];

         set(value)
         {
            _list.setAt(*_index, value);
         }
      }

      int Index
      {
         get()
            = *_index;

         set(value)
         {
            _index.Value := value
         }
      }

      int Length
         = _list.Length;

      appendIndex(int index)
      {
         _index.Value := *_index + index
      }
   }

   public sealed class SortedArrayList : Indexable
   {
      VarTuple<object, object>[] _array;
      Reference<int>             _length;
      Reference<int>             _capacity;

      Func<object,object,int>    _comparator;

      constructor()
      {
         _length := 0;
         _capacity := 0;
         _comparator := (object l,object r => l < r ? -1 : (l == r ? 0 : 1));
      }

      constructor(Func<object,object,int> comparator)
      {
         _length := 0;
         _capacity := 0;
         _comparator := comparator;
      }

      constructor allocate(int capacity)
         <= allocate(capacity, (object l,object r => l < r ? -1 : (l == r ? 0 : 1)));

      constructor allocate(int capacity, Func<object,object,int> comparator)
      {
         _length := 0;
         _capacity := capacity;
         _array := new VarTuple<object, object>[](capacity);
         _comparator := comparator;
      }

      constructor allocate()
         <= allocate(32);

      int Length
      {
         /// Reads the number of list item to the output parameter
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

      append(object key, object item)
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

      /// Returns the object at the specified position
      at(int index)
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         ^ _array[index].Item2
      }      

      /// Returns the object at the specified position
      setAt(int index, object o)
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         auto item := _array[index];
         item.Item2 := o
      }

      /// Removes an object at the specified position
      remove(int index)
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

      /// Clears the list
      clear()
      {
         _length.Value := 0
      }

      trim(int pos)
      {
         if (pos > *_length)
            { OutOfRangeException.raise() };

         _length.Value := pos
      }

      /// Returns the list enumerator
      Enumerator enumerator() = new BaseEnumerator
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

      /// Returns the list indexer
      Indexer indexer() = new BaseIndexer
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
   public sealed class SortedList<TKey, TValue> : Enumerable<TValue>
   {
      VarTuple<TKey, TValue>[] _array;
      Reference<int>           _length;
      Reference<int>           _capacity;

      Func<TKey,TKey,int>    _comparator;

      constructor()
      {
         _length := 0;
         _capacity := 0;
         _comparator := (TKey l,TKey r => l < r ? -1 : (l == r ? 0 : 1));
      }

      constructor(Func<TKey,TKey,int> comparator)
      {
         _length := 0;
         _capacity := 0;
         _comparator := comparator;
      }

      constructor allocate(int capacity)
         <= allocate(capacity, (TKey l,TKey r => l < r ? -1 : (l == r ? 0 : 1)));

      constructor allocate(int capacity, Func<TKey,TKey,int> comparator)
      {
         _length := 0;
         _capacity := capacity;
         _array := new VarTuple<TKey, TValue>[](capacity);
         _comparator := comparator;
      }

      constructor allocate()
         <= allocate(32);

      int Length
      {
         /// Reads the number of list item to the output parameter
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

      append(TKey key, TValue item)
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

      /// Returns the object at the specified position
      TValue at(int index)
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         ^ _array[index].Item2
      }      

      /// Returns the object at the specified position
      setAt(int index, TValue o)
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         auto item := _array[index];
         item.Item2 := o
      }

      /// Removes an object at the specified position
      remove(int index)
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

      /// Clears the list
      clear()
      {
         _length.Value := 0
      }

      trim(int pos)
      {
         if (pos > *_length)
            { OutOfRangeException.raise() };

         _length.Value := pos
      }

      /// Returns the list enumerator
      Enumerator<TValue> enumerator() = new BaseEnumerator<TValue>
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

      /// Returns the list indexer
      Indexer<TValue> indexer() = new BaseIndexer<TValue>
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