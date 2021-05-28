namespace collections
{
    // --- listof ---
    
    public sealed List<T> : Enumerable<T>
    {
        T[]      theArray;
        ref<int> theLength;
        ref<int> theCapacity;
    
        constructor(T[] a)
        {
            int len := a.Length;
            int size := len / 32;
            size := size + 1;
            size := size * 32;
            
            theLength := len;
            theCapacity := size;
            theArray := new T[](size);
            
            for(int i := 0, i < len, i += 1)
            {
                theArray[i] := a[i]
            }
        }
            
        constructor(Enumerator<T> e)
        {
            theCapacity := 16;
            theLength := 0;
            theArray := new T[](16);
            
            self.appendRange(e)        
        }
        
        constructor(int capacity)
        {
            theCapacity := capacity;
            theLength := 0;
            theArray := new T[](capacity);
        }
        
        constructor()
        {
            theCapacity := 16;
            theLength := 0;
            theArray := new T[](16);
        }
        
        constructor load(T[] a, int index, int len)
        {
            int size := len / 32;
            size := size + 1;
            size := size * 32;
            
            theLength := len;
            theCapacity := size;
            theArray := new T[](size);
            
            for(int i := 0, i < len, i += 1)
            {
                int j := i + index;
                
                theArray[i] := a[j]
            }
        }
            
        int Length
        {
            get()
            {
                int retVal := theLength.Value;
                
                ^ retVal
            }        
        }
        
        private expand()
        {
            int cap := theCapacity.Value;
            int len := theLength.Value;
            
            if (cap <= len)
            {
                theCapacity.append(32);
                cap += 32;
                
                T[] newArray := new T[](cap);
    
                for(int i := 0, i < len, i += 1)
                {
                    //newArray[i] := theArray[i]
                    T tmp := theArray[i];
                    newArray[i] := tmp
                };
                
                theArray := newArray
            }
        }
        
        /// Returns the object at the specified position
        T at(int index)
        {
            int len := theLength.Value;
            
            ifnot(index < len)
                { OutOfRangeException.raise() };
                
            ^ theArray[index]
        }
    
        /// Returns the object at the specified position
        setAt(int index, T object)
        {
            int len := theLength.Value;
            
            ifnot(index < len)
                { OutOfRangeException.raise() };
            
            theArray[index] := object
        }
    
        /// Appends a new object to the end of the list
        append(T object)
        {
            self.expand();
            
            int index := theLength.Value;
            theArray[index] := object;
            
            theLength.append(1)
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
            theLength.append(1);
            
            self.expand();
            
            int len := theLength.Value;
            for (int i := len, i > index, i -= 1)
            {
                theArray[i] := theArray[i - 1]
            };
            
            theArray[index] := object
        }
        
        insert(T object)
            <= insert(0, object);
    
        /// Removes an object at the specified position
        remove(int index)
        {
            ifnot (index < theLength)
                { OutOfRangeException.raise() };
            
            theLength.reduce(1);
            
            int i := index;
            while(i < theLength)
            {
                theArray[i] := theArray[i + 1];
                
                i := i + 1
            }
        }
            
        /// Returns the top object
        T First = theArray.at(0);
        
        /// Clears the list
        clear()
        {
            theLength.Value := 0        
        }
        
        trim(int pos)
        {
            if (pos > (cast int(theLength)))
                { OutOfRangeException.raise() };
            
            theLength.Value := pos
        }
        
        Enumerator<T> enumerator() = new BaseEnumerator<T>
        {
            this theIndex := -1;
            
            this theLength := self.Length;
            
            get T Value() = self.at(this theIndex.Value);
            
            enumerable() = self;
            
            reset()
            {
                this theIndex := -1
            }
        };
    
        Indexer<T> indexer() = new BaseIndexer<T>
        {
            this theIndex := 0;
            
            this theLength := self.Length;
            
            get T Value()
                = self.at(theIndex);
                
            set Value(T o)
            {
                self.setAt(theIndex, o)
            }
        };
            
        get T[] Value()
        {
            int len := cast int(theLength);
            T[] copy := new T[](len);
            
            for (int i := 0, i < len, i += 1)
            {
                copy[i] := theArray[i]
            };
            
            ^ copy
        }
    
        clone()
            = class List<T>.load(theArray, 0, theLength.Value);
    }
    
    // --- ListItem ---
    
    /// A list item
    sealed class ListItem
    {
        object   theContent;
        ListItem theNext;
        
        /// Creates a new item
        constructor new(object)
        {
            theContent := object
        }
    
        /// Returns the item object
        get() = theContent;
                
        /// Sets the item object
        set(object)
        {
            theContent := object
        }
    
        ListItem NextItem
        {
            /// Returns the next item
            get() = theNext;
          
            /// Sets the next item  
            set(next)
            {
                theNext := next
            }
        }
    }
    
    // --- ListEnumerator ---
    
    /// A list enumerator
    class ListEnumerator : Enumerator
    {
        List     theList;
        ListItem theCurrent;
        
        /// Creates a list enumerator
        constructor(List list)
        {
            theList := list
        }
    
        /// Returns the current object
        get Value() = theCurrent.get();
        
        /// Resets the enumerator
        reset()
        {
            theCurrent := nil
        }
    
        /// Goes to the next enumeration member and returns true if it is not the last member
        bool next()
        {
            if (nil == theCurrent)
            { 
                theCurrent := theList.TopItem 
            }
            else
            { 
                theCurrent := theCurrent.NextItem 
            };
                
            ^ nil != theCurrent
        }
        
        enumerable() = theList;
    }
    
    // --- ListIndexer ---
    
    /// A list indexer
    class ListIndexer : Indexer
    {
        List     theList;
        ListItem theCurrent;
        ref<int> theIndex;
        
        /// Creates a list indexer
        constructor(List list)
        {
            theList := list;
            theCurrent := list.TopItem;
            theIndex := 0;
        }
    
        /// Returns false if the index out of range
        get bool Available() = nil != theCurrent;
   
        /// Returns the current member
        get Value() = theCurrent.get();
        
        /// Sets the current member
        set Value(o)
        {
            theCurrent.set(o)
        }
        
        /// Reads the current index into an output parameter
        get int Index()
        {
            ^ theIndex.Value
        }
        
        set int Index(int n)
        {
            theIndex.Value := n
        }
        
        appendIndex(int offs)
        {
            int i := theIndex.Value;
            i := i + offs;
            
            self.Index := i
        }
        
        get int Length()
        {
            ^ theList.Length
        }
    }
    
    // --- List ---
    
    /// A list object
    public sealed class List : Enumerable
    {
        ref<int> theCount;
        ListItem theTop;
        ListItem theLast;
    
        /// Creates a list
        constructor()
        {
            theCount := 0
        }
    
        constructor(Array l)
        {
            theCount := 0;
            
            auto it := l.enumerator();
            while (it.next())
            {
                self.append(*it)
            }
        }
    
        /// Returns the top item
        internal ListItem TopItem = theTop;
    
        append(object)
        {
            theCount.append(1);
            if(nil == theTop)
            {
                theTop := ListItem.new(object);
                theLast := theTop
            }
            else
            {
                auto newItem := ListItem.new(object);
                
                theLast.NextItem := newItem;
                theLast := newItem
            }
        }
    
        insert(object)
        {
            theCount.append(1);
            
            auto newItem := ListItem.new(object);
            newItem.NextItem := theTop;
            theTop := newItem
        }
        
        /// Returns the list item at the specified position
        internal ListItem getItemAt(int index)
        {
            int      currentIndex := index;
            ListItem item := theTop;
            
            while (currentIndex > 0)
            {
                item := item.NextItem;
                if (nil == item)
                    { OutOfRangeException.raise() };
                
                currentIndex := currentIndex - 1
            };
            
            ^ item
        }
    
        /// Returns the object at the specified position
        at(int index)
            = self.getItemAt(index).get();
    
        /// Returns the object at the specified position
        setAt(int index, object object)
            = self.getItemAt(index).set(object);
    
        /// Inserts a new object to the specified position
        insert(int index, object object)
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
    
        /// Removes an object at the specified position
        remove(int index)
        {
            if (nil == theTop)
                { OutOfRangeException.raise() };
            
            if (0 == index)
            {
                theTop := theTop.NextItem;
                if (nil == theTop)
                    { theLast := nil }
            }
            else
            {
                ListItem previous := self.getItemAt(index - 1);
                ListItem next := previous.NextItem;
                
                previous.NextItem := next.NextItem;
                
                if(next.equalReference(theLast))
                    { theLast := previous }
            };
                
            theCount.reduce(1)
        }
            
        removeAt(index)
            <= remove(cast int(index));
    
        int Length
        {
            /// Reads the number of list item to the output parameter
            get()
            {
                int retVal := theCount.Value; 
                
                ^ retVal
            }
        }
        
        /// Clears the list
        clear() 
        {
            theCount.Value := 0;
            
            theTop := nil;
            theLast := nil;
        }
    
        /// Returns the list enumerator
        Enumerator enumerator() = new ListEnumerator(self);
    
        /// Returns the list indexer
        Indexer indexer() = new ListIndexer(self);
    
        /// Clones the list
        List clone() 
        {
            auto list := new List();
            
            ListItem item := theTop;        
            while (nil != item)
            {
                list.append(item.get());
                
                item := item.NextItem
            };
            
            ^ list
        }
    }

    // --- ArrayList ---
    
    /// An array list object
    public sealed class ArrayList : Indexable
    {
        Array    theArray;
        ref<int> theLength;
        ref<int> theCapacity;
    
        /// Creates the empty list
        constructor()
        {
            theCapacity := 0;
            theLength := 0;
        }
    
        constructor allocate(int capacity)
        {
            theCapacity := capacity;
            theArray := Array.allocate(capacity);
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
            
            theCapacity := capacity;
            theLength := length;
            
            theArray := array.clone()
        }
        
        int Length
        {
            /// Reads the number of list item to the output parameter
            get()
            {
                int retVal := theLength.Value;
                
                ^ retVal
            }
        }
    
        /// Returns the object at the specified position
        at(int index)
        {
            ifnot ((cast int(theLength)) > index)
                { OutOfRangeException.raise() };
    
            ^ theArray[index]
        }
        
        /// Returns the object at the specified position
        setAt(int index, object o)
        {
            ifnot (cast int(theLength) > index)
                { OutOfRangeException.raise() };
            
            theArray.setAt(index, o)
        }
        
        private expand()
        {
            if (theCapacity <= theLength)
            {
                theCapacity.append(32);
                
                int capacity := theCapacity.Value;
                Array newArray := Array.allocate(capacity);
                for(IntNumber i := 0, i < theLength, i := i + 1)
                {
                    newArray[i] := theArray[i];
                };
                
                theArray := newArray
            }
        }
        
        /// Appends a new object to the end of the list
        append(item)
        {
            self.expand();
            
            int len := theLength.Value;
            theArray[len] := item;
            
            theLength.append(1)
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
            if (index >= theLength)
                { OutOfRangeException.raise() };
            
            self.expand();
            
            int i := theLength;
            while (i > index)
            {
                theArray[i] := theArray[i - 1];
                
                i := i - 1
            };
            
            theArray[i] := object
        }    
            
        /// Removes an object at the specified position
        remove(int index)
        {
            if (index >= (cast int(theLength)))
                { OutOfRangeException.raise() };
            
            theLength.reduce(1);
            
            int i := index;
            int len := theLength.Value;
            while (i < len)
            {
                theArray.setAt(i, theArray.at(i + 1));
                
                i := i + 1
            }
        }
            
        removeAt(n)
            <= remove(cast int(n));
        
        /// Clears the list
        clear()
        {
            theLength.Value := 0
        }
        
        trim(int pos)
        {
            if (pos > (cast int(theLength)))
                { OutOfRangeException.raise() };
            
            theLength.Value := pos
        }
        
        /// Returns the list enumerator
        Enumerator enumerator() = new ArrayListEnumerator(self);
        
        /// Returns the list indexer9
        Indexer indexer() = new ArrayListIndexer(self);
        
        /// Returns a list copy
        ArrayList clone()
            = ArrayList.fromArray(theArray, *theLength);
            
        Array cast()
            = Array.copy(0, *theLength, theArray);
    
        get Array Value()
            = Array.copy(0, theLength.Value, theArray);
    }

    // --- ArrayListEnumerator ---
    
    /// A list enumerator
    class ArrayListEnumerator : Enumerator
    {
        ArrayList theList;
        ref<int>  theIndex;
        
        /// Creates a list enumerator
        constructor(ArrayList list)
        {
            theList := list;
            theIndex := -1;
        }
    
        /// Returns the current object
        get Value() = theList.at(cast int(theIndex));
        
        /// Resets the enumerator
        reset()
        {
            theIndex.Value := -1
        }
    
        /// Goes to the next enumeration member and returns true if it is not the last member
        bool next()
        {
            theIndex.append(1);
            
            int len := theList.Length;
            
            ^ len > (cast int(theIndex))
        }
        
        enumerable() = theList;
    }

    // --- ArrayListIndexer ---
    
    /// A list indexer
    public class ArrayListIndexer : Indexer
    {
        ArrayList theList;
        ref<int>  theIndex;
        
        /// Creates a list indexer
        constructor(ArrayList list)
        {
            theList := list;
            theIndex := 0
        }
    
        /// Returns false if the index out of range
        get bool Available()
        {
            int length := theList.Length;
            int index := theIndex.Value;
            
            ^ length > index
        }
    
        /// Returns the current member
        get Value()
        {
            int index := theIndex.Value;
            
            ^ theList.at(index)
        }
        
        /// Sets the current member
        set Value(o)
        {
            int index := theIndex.Value;
            
            theList.setAt(index, o)
        }
        
        int Index
        {
            /// Reads the current index into an output parameter
            get()
            {
                int retVal := theIndex.Value;
                
                ^ retVal
            }
            
            /// Sets the index by specified value
            set(n)
            {
                theIndex.Value := n
            }
        }
        
        appendIndex(int n)
        { 
            theIndex.append(n)
        } 
        
        int Length
        {
            get()
            {
                int retVal := theList.Length;
                
                ^ retVal
            }
        }
    }

    // --- Dictionary ---
    
    /// Dictionary class
    public sealed  class Dictionary //: Enumerable
    {
        ArrayList theList;
        Array     theHashTable;
        int       theHashMask;
        
        object    theDefault;
    
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
            theHashMask := new IntNumber(size);
            
            theHashTable := Array.allocate(hashLength);
            theList := ArrayList.allocate(capacity);
            
            theDefault := default;
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
            int hashCode := code & theHashMask;
            
            var hashItem := theHashTable.at(hashCode);
            if (nil == hashItem)
            {
                hashItem := new int[](5);
                
                hashItem[0] := 1;
                
                theHashTable.setAt(hashCode, hashItem)
            };
            
            int[] backet := /*cast int[](*/hashItem/*)*/;
            int  length := backet[0];
            
            for (int i := 1, i < length, i += 1)
            {
                int itemIndex := backet[i];
                
                var item := theList.at(itemIndex);
                
                if(key.safeEqual(item.Key))
                    { ^ item }
            };
            
            var newItem := KeyValue.new(key, theDefault);
            
            int i := theList.Length;
            
            theList.append:newItem;
            
            // Extend a backet
            int capacity := backet.Length;
            if (capacity == length)
            {
                auto newBacket := new int[](capacity + 5);
                newBacket.write(0, capacity, backet);
    
                backet := newBacket;
                theHashTable.setAt(hashCode, newBacket)
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
            
            hashCode := hashCode & theHashMask;
            
            var hashItem := theHashTable.at(hashCode);
            if (nil == hashItem)
            {
                hashItem := new int[](5);
                
                hashItem[0] := 1;
                
                theHashTable.setAt(hashCode, hashItem)
             };

            int[] backet := /*cast int[](*/hashItem/*)*/;
            int  length := backet[0];
            
            for (int i := 1, i < length, i += 1)
            {
                int itemIndex := backet[i];
                
                var item := theList.at(itemIndex);
                
                if(key.safeEqual(item.Key))
                    { ^ true }
            };
            
            ^ false
        }
    
        /// Returns a dictionary enumerator
        Enumerator enumerator() => theList;
        
        ArrayList Values = theList.clone();
    
        /// Clones a dictionary
        clone()
        {
            var list := Dictionary.new();
    
            Enumerator it := theList.enumerator();
            while (it.next())
            {
                var item := *it;
                
                list[item.Key] := item.Value
            };
            
            ^ list
        }
    
        clear()
        {
            theHashTable := Array.allocate(theHashTable.Length);
            theList.clear()
        }
    }

}