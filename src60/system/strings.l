// --- Array template ---

public sealed Array<T> : Indexable<T>
{
   embeddable __string T[] array;

   constructor allocate(int length)
      = new T[](length);   

   constructor copy(T[] array, int index, int length)
      <= allocate(length)
   {
      for(int i := 0 , i < length, i += 1)
      {
         this array[i] := array[i + index]
      }
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

   Enumerable cast() = new Enumerable{ Enumerator enumerator() => self; };

   Indexer<T> indexer() = new BaseIndexer<T>
   {
      this index := 0;
        
      this length := self.Length;
        
      get T Value()
         = self.at(this index);
            
      set Value(T o)
      {
         self.setAt(this index, o)
      }
   };   

   T at(int n)
   {
      if (n < 0)
         { system'InvalidArgumentException.raise() };
        
      int len := $len array;

      if (n < len)
      { 
         ^ array[n] 
      }
      else
      {
         system'InvalidArgumentException.raise();
            
         ^ nil // !! never reached
      }
   }

   setAt(int n, T item)
   {
      if (n < 0)
         { system'InvalidArgumentException.raise() };
        
      int len := $len array;

      if (n < len)
      {
          array[n] := item
      }
      else
      {
         system'InvalidArgumentException.raise()
      }
   }

   setAt(n, item)
      <= setAt(cast int(n), cast T(item));

   at(n)
      <= at(cast int(n));

   int Length
   {
      get()
      {
         int len := $len array;
            
         ^ len
      }
   }

   T[] add(T[] a)
      <= add(a, a.Length);
                              
   T[] add(T[] a, int length)
   {
      int length1 := self.Length;
      int length2 := length;
        
      auto retVal := new T[](length1 + length2);

      for (int i:=0, i < length1, i += 1) {
         retVal[i] := array[i]
      };

      for (int i:=0, i < length2, i += 1) {
         retVal[length1 + i] := a[i]
      };
            
      ^ retVal
   }
   
}

// --- Variadic Array template ---

public sealed nonstructural VariadicArray<T> //: Indexable<T>
{
   embeddable __string T[] array;

   private readArgLength(ref int retVal) : external(system'core_routines'__varg_readLength);

   int Length
   {
      get()
      {
         self.readArgLength(ref int len);
            
         ^ len
      }
   }   
}

// --- String ---

public const struct String : BaseValue, 
   info("A UTF-8 literal value")
{
   __string byte[] array;

   const static string MinValue
      = "";

   constructor allocate(int size)
      = new byte[](size + 1);

    /// Creates the literal string with specified length and copies the parameter value
   constructor copy(int size, string s)
      <= allocate(size)
   {
      PrimitiveStringOperations.copy(array, 0, size, s);

      // add tailing zero
      byte z := 0;
      array[size] := z;
   }

   /// Creates the literal string with specified length fills with specified character value        
   constructor fill(int size, char ch)
      <= allocate(ch.Length * size)
   {
      byte buffer[4];
      int chVal := ch.Value;
      int  length := ch.Length;
      PrimitiveOperations.copyUTF32(buffer, 0, length);
        
      int offs := 0;
      for (int i := 0, i < size, i := i + 1)
      {
         for (int j := 0, j < length, j := j + 1) {
            array[offs] := buffer[j];
            offs := offs + 1;
         }
      }
   }

   /// Creates the literal string copy              
   constructor copy(string s)
      <= copy(s.Length, s);

   /// Creates the copy of a subarray        
   constructor copy(int index, int length, char[] charArray)
      <= allocate(PrimitiveOperations.calcSubStringLength(index, length, charArray))
   {
      PrimitiveOperations.copy(this array, index, length, charArray);
   }

   internal constructor fromByteArray(int index, int size, byte[] buffer)
      <= allocate(size)
   {
      PrimitiveOperations.copy(array, index, size, buffer);

      // add tailing zero
      byte z := 0;
      array[size] := z;
   }

   string toPrintable() = self;

   wide cast()
   {
      int        i := 0;
      int        m := 0;
      int        page := 0;
      int        l := self.Length;
      short buffer[256];            
      if (l < 256)
      {
         m := shortArrayConvertor.convert(buffer, 0, l, self);

         ^ WideString.fromShortArray(0, m, buffer)
      };

      wide retVal := WideString.MinValue;
      while(l > 0)
      {
         page := l;
         if (page > 256)
         {
            page := 256
         };

         m := shortArrayConvertor.convert(buffer, 0, l, self.Substring(i,page));

         retVal := retVal.add(WideString.fromShortArray(0, m, buffer));

         i := i + page;
         l := l - page
      };
        
      ^ retVal      
   }

   Enumerable<char> cast() = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

   int Length
   {
      /// Reads the literal length into an output parameter
      get()
      {
         int l := $len array;
         l := l - 1;

         ^ l;
      }
   }

   bool equal(string s) : external(system'core_routines'__str_equal);

   bool equal(wide s)
      = cast wide(self).equal(s);

   bool equal(char ch)
      <= equal(cast string(ch));

   bool less(string s) : external(system'core_routines'__str_less);

   bool less(wide s)
      = cast wide(self).less(s);

   bool less(char ch)
      <= less(cast string(ch));

   bool equal(o)
   {
      var s := cast string(o) ?? nil;
      (nil == s)
         ? { ^ false } : { ^ self.equal(s) }      
   }

   bool less(o)
      <= less(cast string(o));

   private copySubS(string dest, string sour, int index, int length) : external(system'core_routines'__str_copysubs);
   private copySubS(string dest, string sour, int index, int length, int sourIndex) : external(system'core_routines'__str_xcopysubs);

   char at(int index)
      = PrimitiveOperations.readUTF32(array, index);

   retoverload byte at(int index)
      = array[index];

   string add(o)
      <= add(cast string(o));

   string add(BaseVariable v)
      <= add(*v);

   string add(string subs)
      : info("Concatinates the values")
   {
      int l1 := self.Length;
      if (l1 == 0) {
         ^ subs
      };

      int l2 := subs.Length;
      string retVal := String.allocate(l1 + l2);

      self.copySubS(retVal, self, 0, l1);
      self.copySubS(retVal, subs, l1, l2);    

      ^ retVal
   }

   int indexOf(int index, string literal)
      : info("Saves the index of the first occurence of the specified substring into an output parameter.<br/>If the substring is not found. returns -1")
   {
      int sublen := literal.Length;
      int retVal := PrimitiveStringOperations.indexOf(self, literal, index, sublen);
        
      ^ retVal
   }

   int indexOf(int index, char ch)
   {
      int sublen := self.Length;
      int retVal := PrimitiveStringOperations.indexOf(self, ch, index, sublen);
        
      ^ retVal
   }

   string insert(int index, string s)
      : info("Inserts the substring into and returns the new one")
   {
      int l1 := self.Length;
      int l2 := s.Length;
      string retVal := String.allocate(l1 + l2);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, s, index, l2);    
      self.copySubS(retVal, self, index + l2, l1 - index, index);
    
      ^ retVal;
   }

   string insert(int index, char ch)
      = self.insert(index, cast string(ch));

   string delete(int index, int length)
      : info("Deletes the substring from the object and returns the new one")
   {
      int len1 := self.Length;
      int sindex := index + length;
      string retVal := String.allocate(len1-length);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, self, sindex, len1 - length, index);
    
      ^ retVal;         
   }

   string Substring(int index, int length)
      : info("Returns the substring")
   {
      ^ String.fromByteArray(index, length, array);
   }

   int save(int index, int length, char[] buffer, int destIndex)
      = PrimitiveStringOperations.save(buffer, self, index, length, destIndex);

   internal saveToByteArray(int index, byte[] buffer, int length)
      = PrimitiveStringOperations.save(buffer, this array, index, length);

   Enumerator<char> enumerator()
         : info("Returns the string enumerator")
      = new StringEnumerator(self);

   Indexer<char> indexer() = new StringIndexer(self);
    
   /// Clones the value
   string clone() = String.copy(self);
}

public const string emptyString = String.MinValue;

// --- StringEnumerator ---

sealed class StringEnumerator : Enumerator<char>
{
   string         literal;
   Reference<int> index;
   Reference<int> diff;
   char           char;  

   constructor(string s)
   {
      literal := s;
      index := -1;
      diff := 1;
      char := $0;
   }

   reset()
   {
      index.Value := -1;
      diff.Value := 1
   }

   bool next()
   {
      int len := literal.Length;
        
      index.append(*diff);
        
      int i := *index;
      if (i < len)
      {
         char := literal[i];
                
         diff.Value := char.Length;
                
         ^ true
      };

      diff.Value := 0;
      char := $0;

      ^ false
   }

   Enumerator cast() = new Enumerator
   {
      embeddable dispatch() => self;
   };
   
   enumerable() = literal;

   char Value = char;
}

// --- StringIndexer ---

sealed class StringIndexer : Indexer<char>
{
   string         literal;
   Reference<int> index;

   constructor(string s)
   {
      literal := s;
      index := 0;
   }

   get bool Available()
   {
      int len := literal.Length;
      int val := *index;
        
      ^ val < len
   }
   
   get char Value()
   {
      int val := *index;
      char ch := literal[val];

      ^ ch
   }
   
   set Value(char ch)
   {
      system'NotSupportedException.raise()
   }

   int Index
   {
      /// Reads the current index and saves it into an output parameter
      get()
      {
         int retVal := *index;
            
         ^ retVal
      }
        
      /// Sets the index by specified value
      set(val)
      {
         index.Value := val
      }
   }
   
   appendIndex(int val)
   {
      index.append(val)
   }
    
   int Length
   {
      /// Reads the length to the output parameter    
      get()
      {
         int retVal := literal.Length;
            
         ^ retVal
      }
   }
        
   Indexer cast() = new Indexer{ embeddable dispatch() => self; };   
}

// --- WideString ---

public const struct WideString : BaseValue
{
   __string short[] array;

   const static wide MinValue
      = ""w;

   constructor allocate(int size)
      = new short[](size + 1);

   constructor copy(int size, wide s)
      <= allocate(size)
   {
      PrimitiveWideStringOperations.copy(array, 0, size, s);

      // add tailing zero
      short z := 0;
      array[size] := z;
   }

   /// Creates the literal string with specified length fills with specified character value        
   constructor fill(int size, char ch)
      <= allocate(ch.Length * size)
   {
      short buffer[4];
      int chVal := ch.Value;
      int  length := ch.Length;
      PrimitiveOperations.copyUTF32(buffer, 0, length);
        
      int offs := 0;
      for (int i := 0, i < size, i := i + 1)
      {
         for (int j := 0, j < length, j := j + 1) {
            array[offs] := buffer[j];
            offs := offs + 1;
         }
      }
   }

   /// Creates the literal string copy              
   constructor copy(wide s)
      <= copy(s.Length, s);

   /// Creates the copy of a subarray        
   constructor copy(int index, int length, char[] charArray)
      <= allocate(PrimitiveOperations.calcSubWideStringLength(index, length, charArray))
   {
      PrimitiveOperations.copy(this array, index, length, charArray);
   }

   internal constructor fromShortArray(int index, int size, short[] buffer)
      <= allocate(size)
   {
      PrimitiveOperations.copy(array, index, size, buffer);

      // add tailing zero
      short z := 0;
      array[size] := z;
   }

   string cast()
   {
      int        i := 0;
      int        m := 0;
      int        page := 0;
      int        l := self.Length;
      byte buffer[256];            
      if (l < 128)
      {
         m := byteArrayConvertor.convert(buffer, 0, l, self);

         ^ String.fromByteArray(0, m, buffer)
      };

      string retVal := String.MinValue;
      while(l > 0)
      {
         page := l;
         if (page > 256)
         {
            page := 256
         };

         m := byteArrayConvertor.convert(buffer, 0, l, self.Substring(i,page));

         retVal := retVal.add(String.fromByteArray(0, m, buffer));

         i := i + page;
         l := l - page
      };
        
      ^ retVal      
   }

   Enumerable<char> cast() = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

   string toPrintable() = self;

   bool equal(o)
   {
      var s := cast wide(o) ?? nil;
      (nil == s)
         ? { ^ false } : { ^ self.equal(s) }
   }
   
   bool less(o)
      <= less(cast wide(o));

   bool equal(wide s) : external(system'core_routines'__wstr_equal);

   bool less(wide s) : external(system'core_routines'__wstr_less);

   bool equal(char ch)
      <= equal(cast wide(ch));

   bool less(char ch)
      <= less(cast wide(ch));

   char at(int index)
      = PrimitiveOperations.readUTF32(array, index);

   retoverload short at(int index)
      = array[index];

   wide add(o)
      <= add(cast wide(o));

   wide add(BaseVariable v)
      <= add(*v);

   wide add(wide subs)
      : info("Concatinates the values")
   {
      int l1 := self.Length;
      if (l1 == 0) {
         ^ subs
      };

      int l2 := subs.Length;
      string retVal := WideString.allocate(l1 + l2);

      self.copySubS(retVal, self, 0, l1);
      self.copySubS(retVal, subs, l1, l2);    

      ^ retVal
   }

   int indexOf(int index, wide literal)
      : info("Saves the index of the first occurence of the specified substring into an output parameter.<br/>If the substring is not found. returns -1")   
   {
      int sublen := literal.Length;
      int retVal := PrimitiveWideStringOperations.indexOf(self, literal, index, sublen);
        
      ^ retVal
   }

   int indexOf(int index, char ch)
   {
      int sublen := self.Length;
      int retVal := PrimitiveWideStringOperations.indexOf(self, ch, index, sublen);
        
      ^ retVal
   }

   wide insert(int index, wide s)
      : info("Inserts the substring into and returns the new one")
   {
      int l1 := self.Length;
      int l2 := s.Length;
      string retVal := WideString.allocate(l1 + l2);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, s, index, l2);    
      self.copySubS(retVal, self, index + l2, l1 - index, index);
    
      ^ retVal;
   }

   wide insert(int index, char ch)
      = self.insert(index, cast wide(ch));

   wide delete(int index, int length)
      : info("Deletes the substring from the object and returns the new one")
   {
      int len1 := self.Length;
      int sindex := index + length;
      string retVal := WideString.allocate(len1-length);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, self, sindex, len1 - length, index);
    
      ^ retVal;         
   }

   string Substring(int index, int length)
      : info("Returns the substring")
   {
      ^ String.fromByteArray(array, index, length);
   }

   int save(int index, char[] buffer, int length, int destIndex)
      = PrimitiveWideStringOperations.save(buffer, self, index, length, destIndex);

   internal saveToShortArray(int index, short[] buffer, int length)
      = PrimitiveWideStringOperations.save(buffer, this array, index, length);

   Enumerator<char> enumerator()
         : info("Returns the string enumerator")
      = new WideStringEnumerator(self);

   Indexer<char> indexer() = new WideStringIndexer(self);
    
   /// Clones the value
   wide clone() = WideString.copy(self);

   int Length
   {
      /// Reads the literal length into an output parameter
      get()
      {
         int l := $len array;
         l := l - 1;

         ^ l;
      }
   }
}

// --- emptyWideString ---

/// An empty wide literal value
public const wide emptyWideString = WideString.MinValue;

// --- WideStringEnumerator ---

sealed class WideStringEnumerator : Enumerator<char>
{
   wide           literal;
   Reference<int> index;
   Reference<int> diff;
   char           char;  

   constructor(wide s)
   {
      literal := s;
      index := -1;
      diff := 1;
      char := $0;
   }

   reset()
   {
      index.Value := -1;
      diff.Value := 1
   }

   bool next()
   {
      int len := literal.Length;
        
      index.append(*diff);
        
      int i := *index;
      if (i < len)
      {
         char := literal[i];
                
         diff.Value := char.Length;
                
         ^ true
      };

      diff.Value := 0;
      char := $0;

      ^ false
   }

   Enumerator cast() = new Enumerator
   {
      embeddable dispatch() => self;
   };
   
   enumerable() = literal;

   char Value = char;
}

// --- WideStringIndexer ---

sealed class WideStringIndexer : Indexer<char>
{
   wide           literal;
   Reference<int> index;

   constructor(wide s)
   {
      literal := s;
      index := 0;
   }

   get bool Available()
   {
      int len := literal.Length;
      int val := *index;
        
      ^ val < len
   }
   
   get char Value()
   {
      int val := *index;
      char ch := literal[val];

      ^ ch
   }
   
   set Value(char ch)
   {
      system'NotSupportedException.raise()
   }

   int Index
   {
      /// Reads the current index and saves it into an output parameter
      get()
      {
         int retVal := *index;
            
         ^ retVal
      }
        
      /// Sets the index by specified value
      set(val)
      {
         index.Value := val
      }
   }
   
   appendIndex(int val)
   {
      index.append(val)
   }
    
   int Length
   {
      /// Reads the length to the output parameter    
      get()
      {
         int retVal := literal.Length;
            
         ^ retVal
      }
   }
        
   Indexer cast() = new Indexer{ embeddable dispatch() => self; };   
}

public sealed class Variant
   : info("A basic type variant class")
{
    object value;
    
    constructor(object)
    {
        value := object
    }
    
    /// Tries to convert the value to a byte number
    byte cast()
        = byteConvertor.convert(value);

    /// Tries to convert the value to a short integer number
    short cast()
        = shortConvertor.convert(value);

    /// Tries to convert the value to a character
    char cast()
        = charConvertor.convert(value);

    /// Tries to convert the literal value to the integer number
    int cast()
        = intConvertor.convert(value);

    /// Tries to convert the literal value to the long integer number
    long cast()
        = longConvertor.convert(value);

    /// Tries to convert the literal value to the real number
    real cast()
        = realConvertor.convert(value);

    /// Returns a literal value
    string cast()
        = stringConvertor.convert(value);
    
    /// Returns a wide literal value
    wide cast()
        = wideConvertor.convert(value);

    string toPrintable()
        => value;

    private castValueTo(target)
        = new{
            castTo(o)
                = o.toPrintable();
            
            castTo(BaseVariable v)
                = self.castValueTo(v.Value);
            
            castTo(int val)
                = cast int(self);
                
            castTo(byte val)
                = cast byte(self);
                
            castTo(short val)
                = cast short(self);
                
            castTo(long val)
                = cast long(self);
                
            castTo(real val)
                = cast real(self);
                
            castTo(string val)
                = cast string(self);
                
            castTo(wide val)
                = cast wide(self);
        }.castTo(target);

    saveTo(target)
    {
        target.Value := self.castValueTo(target.Value);
        
        ^ target
    }

    add(p)
        = p.add(self.castValueTo(p));

    subtract(p)
        = self.castValueTo(p).subtract(p);

    multiply(p)
        = self.castValueTo(p).multiply(p);

    divide(p)
        = self.castValueTo(p).divide(p);

    bool equal(p)
        = self.castValueTo(p).equal(p);
}

// --- Array ---

public sealed Array : Indexable
{
   __string object[] array;
    
   /// Creates an empty array
   constructor MinValue
      = new object[](0);
    
   /// Creates the array with the specified length
   constructor allocate(int len)
      = new object[](len);

    /// Creates a copy of the provided sub array
   constructor copy(int index, int length, Array array)
      <= allocate(length)
   {
      for(int i := 0 , i < length, i += 1)
      {
         this array[i] := array[i + index]
      }
   }
        
   /// Concatinates the arrays    
   add(Array a)
   {
      int length1 := self.Length;
      int length2 := a.Length;
        
      auto retVal := Array.allocate(length1 + length2);

      for (int i:=0, i < length1, i += 1) {
         retVal[i] := array[i]
      };

      for (int i:=0, i < length2, i += 1) {
         retVal[length1 + i] := a[i]
      };
            
      ^ retVal
   }

   int Length
   {
      get()
      {
         int len := $len array;
            
         ^ len
      }
   }

    /// Sets the specified array member
    setAt(int n, object o)
    {
        array[n] := o
    }

    /// Returns the specified array member
    object at(int n)
        = array[n];
 
    /// Returns the array enumerator
    Enumerator enumerator() = new ArrayEnumerator(self);

    /// Returns the array indexer
    Indexer indexer() = new ArrayIndexer(self);

//    object[] Value = theArray;

    /// Clones the array
    Array clone()
        = Array.copy(0, self.Length, self);
}

// --- ArrrayEnumerator ---

public sealed ArrayEnumerator : Enumerator
{
   Array          array;
   Reference<int> index;

   constructor(Array arr)
   {
      array := arr;
      index := -1;
   }

   reset()
   {
      index.Value := -1;
   }

   bool next()
   {
      int len := array.Length;
        
      index.append(1);
        
      int i := *index;
      if (i < len)
      {
         ^ true;
      };

      ^ false
   }

   Value
      = array[*index];

   enumerable() = array;
}

// --- ArrayIndexer ---

public sealed ArrayIndexer : Indexer
{
   Array          array;
   Reference<int> index;

   constructor(Array arr)
   {
      array := arr;
      index := 0;
   }

   get bool Available()
   {
      int len := array.Length;
      int val := *index;
        
      ^ val < len
   }
   
   get Value()
   {
      int val := *index;

      ^ array[val];
   }
   
   set Value(o)
   {
      int val := *index;

      array[val] := o;      
   }

   int Index
   {
      /// Reads the current index and saves it into an output parameter
      get()
      {
         int retVal := *index;
            
         ^ retVal
      }
        
      /// Sets the index by specified value
      set(val)
      {
         index.Value := val
      }
   }
   
   appendIndex(int val)
   {
      index.append(val)
   }
    
   int Length
   {
      /// Reads the length to the output parameter    
      get()
      {
         int retVal := array.Length;
            
         ^ retVal
      }
   }
}