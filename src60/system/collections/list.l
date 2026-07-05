namespace collections
{
   public sealed class ArrayList : Indexable, info("A weak indexable collection")
   {
      Array          _array;
      Reference<int> _length;
      Reference<int> _capacity;

      constructor()
         : info("Creates an empty collection")
      {
         _length := 0;
         _capacity := 0;
      }

      constructor allocate(int capacity)
         : info("Creates an empty collection with preallocated space")
      {
         _capacity := capacity;
         _array := Array.allocate(capacity);
      }

      constructor allocate() : info("Creates an empty collection with preallocated space for 32 items")
         <= allocate(32);

      constructor load(Array a) : info("Copies a weak array <b>a</b> into the collection")
         <= fromArray(a, a.Length);

      constructor load(o) : info("Copies a weak enumerable object <b>o</b> into the collection")
      {
         Enumerator e := o.enumerator();
         while (e.next())
         {
            self.append(*e)
         }
      }

      internal constructor fromArray(Array array, int length) : info("Copies first <b>length</b> elements of a weak array <b>a</b> into the collection")
      {
         int capacity := array.Length;

         _capacity := capacity;
         _length := length;

         _array := array.clone()
      }

      int Length : info("Returns the number of elements in the collection")
      {
         get()
         {
            int retVal := *_length;

            ^ retVal
         }
      }

      at(int index) : info("Returns the element at a position <b>index</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         ^ _array[index]
      }      

      setAt(int index, object? o) : info("Assigns <b>o</b> to the element at the position <b>index</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
      {
         int len := *_length;
         if:not (len > index)
            { OutOfRangeException.raise() };

         _array.setAt(index, o!)
      }

      private expand()
      {
         int cap := *_capacity;
         int len := *_length;
         if (cap <= len)
         {
            _capacity.append(32);

            cap := *_capacity;
            Array newArray := Array.allocate(cap);
            for(int i := 0; i < len; i := i + 1)
            {
               newArray[i] := _array[i];
            };

            _array := newArray
         }
      }

      append(item) : info("Appends a new object to the end of the list")
      {
         self.expand();

         int len := *_length;
         _array[len] := item;

         _length.append(1)
      }

      appendRange(subarray) : info("Appends all emenents of an array <b>subarray</b>")
      {
         if (nil != subarray)
         {
            int len := subarray.Length;
            for(int i := 0; i < len; i += 1)
            {
               self.append(subarray.at(i))
            }
         }
      }

      appendRange(Indexable indexable) : info("Appends all emenents of an <b>indexable</b> object")
      {
         if (nil != indexable)
         {
            Indexer it := indexable.indexer();

            while (it.Available)
            {       
               self.append(*it);

               it.appendIndex(1)
            }
         }
      }

      insert(int index, object object) : info("Inserts an element <b>object</b> to the specified position <b>index</b>")
      {
         if (index >= *_length)
            { OutOfRangeException.raise() };

         self.expand();

         int i := *_length;
         while (i > index)
         {
            _array[i] := _array[i - 1];

            i := i - 1
         };

         _array[i] := object;
         _length.append(1)
      }

      remove(int index) : info("Removes an element at the specified position <b>index</b>")
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

      removeAt(n)
         <= remove(cast int(n));

      clear() : info("Removes all elements of the collection")
      {
         _length.Value := 0
      }

      trim(int pos) : info("Removes the last <b>pos</b> elements of the collection")
      {
         if (pos > *_length)
            { OutOfRangeException.raise() };

         _length.Value := pos
      }

      Enumerator enumerator() : info("Returns the collection enumerator") = new ArrayListEnumerator(self);

      Indexer indexer() : info("Returns the collection indexer") = new ArrayListIndexer(self);

      ArrayList clone() : info("Returns a shallow copy of the collection")
      {
         int len := *_length;
         if (len == 0) {
            ^ new ArrayList()
         };

         ^ ArrayList.fromArray(_array, len)
      }

      Array cast()
         = Array.copy(0, *_length, _array);

      get Array Value() : info("Returns a shallow copy of the collection as a weak array")
         = Array.copy(0, *_length, _array);
   }

   // --- ArrayListEnumerator ---    
   class ArrayListEnumerator : Enumerator, info("A weak collection enumerator")
   {
      ArrayList      _list;
      Reference<int> _index;

      /// Creates a list indexer
      constructor(ArrayList list) : info("Creates an enumerator of <b>list</b> weak collection")
      {
         _list := list;
         _index := -1
      }

      get Value() : info("Returns the current element")
         = _list.at(*_index);
        
      reset() : info("Resets the enumerator")
      {
         _index.Value := -1
      }
    
      bool next() : info("Goes to the next enumeration member and returns true if it is not the last member")
      {
         _index.append(1);
            
         int len := _list.Length;            

         ^ len > *_index
      }
        
      enumerable() : info("returns the enumerating collection")
        = _list;
   }

    // --- ArrayListIndexer ---
    
   public class ArrayListIndexer : Indexer, info("A weak collection indexer")
   {
      ArrayList      _list;
      Reference<int> _index;

      constructor(ArrayList list) : info("Creates an indexer of <b>list</b> weak collection")
      {
         _list := list;
         _index := 0
      }

      get bool Available() : info("Returns true if there is an element with a given index in the corresponding collection<br/>Otherwise returns false")
      {
         int length := _list.Length;
            
         ^ length > *_index
      }
    
      get Value() : info("Returns the indexed element")
      {
         ^ _list.at(*_index)
      }
        
      set Value(o) : info("Set the element at the current index")
      {
         _list.setAt(*_index, o)
      }
        
      int Index : info("Index property")
      {
         get()
         {
            ^ *_index
         }
            
         set(n)
         {
            _index.Value := n
         }
      }
        
      appendIndex(int n) : info("Increases the current index by parameter **index**")
      { 
         _index.append(n)
      } 
        
      int Length : info("Returns the length of the indexed weak collection")
      {
         get()
         {
            int retVal := _list.Length;
                
            ^ retVal
         }
      }

   }

   public sealed  class Dictionary : Enumerable, info("A weak dictionary class")
   {
      ArrayList _list;
      Array     _hashTable;
      int       _hashMask;

      object?   _default;

      constructor new() : info("Creates a dictionary")
         <= new(32, 32, nil);

      constructor new(object? default) : info("Creates a dictionary with the specified default value")
         <= new(32, 32, default);

      constructor new(int capacity, object? default) : info("Creates a dictionary with the specified default value and the preallocated space")
         <= new(capacity, 32, default);

      constructor new(int capacity, int hashLength, object? default) : info("Creates a dictionary with the specified default value and the preallocated space")
      {
         int size := 32;
         while (size < hashLength)
         {
            size := size $shl 1;
         };

         size := size - 1;
         _hashMask := size;

         _hashTable := Array.allocate(hashLength);
         _list := ArrayList.allocate(capacity);

         _default := default;
      }

      setAt(object key, object object) : info("Adds or updates a dictionary element with <b>key</b>")
      {
         int hashCode := key.Hashcode;

         self.getItem(hashCode, key).Value := object
      }

      private getItem(int code, object key)
      {
         int hashCode := code & _hashMask;

         var hashItem := _hashTable.at(hashCode);
         if (nil == hashItem)
         {
            hashItem := new int[](5);

            hashItem[0] := 1;

            _hashTable.setAt(hashCode, hashItem)
         };

         var int[] backet := /*cast int[](*/hashItem/*)*/;
         int  length := backet[0];

         for (int i := 1; i < length; i += 1)
         {
            int itemIndex := backet[i];

            var item := _list.at(itemIndex);

            if(key.safeEqual(item.Key))
               { ^ item }
         };

         var newItem := KeyValue.new(key, _default);

         int i := _list.Length;

         _list.append(newItem);

         // Extend a backet
         int capacity := backet.Length;
         if (capacity == length)
         {
            auto newBacket := new int[](capacity + 5);
            Array<int>.copy(newBacket, backet, 0, capacity);

            backet := newBacket;
            _hashTable.setAt(hashCode, newBacket)
         };

         backet[length] := i;

         length := length + 1;
         backet[0] := length;

         ^ newItem
      }    

      at(object key) : info("Returns a dictionary element with <b>key</b>")
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      bool containsKey(object key) : info("Returns true if the dictionary element with <b>key</b> exists in the dictionary")
      {
         int hashCode := key.Hashcode;

         hashCode := hashCode & _hashMask;

         var hashItem := _hashTable.at(hashCode);
         if (nil == hashItem)
            { ^ false };

         var int[] backet := hashItem;
         int  length := backet[0];

         for (int i := 1; i < length; i += 1)
         {
            int itemIndex := backet[i];

            var item := _list.at(itemIndex);

            if(key.safeEqual(item.Key))
               { ^ true }
         };

         ^ false
      }

      Enumerator enumerator() : info("Returns a dictionary enumerator") => _list;

      ArrayList Values : info("Returns an array of key-value pairs") = _list.clone();

      clone() : info("Makes a shallow copy a dictionary")
      {
         var list := Dictionary.new();

         Enumerator it := _list.enumerator();
         while (it.next())
         {
            var item := *it;

            list[item.Key] := item.Value
         };

         ^ list
      }

      clear() : info("Clears the dictionary content")
      {
         _hashTable := Array.allocate(_hashTable.Length);
         _list.clear()
      }
   }

   // --- List ---    

   sealed class ListItem
   {
      object    _content;
      ListItem? _next;
        
      /// Creates a new item
      constructor new(object) 
      {
         _content := object
      }
    
      /// Returns the item object
      get() = _content;
                
      /// Sets the item object
      set(object)
      {
         _content := object
      }
    
      ListItem? NextItem
      {
         /// Returns the next item
         get() = _next;
          
         /// Sets the next item  
         set(next)
         {
            _next := next
         }
      }
   }

   public sealed class List : Enumerable,
         info("A list of objects")
   {
      Reference<int> _count;
      ListItem?      _top;
      ListItem?      _last;
    
      constructor() : info("Creates a weak linked list")
      {
         _count := 0
      }
    
      constructor(Array l) : info("Copies an array")
      {
         _count := 0;
            
         auto it := l.enumerator();
         while (it.next())
         {
            self.append(*it)
         }
      }
    
      /// Returns the top item
      internal ListItem? TopItem = _top;
    
      append(object) : info("Appends an element")
      {
         _count.append(1);
         if(nil == _top)
         {
            _top := ListItem.new(object);
            _last := _top
         }
         else
         {
            auto newItem := ListItem.new(object);
                
            _last!.NextItem := newItem;
            _last:= newItem
         }
      }
    
      insert(object) : info("Inserts an element at the list top")
      {
         _count.append(1);
            
         auto newItem := ListItem.new(object);
         newItem.NextItem := _top;
         _top := newItem
      }
        
      internal ListItem getItemAt(int index)
      {
         int       currentIndex := index;
         ListItem? item := _top;
            
         while (currentIndex > 0)
         {
            item := item!.NextItem;
            if (nil == item)
               { OutOfRangeException.raise() };
                
            currentIndex := currentIndex - 1
         };
            
         ^ item!
      }
    
      at(int index) : info("Returns the element at the specified position")
         = self.getItemAt(index).get();
    
      setAt(int index, object object) : info("Sets the element at the specified position")
         = self.getItemAt(index).set(object);
    
      insert(int index, object object) : info("Inserts a new object to the specified position")
      {
         if (index == 0)
         { 
            self.insert(object)
         }
         else {
            auto newItem := ListItem.new(object);
            auto prevItem := self.getItemAt(index);
            newItem.NextItem := prevItem.NextItem;
            prevItem.NextItem := newItem;
         }
      }
    
      remove(int index) : info("Removes an object at the specified position")
      {
         if (nil == _top)
            { OutOfRangeException.raise() };
            
         if (0 == index)
         {
            _top := _top!.NextItem;
            if (nil == _top)
               { _last := nil }
         }
         else
         {
            ListItem previous := self.getItemAt(index - 1);
            ListItem? next := previous.NextItem;
                
            previous.NextItem := next!.NextItem;
                
            if(next!.equalReference(_last))
               { _last := previous }
         };
                
         _count.reduce(1)
      }
            
      removeAt(index) : info("Removes an object at the specified position")
         <= remove(cast int(index));
    
      int Length : info("Returns the list length")
      {
         get()
         {
            int retVal := *_count; 
                
            ^ retVal
         }
      }
        
      clear() : info("Removes all list elements")
      {
         _count.Value := 0;
            
         _top := nil;
         _last := nil;
      }
    
      Enumerator enumerator() : info("Returns the list enumerator")
         = new ListEnumerator(self);
    
      Indexer indexer() : info("Returns the list indexer")
         = new ListIndexer(self);
    
      List clone() : info("Creates a shallow copy of the list")
      {
         auto list := new List();
            
         ListItem? item := _top;        
         while (nil != item)
         {
            list.append(item.get());
                
            item := item.NextItem
         };
            
         ^ list
      }
   }

   class ListEnumerator : Enumerator, info("A list enumerator")
   {
      List      _list;
      ListItem? _current;
        
      constructor(List list) : info("Creates a list enumerator")
      {
         _list := list
      }
    
      get Value() : info("Returns the current object") = _current?.get();
        
      reset() : info("Resets the enumerator")
      {
         _current := nil
      }
    
      bool next() : info("Goes to the next enumeration member and returns true if it is not the last member")
      {
         if (nil == _current)
         { 
            _current := _list.TopItem 
         }
         else
         { 
            _current := _current.NextItem 
         };
                
         ^ nil != _current
      }
        
      enumerable() : info("Returns an enumerable list") = _list;
   }
    
   // --- ListIndexer ---
    
   class ListIndexer : Indexer, info("A list indexer")
   {
      List           _list;
      ListItem?      _current;
      Reference<int> _index;
        
      constructor(List list) : info("Creates a list indexer")
      {
         _list := list;
         _current := list.TopItem;
         _index := 0;
      }
    
      get bool Available() : info("Returns false if the index out of range") = nil != _current;
   
      get Value() : info("Returns the current member") = _current?.get();
        
      set Value(o) : info("Sets the current member")
      {
         _current!.set(o)
      }
        
      get int Index() : info("Returns the current index")
      {
         ^ _index.Value
      }
        
      set Index(int n) : info("Sets the current index")
      {
         _index.Value := n
      }
        
      appendIndex(int offs) : info("Increases the current index by parameter **index**")
      {
         int i := _index.Value;
         i := i + offs;
            
         self.Index := i
      }
        
      get int Length() : info("Returns the indexed list length")
      {
         ^ _list.Length
      }
   }

   // --- List ---
   public sealed List<T> : Enumerable<T>, info("A strong-typed list")
   {
      T[]            _array;
      Reference<int> _length;
      Reference<int> _capacity;

      constructor(T[] a) : info("Copies a strong-typed array")
      {
         int len := a.Length;
         int size := len / 32;
         size := size + 1;
         size := size * 32;

         _length := len;
         _capacity := size;
         _array := new T[](size);

         for(int i := 0; i < len; i += 1)
         {
            _array[i] := a[i]
         }
      }
            
      constructor(Enumerator<T> e) : info("Copies a strong-typed enumerator")
      {
         _capacity := 16;
         _length := 0;
         _array := new T[](16);
            
         self.appendRange(e)        
      }
        
      constructor(int capacity) : info("Creates an empty list with the preallocated space")
      {
         _capacity := capacity;
         _length := 0;
         _array := new T[](capacity);
      }
        
      constructor() : info("Creates an empty list")
      {
         _capacity := 16;
         _length := 0;
         _array := new T[](16);
      }
        
      constructor load(source)
         <= load(cast Enumerator<T>(source));

      constructor load(Enumerator<T> e) : info("Copies a strong-typed enumerator")
      {
         _capacity := 16;
         _length := 0;
         _array := new T[](16);
            
         self.appendRange(e)        
      }       

      constructor load(T[] a, int index, int len) : info("Copies a strong-typed sub array <b>a</b> from <b>index</b> element, with the provided length <b>len</b>")
      {
         int size := len / 32;
         size := size + 1;
         size := size * 32;

         _length := len;
         _capacity := size;
         _array := new T[](size);

         for(int i := 0; i < len; i += 1)
         {
            int j := i + index;

            _array[i] := a[j]
         }
      }
            
      int Length : info("Returns the list length")
      {
         get()
         {
            int retVal := *_length;
                
            ^ retVal
         }        
      }

      get bool Empty() : info("Returns true if the list is empty")
         = *_length == 0;
        
      private expand()
      {
         int cap := *_capacity;
         int len := *_length;

         if (cap <= len)
         {
            _capacity.append(32);
            cap := *_capacity;

            var T[] newArray := new T[](cap);

            for(int i := 0; i < len; i += 1)
            {
               T tmp := _array[i];
               newArray[i] := tmp
            };

            _array := newArray
         }
      }
        
      T at(int index) : info("Returns the object at the specified position")
      {
         int len := *_length;
            
         if:not(index < len)
            { OutOfRangeException.raise() };
                
         ^ _array[index]
      }
    
      setAt(int index, T object) : info("Sets the element at the specified position")
      {
         int len := *_length;
            
         if:not(index < len)
            { OutOfRangeException.raise() };
            
         _array[index] := object
      }
    
      append(T object) : info("Appends a new object to the end of the list")
      {
         self.expand();
            
         int index := *_length;
         _array[index] := object;
            
         _length.append(1)
      }
    
      appendRange(Enumerator<T> e) : info("Copies an enumerable collection")
      {
         while(e.next())
         {
            self.append(*e)
         }        
      }
    
      insert(int index, T object) : info("Inserts the element at the specified index")
      {
         self.expand();

         int len := *_length;
         for (int i := len; i > index; i -= 1)
         {
            _array[i] := _array[i - 1]
         };

         _array[index] := object;
         _length.append(1)
      }
        
      insert(T object) : info("Inserts the element at the top")
         <= insert(0, object);
    
      remove(int index) : info("Removes an object at the specified position")
      {
         if:not (index < *_length)
            { OutOfRangeException.raise() };
            
         _length.reduce(1);
            
         int i := index;
         int len := *_length;
         while(i < len)
         {
            _array[i] := _array[i + 1];
                
            i := i + 1
         }
      }
            
      T First : info("Returns the first elemenet of the list") = _array.at(0);
        
      T Last : info("Returns the last elemenet of the list") = _array.at(*_length - 1);
        
      clear() : info("Removes all elemements")
      {
         _length.Value := 0        
      }
        
      trim(int pos) : info("Trims the list at the specified position")
      {
         if (pos > *_length)
            { OutOfRangeException.raise() };
            
         _length.Value := pos
      }
        
      Enumerator<T> enumerator() : info("Returns a list enumerator") = new BaseEnumerator<T>
      {
         this index := -1;
            
         this length := self.Length;
            
         get T Value() = self.at(this index.Value);
            
         enumerable() = self;
            
         reset()
         {
            this index.Value := -1
         }
      };
    
      Indexer<T> indexer() : info("Returns a list indexer") = new BaseIndexer<T>
      {
         this index := 0;
            
         this length := self.Length;
            
         get T Value()
            = self.at(this index.Value);
                
         set Value(T o)
         {
            self.setAt(this index.Value, o)
         }
      };
            
      get T[] Value() : info("Returns a shallow copy of the list as a strong-typed array")
      {
         int len := *_length;
         var T[] copy := new T[](len);
            
         for (int i := 0; i < len; i += 1)
         {
            copy[i] := _array[i]
         };
            
         ^ copy
      }
    
      clone() : info("Returns a shallow copy of the list")
         = class List<T>.load(_array, 0, *_length);
   }
}
