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

      add(object key, object item)
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

   public sealed class SortedArrayList //: Indexable
   {
//      VarTuple<object, object>[] _array;

//      Array          _array;
//      Reference<int> _length;
//      Reference<int> _capacity;
//
//      constructor()
//      {
//         _length := 0;
//         _capacity := 0;
//      }
//
//      constructor allocate(int capacity)
//      {
//         _capacity := capacity;
//         _array := Array.allocate(capacity);
//      }
//
//      constructor allocate()
//         <= allocate(32);
//
//      constructor load(Array a)
//         <= fromArray(a, a.Length);
//
//      constructor load(o)
//      {
//         Enumerator e := o.enumerator();
//         while (e.next())
//         {
//            self.append(*e)
//         }
//      }
//
//      /// Creates the list
//      internal constructor fromArray(Array array, int length)
//      {
//         int capacity := array.Length;
//
//         _capacity := capacity;
//         _length := length;
//
//         _array := array.clone()
//      }
//
//      int Length
//      {
//         /// Reads the number of list item to the output parameter
//         get()
//         {
//            int retVal := *_length;
//
//            ^ retVal
//         }
//      }
//
//      /// Returns the object at the specified position
//      at(int index)
//      {
//         int len := *_length;
//         ifnot (len > index)
//            { OutOfRangeException.raise() };
//
//         ^ _array[index]
//      }      
//
//      /// Returns the object at the specified position
//      setAt(int index, object o)
//      {
//         int len := *_length;
//         ifnot (len > index)
//            { OutOfRangeException.raise() };
//
//         _array.setAt(index, o)
//      }
//
//      private expand()
//      {
//         int cap := *_capacity;
//         int len := *_length;
//         if (cap <= len)
//         {
//            _capacity.append(32);
//
//            cap := *_capacity;
//            Array newArray := Array.allocate(cap);
//            for(int i := 0; i < len; i := i + 1)
//            {
//               newArray[i] := _array[i];
//            };
//
//            _array := newArray
//         }
//      }
//
//      /// Appends a new object to the end of the list
//      append(item)
//      {
//         self.expand();
//
//         int len := *_length;
//         _array[len] := item;
//
//         _length.append(1)
//      }
//
//      appendRange(subarray)
//      {
//         if (nil != subarray)
//         {
//            int len := subarray.Length;
//            for(int i := 0; i < len; i += 1)
//            {
//               self.append(subarray.at(i))
//            }
//         }
//      }
//
//      appendRange(Indexable indexable)
//      {
//         if (nil != indexable)
//         {
//            Indexer it := indexable.indexer();
//
//            while (it.Available)
//            {       
//               self.append(*it);
//
//               it.appendIndex(1)
//            }
//         }
//      }
//
//      /// Inserts a new object to the specified position
//      insert(int index, object object)
//      {
//         if (index >= *_length)
//            { OutOfRangeException.raise() };
//
//         self.expand();
//
//         int i := *_length;
//         while (i > index)
//         {
//            _array[i] := _array[i - 1];
//
//            i := i - 1
//         };
//
//         _array[i] := object
//      }    
//
//      /// Removes an object at the specified position
//      remove(int index)
//      {
//         if (index >= *_length)
//            { OutOfRangeException.raise() };
//  
//         _length.reduce(1);
//
//         int i := index;
//         int len := *_length;
//         while (i < len)
//         {
//            _array.setAt(i, _array.at(i + 1));
//
//            i := i + 1
//         }
//      }
//
//      removeAt(n)
//         <= remove(cast int(n));
//
//      /// Clears the list
//      clear()
//      {
//         _length.Value := 0
//      }
//
//      trim(int pos)
//      {
//         if (pos > *_length)
//            { OutOfRangeException.raise() };
//
//         _length.Value := pos
//      }
//
//      /// Returns the list enumerator
//      Enumerator enumerator() = new ArrayListEnumerator(self);
//
//      /// Returns the list indexer
//      Indexer indexer() = new ArrayListIndexer(self);
//
//      /// Returns a list copy
//      ArrayList clone()
//         = ArrayList.fromArray(_array, *_length);
//
//      Array cast()
//         = Array.copy(0, *_length, _array);
//
//      get Array Value()
//         = Array.copy(0, *_length, _array);
   }

//   // --- SortedList<TKey,TValue> ---
//
//   sealed class SortedListItem<TKey,TValue>
//   {
//      TKey                         Key  : rprop;
//      TValue                       Item : prop;
//      SortedListItem<TKey, TValue> Next : prop;
//   
//      constructor(TKey key, TValue item)
//      {
//         Key := key;
//         Item := item;
//         Next := nil;
//      }
//   }
   
//   public sealed class SortedList<TKey,TValue>
//   {
//      SortedListItem<TKey, TValue> _top;
//   
//      Func<TKey,TKey,int> _comparator;
//   
//      constructor new()
//   //      <= new((object l,object r => l < r ? -1 : (l == r ? 0 : 1)))
//      {
//      }
//   
//      constructor new(Func<TKey,TKey,int> comparator)
//      {
//         _comparator := comparator
//      }   
//   
//      add(TKey key, TValue item)
//      {
////         auto item := new SortedListItem<TKey,TValue>(key, item);
////   
////         SortedListItem<TKey,TValue> prev := nil;
////         SortedListItem<TKey,TValue> current := _top;
////         while (current != nil && _comparator(current.Key, key) < 0) {
////            prev := current;
////            current := current.Next;
////         };
////   
////         if (prev == nil) {
////            item.Next := _top;
////            _top := item;
////         }
////         else {
////            item.Next := prev;
////            prev.Next := item;
////         }
//      }
//   
//      private /*SortedListItem<TKey,TValue>*/object retrieve(int index)
//      {
////         int n := index;
////         SortedListItem<TKey,TValue> current := _top;
////         while (n > 1) {
////            current := current.Next;
////            n--;
////         };
////   
////         ^ current
//      }
//   
//      TValue at(int index)
//      {
////         SortedListItem<TKey,TValue> current := retrieve(index);
////   
////         ^ current?.Item;
//      }   
//   
//      setAt(int index, TValue item)
//      {
////         SortedListItem current := retrieve(index);
////         
////         current.Item := item;
//      }
//   }
}