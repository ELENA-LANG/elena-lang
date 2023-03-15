namespace collections
{
   public sealed class ArrayList : Indexable
   {
      Array          _array;
      Reference<int> _length;
      Reference<int> _capacity;

      constructor()
      {
         _length := 0;
         _capacity := 0;
      }

      constructor allocate(int capacity)
      {
         _capacity := capacity;
         _array := Array.allocate(capacity);
      }

      constructor allocate()
         <= allocate(32);

      constructor load(Array a)
         <= fromArray(a, a.Length);

      constructor load(o)
      {
         Enumerator e := o.enumerator();
         while (e.next())
         {
            self.append(*e)
         }
      }

      /// Creates the list
      internal constructor fromArray(Array array, int length)
      {
         int capacity := array.Length;

         _capacity := capacity;
         _length := length;

         _array := array.clone()
      }

      int Length
      {
         /// Reads the number of list item to the output parameter
         get()
         {
            int retVal := *_length;

            ^ retVal
         }
      }

      /// Returns the object at the specified position
      at(int index)
      {
         int len := *_length;
         ifnot (len > index)
            { OutOfRangeException.raise() };

         ^ _array[index]
      }      

      /// Returns the object at the specified position
      setAt(int index, object o)
      {
         int len := *_length;
         ifnot (len > index)
            { OutOfRangeException.raise() };

         _array.setAt(index, o)
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
            for(int i := 0, i < len, i := i + 1)
            {
               newArray[i] := _array[i];
            };

            _array := newArray
         }
      }

      /// Appends a new object to the end of the list
      append(item)
      {
         self.expand();

         int len := *_length;
         _array[len] := item;

         _length.append(1)
      }

      appendRange(subarray)
      {
         if (nil != subarray)
         {
            int len := subarray.Length;
            for(int i := 0, i < len, i += 1)
            {
               self.append(subarray.at(i))
            }
         }
      }

      appendRange(Indexable indexable)
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

      /// Inserts a new object to the specified position
      insert(int index, object object)
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

         _array[i] := object
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

      removeAt(n)
         <= remove(cast int(n));

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
      Enumerator enumerator() = new ArrayListEnumerator(self);

      /// Returns the list indexer
      Indexer indexer() = new ArrayListIndexer(self);

      /// Returns a list copy
      ArrayList clone()
         = ArrayList.fromArray(_array, *_length);

      Array cast()
         = Array.copy(0, *_length, _array);

      get Array Value()
         = Array.copy(0, *_length, _array);
   }

   // --- ArrayListEnumerator ---
    
   /// A list enumerator
   class ArrayListEnumerator : Enumerator
   {
      ArrayList      _list;
      Reference<int> _index;

      /// Creates a list indexer
      constructor(ArrayList list)
      {
         _list := list;
         _index := -1
      }

      /// Returns the current object
      get Value() = _list.at(*_index);
        
      /// Resets the enumerator
      reset()
      {
         _index.Value := -1
      }
    
      /// Goes to the next enumeration member and returns true if it is not the last member
      bool next()
      {
         _index.append(1);
            
         int len := _list.Length;            

         ^ len > *_index
      }
        
      enumerable() = _list;
   }

    // --- ArrayListIndexer ---
    
    /// A list indexer
   public class ArrayListIndexer : Indexer
   {
      ArrayList      _list;
      Reference<int> _index;

      /// Creates a list indexer
      constructor(ArrayList list)
      {
         _list := list;
         _index := 0
      }

      /// Returns false if the index out of range
      get bool Available()
      {
         int length := _list.Length;
            
         ^ length > *_index
      }
    
      /// Returns the current member
      get Value()
      {
         ^ _list.at(*_index)
      }
        
      /// Sets the current member
      set Value(o)
      {
         _list.setAt(*_index, o)
      }
        
      int Index
      {
         /// Reads the current index into an output parameter
         get()
         {
            ^ *_index
         }
            
         /// Sets the index by specified value
         set(n)
         {
            _index.Value := n
         }
      }
        
      appendIndex(int n)
      { 
         _index.append(n)
      } 
        
      int Length
      {
         get()
         {
            int retVal := _list.Length;
                
            ^ retVal
         }
      }

   }

   /// Dictionary class
   public sealed  class Dictionary : Enumerable
   {
      ArrayList _list;
      Array     _hashTable;
      int       _hashMask;

      object    _default;

      /// Creates a dictionary
      constructor new()
         <= new(32, 32, nil);

      constructor new(object default)
         <= new(32, 32, default);

      constructor new(int capacity, object default)
         <= new(capacity, 32, default);

      /// Creates a dictionary
      constructor new(int capacity, int hashLength, object default)
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

      setAt(int key, object object)
      {
         int hashCode := key.Hashcode;

         self.getItem(hashCode, key).Value := object
      }

      setAt(long key, object object)
      {
         int hashCode := key.Hashcode;

         self.getItem(hashCode, key).Value := object
      }

      setAt(real key, object object)
      {
         int hashCode := key.Hashcode;

         self.getItem(hashCode, key).Value := object
      }

      setAt(char key, object object)
      {
         int hashCode := key.Hashcode;

         self.getItem(hashCode, key).Value := object
      }

      setAt(string key, object object)
      {
         int hashCode := key.Hashcode;

         self.getItem(hashCode, key).Value := object
      }

      setAt(wide key, object object)
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

         for (int i := 1, i < length, i += 1)
         {
            int itemIndex := backet[i];

            var item := _list.at(itemIndex);

            if(key.safeEqual(item.Key))
               { ^ item }
         };

         var newItem := KeyValue.new(key, _default);

         int i := _list.Length;

         _list.append:newItem;

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

      at(int key)
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      at(long key)
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      at(real key)
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      at(char key)
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      at(string key)
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      at(wide key)
      {
         int hashCode := key.Hashcode;

         ^ self.getItem(hashCode, key).Value
      }

      bool containsKey(object key)
      {
         int hashCode := key.Hashcode;

         hashCode := hashCode & _hashMask;

         var hashItem := _hashTable.at(hashCode);
         if (nil == hashItem)
         {
            hashItem := new int[](5);

            hashItem[0] := 1;

            _hashTable.setAt(hashCode, hashItem)
         };

         var int[] backet := /*cast int[](*/hashItem/*)*/;
         int  length := backet[0];

         for (int i := 1, i < length, i += 1)
         {
            int itemIndex := backet[i];

            var item := _list.at(itemIndex);

            if(key.safeEqual(item.Key))
               { ^ true }
         };

         ^ false
      }

      /// Returns a dictionary enumerator
      Enumerator enumerator() => _list;

      ArrayList Values = _list.clone();

      /// Clones a dictionary
      clone()
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

      clear()
      {
         _hashTable := Array.allocate(_hashTable.Length);
         _list.clear()
      }
   }

   // --- List ---
   public sealed List<T> : Enumerable<T>
   {
      T[]            _array;
      Reference<int> _length;
      Reference<int> _capacity;
    
      constructor(T[] a)
      {
         int len := a.Length;
         int size := len / 32;
         size := size + 1;
         size := size * 32;
            
         _length := len;
         _capacity := size;
         _array := new T[](size);
            
         for(int i := 0, i < len, i += 1)
         {
            _array[i] := a[i]
         }
      }
            
      constructor(Enumerator<T> e)
      {
         _capacity := 16;
         _length := 0;
         _array := new T[](16);
            
         self.appendRange(e)        
      }
        
      constructor(int capacity)
      {
         _capacity := capacity;
         _length := 0;
         _array := new T[](capacity);
      }
        
      constructor()
      {
         _capacity := 16;
         _length := 0;
         _array := new T[](16);
      }
        
      constructor load(T[] a, int index, int len)
      {
         int size := len / 32;
         size := size + 1;
         size := size * 32;
            
         _length := len;
         _capacity := size;
         _array := new T[](size);
            
         for(int i := 0, i < len, i += 1)
         {
            int j := i + index;
                
            _array[i] := a[j]
         }
      }
            
      int Length
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
            cap += 32;
                
            var T[] newArray := new T[](cap);
    
            for(int i := 0, i < len, i += 1)
            {
               //newArray[i] := theArray[i]
               T tmp := _array[i];
               newArray[i] := tmp
            };
                
            _array := newArray
         }
      }
        
      /// Returns the object at the specified position
      T at(int index)
      {
         int len := *_length;
            
         ifnot(index < len)
            { OutOfRangeException.raise() };
                
         ^ _array[index]
      }
    
      /// Returns the object at the specified position
      setAt(int index, T object)
      {
         int len := *_length;
            
         ifnot(index < len)
            { OutOfRangeException.raise() };
            
         _array[index] := object
      }
    
      /// Appends a new object to the end of the list
      append(T object)
      {
         self.expand();
            
         int index := *_length;
         _array[index] := object;
            
         _length.append(1)
      }
    
      append(object)
         <= append(cast T(object));
    
      appendRange(Enumerator<T> e)
      {
         while(e.next())
         {
            self.append(*e)
         }        
      }
    
      insert(int index, T object)
      {
         _length.append(1);
            
         self.expand();
            
         int len := *_length;
         for (int i := len, i > index, i -= 1)
         {
            _array[i] := _array[i - 1]
         };
            
         _array[index] := object
      }
        
      insert(T object)
         <= insert(0, object);
    
      /// Removes an object at the specified position
      remove(int index)
      {
         ifnot (index < *_length)
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
            
      /// Returns the top object
      T First = _array.at(0);
        
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
        
      Enumerator<T> enumerator() = new BaseEnumerator<T>
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
    
      Indexer<T> indexer() = new BaseIndexer<T>
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
            
      get T[] Value()
      {
         int len := *_length;
         var T[] copy := new T[](len);
            
         for (int i := 0, i < len, i += 1)
         {
            copy[i] := _array[i]
         };
            
         ^ copy
      }
    
      clone()
         = class List<T>.load(_array, 0, *_length);
   }
}
