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

   static copy(Array<T> target, Array<T> source, int index, int length)
   {      
      for(int i := 0 , i < length, i += 1)
      {
         target[i] := source[i + index]
      }
   }

   static copyTo(Array<T> target, Array<T> source, int index, int length)
   {      
      for(int i := 0 , i < length, i += 1)
      {
         target[i + index] := source[i]
      }
   }

   static move(Array<T> target, int index, int length, int offset)
   {
      for (int i := 0, i < length, i += 1)
      {
         target[i] := target[i + offset]
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
      };
            
      ^ nil // !! never reached
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
                                 
   T[] clone()
   {
      int len := self.Length;
      T[] copy := new T[](len);
      for(int i := 0, i < len, i+=1) {
         copy[i] := array[i]
      };
        
      ^ copy
   }
}

// --- Variadic Array template ---

public sealed nonstructural VariadicArray<T> : Indexable<T>
{
   embeddable __string T[] array;

   private readArgLength(ref int retVal) : external(system'core_routines'__varg_readLength);

   T at(int n)
      = array[n];

   int Length
   {
      get()
      {
         self.readArgLength(ref int len);
            
         ^ len
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
         this index := -1
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

   T[] cast()
   {
      int len := self.Length;
        
      T[] copy := new T[](len);
      for(int i := 0, i < len, i += 1)
      {
         copy[i] := array[i]
      };
        
      ^ copy
   }

}

// --- matrixof ---

public sealed Matrix<T>
{
   T[] _array;
   int _rows;
   int _columns;
    
   constructor allocate(int rows, int columns)
   {
      _rows := rows;
      _columns := columns;
        
      int len := _rows * _columns;
        
      _array := new T[](len)
   }
    
   constructor allocate(rows, columns)
      <= allocate(cast int(rows), cast int(columns));
    
   constructor load(params)
   {
      _rows := params.Rows;
      _columns := params.Columns;
        
      int len := _rows * _columns;
        
      _array := new T[](/*theRows * theColumns*/len);
        
      int i := 0;
      int j := 0;
        
      while (i < _rows)
      {
         j := 0;
         while (j < _columns)
         {
            T val := params.at(i,j);
                
            self.setAt(i, j, val);
                
            j := j + 1
         };
            
         i := i + 1
      }
   }    
    
   int Length
   {
      get()
      {
         ^ _rows
      }
   }    

   int Columns
   {
      get()
      {
         ^ _columns
      }
   }

   int Rows
   {
      get()
      {
         ^ _rows
      }
   }

   /// Writes an integer value at the specified position
   setAt(int i, int j, T v)
   {        
      int index := i * _columns + j;
        
      _array[index] := v
   }

   /// Reads an integer value at the specified position and saves it into an output variable
   read(int i, int j, ref T retVal)
   {
      int index := i * _columns + j;
        
      retVal := _array[index];
   }

   T at(int i, int j)
   {
      self.read(i, j, ref T retVal);
      ^ retVal
   }

   /// Returns a row
   at(i) = new
   {
      Length = _columns;
        
      at(j)
         = self.at(i,j);
        
      setAt(j, v)
      {
         self.setAt(i,j,v)
      }
   };

   Enumerator enumerator() 
   {
      int i := 0;
      int j := -1;
      int length1 := _rows;
      int length2 := _columns;
        
      ^ new Enumerator
          {
             get Value() = self.at(i, j);
              
             enumerable() = self;
              
             reset() { i := 0; j := -1; }
              
             bool next()
             {
                ifnot (i < length1)
                {
                   ^ false
                };
                  
                j := j + 1;
                  
                ifnot (j < length2)
                {
                   j := -1;
                   i := i + 1;
                      
                   ^ false
                };
                  
                ^ true
             }
          }
   }
}

// === basic memory classes ===

// --- BitArray ---

public sealed class BitArray
{
   int[] _array;
   int   _length;
    
   /// Creates the array with the specified length
   constructor allocate(int length)
   {
      _length := length;
      _array := new int[](length / 32 + 1)
   }
    
   int Length
   {
      get()
      {
         ^ _length
      }
   }
    
   at(index)
      = self.at(cast int(index));
    
   bool at(int index)
   {
      int arr_index := index / 32;
      int bit_index := arr_index * 32;
      bit_index := index - arr_index;
        
      int bit_mask := 1;
      if (bit_index != 0)
         { bit_mask := bit_mask $shl bit_index };
        
      int bits := _array[arr_index];
        
      ^ (bits & bit_mask) != 0
   }

   setAt(int index, bool value)
   {
      int arr_index := index / 32;
      int bit_index := arr_index * 32;
      bit_index := index - arr_index;
        
      int bit_mask := 1;
      if (bit_index != 0)
         { bit_mask := bit_mask $shl bit_index };
        
      int bits := _array[arr_index];
        
      if(value)
      { 
         bits := bits | bit_mask 
      }
      else
      {  
         bit_mask := bit_mask.BInverted;
            
         bits := bits & bit_mask
      };
            
      _array[arr_index] := bits
   }
    
   /// Returns the array enumerator
   Enumerator<bool> enumerator() = new BitArrayEnumerator(self);

   /// Returns the array indexer
   Indexer<bool> indexer() = new BitArrayIndexer(self);
}

// --- BitArray32 ---

public sealed class BitArray32
{
   int _value;
    
   constructor load(int value)
   {
      _value := value
   }
    
   int Length = 32;
    
   write(int value)
   {
      _value := value
   }
    
   bool at(int index)
   {
      int bit_mask := 1;
      if (index != 0)
         { bit_mask := bit_mask $shl index };
        
      ^ (_value & bit_mask) != 0
   }
    
   setAt(int index, bool value)
   {
      int bit_mask := 1;
      if (index != 0)
         { bit_mask := bit_mask $shl index };
        
      if(value)
      { 
         _value := _value | bit_mask 
      }
      else
      {  
         bit_mask := bit_mask.BInverted;
            
         _value := _value & bit_mask
      }
   }
    
   /// Returns the array enumerator
   Enumerator<bool> enumerator() = new BitArray32Enumerator(self);

   /// Returns the array indexer
   Indexer<bool> indexer() = new BitArray32Indexer(self);
}

// --- IntMatrix ---

public sealed class IntMatrix
{
   int[] _array;
   int   _rows;
   int   _columns;
    
   constructor allocate(int rows, int columns)
   {
      _rows := rows;
      _columns := columns;
        
      _array := new int[](columns * rows)
   }
        
   /// Reads the matrix lengths to the output parameter
   readLengthsTo(ref int rows, ref int columns)
   {
      rows := _rows;
      columns := _columns
   }

   int Length
   {
      get()
      {
         ^ _rows
      }
   }    

   int Columns
   {
      get()
      {
         ^ _columns
      }
   }

   int Rows
   {
      get()
      {
         ^ _rows
      }
   }

   /// Writes an integer value at the specified position
   setAt(int i, int j, int v)
   {
      int index := i * _columns + j;
        
      _array[index] := v
   }

   int at(int i, int j)
   {
      int index := i * _columns + j;
      int retVal := _array[index];
        
      ^ retVal
   }

   /// Returns a row
   at(int i) = new Indexable<int>
   {
      Length = _columns;
        
      Indexer<int> indexer() = new BaseIndexer<int>
      {
         this index := 0;
            
         this length := self.Columns;
            
         get int Value()
            = self.at(i, index);
                
         set Value(int o)
         {
            self.setAt(i,index,o);
         }                        
      }; 
        
      int at(int j)
         = self.at(i,j);
        
      setAt(int j, int v)
      {
         self.setAt(i,j,v)
      }
   };
}

// --- RealMatrix ---

public sealed class RealMatrix
{
   real[] _array;
   int    _rows;
   int    _columns;
    
   constructor allocate(int rows, int columns)
   {
      _rows := rows;
      _columns := columns;
        
      _array := new real[](columns * rows)
   }
        
   /// Reads the matrix lengths to the output parameter
   readLengthsTo(ref int rows, ref int columns)
   {
      rows := _rows;
      columns := _columns
   }

   int Length
   {
      get()
      {
         ^ _rows
      }
   }    

   int Columns
   {
      get()
      {
         ^ _columns
      }
   }

   int Rows
   {
      get()
      {
         ^ _rows
      }
   }

   /// Writes a real value at the specified position
   setAt(int i, int j, real v)
   {
      int index := i * _columns + j;
        
      _array[index] := v
   }

   /// Reads a real value at the specified position and saves it into an output variable
   read(int i, int j, ref real retVal)
   {
      int index := i * _columns + j;
        
      retVal := _array[index];
   }

   real at(int i, int j)
   {
      int index := i * _columns + j;
      real retVal := _array[index];
        
      ^ retVal
   }

   at(int i) = new Indexable<real>
   {
      Length = _columns;
        
      Indexer<real> indexer() = new BaseIndexer<real>
      {
         this index := 0;
            
         this length := self.Columns;
            
         get real Value()
            = self.at(i,index);
                
         set Value(real o)
         {
            self.setAt(i,index,o);
         }                        
      }; 
        
      real at(int j)
         = self.at(i,j);
        
      setAt(int j, real v)
      {
         self.setAt(i,j,v)
      }
   };
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
      length := UTFOperations.convertUTF32_2_8(chVal, buffer, 0, 4);
        
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
      <= allocate(UTFOperations.calcSubStringLength(index, length, charArray))
   {
      PrimitiveArrayOperations.copyCharArray(charArray, index, length, this array);
   }

   internal constructor fromByteArray(int index, int size, byte[] buffer)
      <= allocate(size)
   {
      PrimitiveArrayOperations.copy(buffer, index, size, array);

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
         m := shortArrayConvertor.convert(self, l, buffer, 0, 256);

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

         m := shortArrayConvertor.convert(self.Substring(i,page), page, buffer, 0, 256);

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
      auto s := cast string(o) ?? nil;
      (nil == s)
         ? { ^ false };

      ^ self.equal(s)
   }

   bool less(o)
      <= less(cast string(o));

   private copySubS(string dest, string sour, int index, int length) : external(system'core_routines'__str_copysubs);
   private copySubS(string dest, string sour, int index, int length, int sourIndex) : external(system'core_routines'__str_xcopysubs);

   char at(int index)
   {
      int len := self.Length;
      if (index < 0 || index >= len)
      {
         OutOfRangeException.raise()
      };

      UTFOperations.convertUTF8_2_32(array, index, len, ref int retVal);

      char ch := retVal;

      ^ ch;
   }

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
      int sublen := self.Length - index;

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
      self.copySubS(retVal, self, index, len1 - sindex, sindex);
    
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

public const String emptyString = String.MinValue;

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
      length := UTFOperations.convertUTF32_2_16(chVal, buffer, 0, length);
        
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
      <= allocate(UTFOperations.calcSubWideStringLength(index, length, charArray))
   {
      PrimitiveArrayOperations.copyCharArray(charArray, index, length, this array);
   }

   internal constructor fromShortArray(int index, int size, short[] buffer)
      <= allocate(size)
   {
      PrimitiveArrayOperations.copy(buffer, index, size, array);

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
         m := byteArrayConvertor.convert(self, l, buffer, 0, 256);

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

         m := byteArrayConvertor.convert(self.Substring(i,page), page, buffer, 0, 256);

         retVal := retVal.add(String.fromByteArray(0, m, buffer));

         i := i + page;
         l := l - page
      };
        
      ^ retVal      
   }

   Enumerable<char> cast() = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

   string toPrintable() = self;


   private copySubS(wide dest, wide sour, int index, int length) : external(system'core_routines'__wstr_copysubs);
   private copySubS(wide dest, wide sour, int index, int length, int sourIndex) : external(system'core_routines'__wstr_xcopysubs);

   bool equal(o)
   {
      auto s := cast wide(o) ?? nil;
      (nil == s)
         ? { ^ false };

      ^ self.equal(s)
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
   {
      int len := self.Length;
      if (index < 0 || index >= len)
      {
         OutOfRangeException.raise()
      };

      UTFOperations.convertUTF16_2_32(array, index, len, ref int retVal);

      char ch := retVal;

      ^ ch;
   }

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
      wide retVal := WideString.allocate(l1 + l2);

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
      int sublen := self.Length - index;
      int retVal := PrimitiveWideStringOperations.indexOf(self, ch, index, sublen);
        
      ^ retVal
   }

   wide insert(int index, wide s)
      : info("Inserts the substring into and returns the new one")
   {
      int l1 := self.Length;
      int l2 := s.Length;
      wide retVal := WideString.allocate(l1 + l2);

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
      wide retVal := WideString.allocate(len1-length);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, self, index, len1 - sindex, sindex);
    
      ^ retVal;         
   }

   wide Substring(int index, int length)
      : info("Returns the substring")
   {
      ^ WideString.fromShortArray(index, length, array);
   }

   int save(int index, int length, char[] buffer, int destIndex)
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
public const WideString emptyWideString = WideString.MinValue;

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
                = self.castValueTo(*v);
            
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

   object[] Value = array;

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

// --- BitArrayIndexer ---

/// An array indexer
class BitArrayIndexer : Indexer<bool>
{
   BitArray       _array;
   Reference<int> _index;

   /// Returns the parameter indexer
   constructor(BitArray array)
   {
      _array := array;
      _index := 0
   }
    
   /// Returns false if the index out of range
   get bool Available()
   {
      int length := _array.Length;
      int index := _index.Value;
        
      ^ index < length
   }

   /// Returns the current member
   get bool Value()
   {
      int index := _index.Value;
        
      ^ _array.at(index)
   }
    
   /// Sets the current member
   set Value(bool val)
   {
      int index := _index.Value;
        
      _array.setAt(_index, val)
   }
    
   int Index
   {
      /// Reads the current index and saves it into an output parameter
      get()
      {
         int retVal := _index.Value;
            
         ^ retVal
      }
        
      /// Sets the index by specified value
      set(val)
      {
         _index.Value := val
      }
   }
    
   appendIndex(int val)
   {
      _index.append(val)
   }
   
   int Length
   {
      /// Reads the length to the output parameter    
      get()
      {
         int retVal := _array.Length;
            
         ^ retVal
      }
   }
        
   Indexer cast() = new Indexer { embeddable dispatch() => self; };        
}

// --- BitArrayEnumerator ---

/// An array enumerator
class BitArrayEnumerator : Enumerator<bool>
{
   BitArray       _array;
   Reference<int> _index;
    
   /// Creates an array enumerator
   constructor(BitArray a)
   {
      _array := a;
      _index := -1
   }

   /// Resets the enumerator
   reset()
   {
      _index.Value := -1
   }

   /// Goes to the next enumeration member and returns true if it is not the last member
   bool next()
   {
      int length := _array.Length;
        
      _index.append(1);
        
      int index := _index.Value;

      ^ index < length
   }
    
   /// Returns the current object
   get bool Value()
   {
      int index := _index.Value;
        
      ^ _array.at(index)
   }
        
   enumerable() = _array;
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// --- BitArray32Indexer ---

/// An array indexer
class BitArray32Indexer : Indexer<bool>
{
   BitArray32     _array;
   Reference<int> _index;

   /// Returns the parameter indexer
   constructor(BitArray32 bs)
   {
      _array := bs;
      _index := 0
   }
    
   /// Returns false if the index out of range
   get bool Available()
   {
      int index := _index;
        
      ^ index < 32
   }

   /// Returns the current member
   get bool Value()
   {
      int index := _index;
        
      ^ _array.at(index)
   }
    
   /// Sets the current member
   set Value(bool val)
   {
      int index := _index;
        
      _array.setAt(index, val)
   }

   int Index
   {    
      /// Reads the current index and saves it into an output parameter
      get()
      {
         int retVal := _index.Value;
            
         ^ retVal
      }
        
      /// Sets the index by specified value
      set(val)
      {
         _index.Value := val
      }
   }
    
   appendIndex(int val)
   {
      _index.append(val)
   }
    
   int Length
   {
      /// Reads the length to the output parameter    
      get()
         = _array.Length;
//         => _array;
   }
  
   Indexer cast() = new Indexer { embeddable dispatch() => self; };        
}

// --- BitArray32Enumerator ---

// An array enumerator
class BitArray32Enumerator : Enumerator<bool>
{
   BitArray32     _array;
   Reference<int> _index;
    
   /// Creates an array enumerator
   constructor(BitArray32 bs)
   {
      _array := bs;
      _index := -1
   }

   /// Resets the enumerator
   reset()
   {
      _index.Value := -1
   }

   /// Goes to the next enumeration member and returns true if it is not the last member
   bool next()
   {
      _index.append(1);

      int index := _index;

      ^ index < 32
   }
    
   /// Returns the current object
   get bool Value()
   {
      int index := _index;
        
      ^ _array.at(index)
   }
        
   enumerable() = _array;        
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}
