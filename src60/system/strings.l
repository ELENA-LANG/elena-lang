// --- Array template ---

public sealed Array<T> : Indexable<T>,
   info("A strong-typed array template")
{
   embeddable __string T[] array;

   static Array<T>? Default : info("Returns nil")
      = nil;

   static Array<T> MinValue : info("Returns an array with zero elements")
      = new T[](0);

   constructor allocate(int length) : info("Creates an array with the specified <b>length</b>")
      = new T[](length);   

   constructor copy(T[] array, int index, int length) : info("Creates a copy of sub array of <b>array</b> starting from <b>index</b> with the specified <b>length</b>")
      <= allocate(length)
   {
      for(int i := 0; i < length; i += 1)
      {
         this array[i] := array[i + index]
      }
   }

   static copy(Array<T> target, Array<T> source, int index, int length)
      : info("Copies a sub array from <b>source</b> starting at <b>index</b> with the specified <b>length</b> into <b>target</b> at 0 position.")
   {      
      for(int i := 0; i < length; i += 1)
      {
         target[i] := source[i + index]
      }
   }

   static copyTo(Array<T> target, Array<T> source, int index, int length)
      : info("Copies <b>source</b> array starting with 0 position with the specified <b>length</b> into <b>target</b> at <b>index</b> position")
   {      
      for(int i := 0; i < length; i += 1)
      {
         target[i + index] := source[i]
      }
   }

   static move(Array<T> target, int index, int length, int offset)
       : info("Moves a sub array inside <b>target</b>.<br/>If <b>index</b> is positive moves <b>length</b> elements starting from <b>offset</b> to the array start<br/>Otherwise moves first <b>length</b> elements to the right on <b>index</b> positions")
   {
      if (offset > 0) {
         for (int i := 0; i < length; i += 1)
         {
            target[i] := target[i + offset]
         }
      }
      else {
         int i := length;
         int j := length - offset;
         while (i > index) {
            i -= 1;
            j -= 1;

            target[j] := target[i];
         }
      }
   }

   Enumerator<T> enumerator()
      : info("Creates an array enumerator") 
   = new BaseEnumerator<T>
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

   Enumerable cast() : info("Returns enumerable wrapper around the array")
      = new Enumerable{ Enumerator enumerator() => self; };

   Indexer<T> indexer() 
      : info("Creates an array indexer") 
   = new BaseIndexer<T>
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
      : info("Returns an element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
   {
      if (n < 0)
         { system'InvalidArgumentException.raise() };
        
      int len := $len array;

      if (n < len)
      { 
         ^ array[n] 
      };

      ^ system'InvalidArgumentException.raise();
   }

   setAt(int n, T item)
      : info("Assigns <b>item</b> to the element at the position <b>n</b><br/>If an index is out of range, it raises InvalidArgumentException exception.")
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
      : info("Returns the array length")
   {
      get()
      {
         int len := $len array;
            
         ^ len
      }
   }

   T[] add(T[] a) : info("Concatenates the array and an argument <b>a</b> and returns the new array")
      <= add(a, a.Length);
                              
   T[] add(T[] a, int length) : info("Concatenate the array and an argument <b>a</b> and returns the new array")
   {
      int length1 := self.Length;
      int length2 := length;
        
      auto retVal := new T[](length1 + length2);

      for (int i:=0; i < length1; i += 1) {
         retVal[i] := array[i]
      };

      for (int i:=0; i < length2; i += 1) {
         retVal[length1 + i] := a[i]
      };
            
      ^ retVal
   }
                                 
   T[] clone()
       : info("Returns the shallow copy of the array")
   {
      int len := self.Length;
      T[] copy := new T[](len);
      for(int i := 0; i < len; i+=1) {
         copy[i] := array[i]
      };
        
      ^ copy
   }
   
   Array cast()
       : info("Creates a weak copy of the array")
   {
      int len := self.Length;
      auto clone := Array.allocate(len);
      for (int i := 0; i < len; i++) {
         clone[i] := array[i];
      };
      
      ^ clone
   }   
}

// --- Variadic Array template ---

public sealed nonstructural VariadicArray<T> : Indexable<T>,
   info("A strong-typed variadic array template<br/>It is used to box the method variadic argument list")
{
   embeddable __string T[] array;

   private readArgLength(out int retVal) : external(system'core_routines'__varg_readLength);

   T at(int n)
          : info("Returns an element at the position <b>n</b>")
      = array[n];

   setAt(int n, T item)
      : info("Assigns <b>item</b> to the element at the position <b>n</b>")
   {
      array[n] := item;
   }  

   int Length
      : info("Returns the variadic array length")
   {
      get()
      {
         self.readArgLength(out int len);
            
         ^ len
      }
   }

   Enumerator<T> enumerator() 
      : info("Creates a variadic array enumerator") 
   = new BaseEnumerator<T>
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

   Indexer<T> indexer() 
      : info("Creates a variadic array indexer") 
   = new BaseIndexer<T>
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
      : info("Creates a strong typed array of the variadic array elements")
   {
      int len := self.Length;
        
      T[] copy := new T[](len);
      for(int i := 0; i < len; i += 1)
      {
         copy[i] := array[i]
      };
        
      ^ copy
   }

}

// --- matrixof ---

public sealed Matrix<T>
   : info("A two-dimensional strong-type array")
{
   T[] _array;
   int _rows;
   int _columns;
    
   constructor allocate(int rows, int columns)
      : info("Creates a two-dimensional array of <b>rows<b> and <b>columns<b>")
   {
      _rows := rows;
      _columns := columns;
        
      int len := _rows * _columns;
        
      _array := new T[](len)
   }
    
   constructor allocate(rows, columns)
      <= allocate(cast int(rows), cast int(columns));
    
   int Length
      : info("Returns the number of rows")
   {
      get()
      {
         ^ _rows
      }
   }    

   int Columns
      : info("Returns the number of columns")
   {
      get()
      {
         ^ _columns
      }
   }

   int Rows
      : info("Returns the number of rows")
   {
      get()
      {
         ^ _rows
      }
   }

   private read(int i, int j, ref T retVal)
   {
      int index := i * _columns + j;
        
      retVal := _array[index];
   }

   /// Writes an integer value at the specified position
   setAt(int i, int j, T v)
     : info("Assigns <b>v</b> argument to the matrix element at a row <b>i</b> and a column <b>j</b>")
   {        
      int index := i * _columns + j;
        
      _array[index] := v
   }

   T at(int i, int j)
     : info("Returns the matrix element at a row <b>i</b> and a column <b>j</b>")
   {
      self.read(i, j, ref T retVal);
      ^ retVal
   }

   /// Returns a row
   at(i) 
      : info("Returns <b>i</b>-th row as an array") 
   = new
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
      : info("Creates a weak matrix enumerator<br/>It goes from the first row till the last one for each column") 
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
                if:not (i < length1)
                {
                   ^ false
                };
                  
                j := j + 1;
                  
                if:not (j < length2)
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
   : info("An array of Boolean values")
{
   int[] _array;
   int   _length;
    
   constructor allocate(int length)
      : info("Creates a bit array with the specified <b>length</b>")
   {
      _length := length;
      _array := new int[](length / 32 + 1)
   }
    
   int Length
      : info("Returns the bit array length")
   {
      get()
      {
         ^ _length
      }
   }
    
   at(index)
      = self.at(cast int(index));
    
   bool at(int index)
      : info("Returns a boolean value at the specified <b>index</b> position")
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
      : info("Set a boolean value at the specified <b>index</b> position")
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
    
   Enumerator<bool> enumerator()
       : info("Creates a boolean enumerator")
     = new BitArrayEnumerator(self);

   Indexer<bool> indexer() 
       : info("Creates a boolean indexer")
     = new BitArrayIndexer(self);
}

// --- BitArray32 ---

public sealed class BitArray32
   : info("An 32-bit integer bit map")
{
   int _value;
    
   constructor load(int value)
      : info("Creates a bit map from <b>value</b>")
   {
      _value := value
   }
    
   int Length : info("Returns 32") 
      = 32;
    
   write(int value)
     : info("Copies a bit map from <b>value</b>")
   {
      _value := value
   }
    
   bool at(int index)
      : info("Returns a bit at the specified <b>index</b> position")
   {
      int bit_mask := 1;
      if (index != 0)
         { bit_mask := bit_mask $shl index };
        
      ^ (_value & bit_mask) != 0
   }
    
   setAt(int index, bool value)
      : info("Set a boolean value of <b>value<b> to the specified <b>index</b> position")
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
    
   Enumerator<bool> enumerator() 
         : info("Creates a bit map enumerator")
      = new BitArray32Enumerator(self);

   Indexer<bool> indexer() 
         : info("Creates a bit map indexer")
     = new BitArray32Indexer(self);
}

// --- IntMatrix ---

public sealed class IntMatrix
   : info("A two-dimensional array of integers") 
{
   int[] _array;
   int   _rows;
   int   _columns;
    
   constructor allocate(int rows, int columns)
      : info("Creates a two-dimensional array of <b>rows<b> and <b>columns<b>")
   {
      _rows := rows;
      _columns := columns;
        
      _array := new int[](columns * rows)
   }
        
   readLengthsTo(out int rows, out int columns)
      : info("Reads the matrix lengths to the output parameter")
   {
      rows := _rows;
      columns := _columns
   }

   int Length
      : info("Returns the number of rows")
   {
      get()
      {
         ^ _rows
      }
   }    

   int Columns
      : info("Returns the number of columns")
   {
      get()
      {
         ^ _columns
      }
   }

   int Rows
      : info("Returns the number of rows")
   {
      get()
      {
         ^ _rows
      }
   }

   setAt(int i, int j, int v)
     : info("Assigns <b>v</b> argument to the matrix element at a row <b>i</b> and a column <b>j</b>")
   {
      int index := i * _columns + j;
        
      _array[index] := v
   }

   int at(int i, int j)
     : info("Returns the matrix element at a row <b>i</b> and a column <b>j</b>")
   {
      int index := i * _columns + j;
      int retVal := _array[index];
        
      ^ retVal
   }

   /// Returns a row
   at(int i) 
       : info("Returns indexable wrapper around the matrix row at the <b>i</b> position")
   = new Indexable<int>
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
   : info("A two-dimensional array of real numbers") 
{
   real[] _array;
   int    _rows;
   int    _columns;
    
   constructor allocate(int rows, int columns)
      : info("Creates a two-dimensional array of <b>rows<b> and <b>columns<b>")
   {
      _rows := rows;
      _columns := columns;
        
      _array := new real[](columns * rows)
   }
        
   readLengthsTo(ref int rows, ref int columns)
      : info("Reads the matrix lengths to the output parameter")
   {
      rows := _rows;
      columns := _columns
   }

   int Length
      : info("Returns the number of rows")
   {
      get()
      {
         ^ _rows
      }
   }    

   int Columns
      : info("Returns the number of columns")
   {
      get()
      {
         ^ _columns
      }
   }

   int Rows
      : info("Returns the number of rows")
   {
      get()
      {
         ^ _rows
      }
   }

   /// Writes a real value at the specified position
   setAt(int i, int j, real v)
     : info("Assigns <b>v</b> argument to the matrix element at a row <b>i</b> and a column <b>j</b>")
   {
      int index := i * _columns + j;
        
      _array[index] := v
   }

   read(int i, int j, ref real retVal)
     : info("Reads a real value at the specified position and saves it into an output variable")
   {
      int index := i * _columns + j;
        
      retVal := _array[index];
   }

   real at(int i, int j)
     : info("Returns the matrix element at a row <b>i</b> and a column <b>j</b>")
   {
      int index := i * _columns + j;
      real retVal := _array[index];
        
      ^ retVal
   }

   at(int i) 
      : info("Returns indexable wrapper around the matrix row at the <b>i</b> position")
   = new Indexable<real>
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
        : info("Returns an empty string")
      = "";

   constructor allocate(int size)
        : info("creates a string with specific <b>size</b>")
      = new byte[](size + 1);

   constructor copy(int size, string s)
        : info("Creates the copy of a substring of <b>s</b> with specified length <b>size</b> starting at 0")
      <= allocate(size)
   {
      PrimitiveStringOperations.copy(array, 0, size, s);

      // add tailing zero
      byte z := 0;
      array[size] := z;
   }

   constructor fill(int size, char ch)
        : info("Creates the literal string with specified length <b>size</b> fills with character <b>ch</b>")
      <= allocate(ch.Length * size)
   {
      byte buffer[4];
      int chVal := ch.Value;
      int  length := ch.Length;
      length := UTFOperations.convertUTF32_2_8(chVal, buffer, 0, 4);
        
      int offs := 0;
      for (int i := 0; i < size; i := i + 1)
      {
         for (int j := 0; j < length; j := j + 1) {
            array[offs] := buffer[j];
            offs := offs + 1;
         }
      };

      // add tailing zero
      byte z := 0;
      array[offs] := z;
   }

   constructor copy(string s)
       : info("Creates the literal string copy of <b>s</b>")
      <= copy(s.Length, s);

   constructor copy(int index, int length, char[] charArray)
        : info("Creates the copy of a sub array <b>charArray</b> starting from <b>index</b> and with the specified <b>length</b>")
      <= allocate(UTFOperations.calcSubStringLength(index, length, charArray))
   {
      PrimitiveArrayOperations.copyCharArray(charArray, index, length, this array);

      // add tailing zero
      int len := self.Length;
      byte z := 0;
      array[len] := z;
   }

   internal constructor fromByteArray(int index, int size, byte[] buffer)
      <= allocate(size)
   {
      PrimitiveArrayOperations.copy(buffer, index, size, array);

      // add tailing zero
      byte z := 0;
      array[size] := z;
   }

   string toPrintable() 
        : info("Returns itself")
      = self;

   string cast(params object[] args)
   {
      byte buffer[1024];

      int len := args.Length;
      int bufLen := 0;

      string? retVal := nil;
      for(int i := 0; i < len; i++) {
         string arg := args[i].toPrintable();
         int argLen := arg.Length;

         if (argLen > 1024 - bufLen) {
            string tempStr := String.fromByteArray(0, bufLen, buffer);

            if (retVal == nil) {
               retVal := tempStr;
            }
            else retVal := retVal + tempStr;

            bufLen := 0;
         };
         PrimitiveStringOperations.copyTo(buffer, bufLen, argLen, arg);
         bufLen += argLen;
      };

      string tempStr := String.fromByteArray(0, bufLen, buffer);
      if (retVal == nil) {
         retVal := tempStr;
      }
      else retVal := retVal + tempStr;

      ^ retVal!;      
   }

   wide cast()
      : info("Converts to the UTF-16 string")
   {
      int        i := 0;
      int        m := 0;
      int        page := 0;
      int        l := self.Length;
      short buffer[256];            
      if (l < 256)
      {
         m := ShortArrayConvertor.convert(self, l, buffer, 0, 256);

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

         m := ShortArrayConvertor.convert(self.Substring(i,page), page, buffer, 0, 256);

         retVal := retVal.add(WideString.fromShortArray(0, m, buffer));

         i := i + page;
         l := l - page
      };
        
      ^ retVal      
   }

   Enumerable<char> cast()
        : info("Creates an enumerable wrapper")
      = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

   int Length
        : info("Returns the string length")
   {
      get()
      {
         int l := $len array;
         l := l - 1;

         ^ l;
      }
   }

   bool equal(string s) : external(system'core_routines'__str_equal), info("Returns true if the string equals to <b>s</b>; otherwise returns false");

   bool equal(wide s)
         : info("Returns true if the string equals to <b>s</b>; otherwise returns false<br/>Note the string will be converted to UTF-16 string before comparison")
      = cast wide(self).equal(s);

   bool equal(char ch)
         : info("Returns true if the string equals to <b>ch</b>; otherwise returns false<br/>Note the character will be converted to UTF-8 string before comparison")
      <= equal(cast string(ch));

   bool less(string s) : external(system'core_routines'__str_less), info("Returns true if the string is less then <b>s</b>; otherwise returns false");

   bool less(wide s)
          : info("Returns true if the string less then <b>s</b>; otherwise returns false<br/>Note the string will be converted to UTF-16 string before comparison")
      = cast wide(self).less(s);

   bool less(char ch)
         : info("Returns true if the string is less then <b>ch</b>; otherwise returns false<br/>Note the character will be converted to UTF-8 string before comparison")
      <= less(cast string(ch));

   bool equal(o)
         : info("Tries to convert the argument <b>o</b> to UTF-8 string and compare with it. If the conversion is not possible it returns false")
   {
      if (o; is string s)
      {
         ^ self.equal(s)
      };

      ^ false
   }

   bool less(o)
         : info("Tries to convert the argument <b>o</b> to UTF-8 string and check if the object is less then it. If the conversion is not possible it returns false")
   {
      if (o; is string s)
      {
         ^ self.less(s)
      };

      ^ false
   }

   private copySubS(string dest, string sour, int index, int length) : external(system'core_routines'__str_copysubs);
   private copySubS(string dest, string sour, int index, int length, int sourIndex) : external(system'core_routines'__str_xcopysubs);

   char at(int index)
      : info("Returns a character at the specified position <b>index</b>")
   {
      int len := self.Length;
      if (index < 0 || index >= len)
      {
         OutOfRangeException.raise()
      };

      UTFOperations.convertUTF8_2_32(array, index, len, out int retVal);

      char ch := retVal;

      ^ ch;
   }

   retoverload byte at(int index)
         : info("Returns a byte at the specified position <b>index</b>")
      = array[index];

   string add(o)
      <= add(cast string(o));

   string add(BaseVariable v)
      <= add(*v);

   string add(string subs)
      : info("Concatenate the values")
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
      : info("Saves the index of the first occurence of the specified string <b>literal</b> starting from <b>index</b> into an output parameter.<br/>If the substring is not found. returns -1")
   {
      int sublen := literal.Length;
      int retVal := PrimitiveStringOperations.indexOf(self, literal, index, sublen);
        
      ^ retVal
   }

   int indexOf(int index, char ch)
      : info("Saves the index of the first occurence of the specified character <b>ch</b> starting from <b>index</b> into an output parameter.<br/>If the substring is not found. returns -1")
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
        : info("Inserts the character <b>ch</b> at the specified <b>index</b> and returns the new one")
      = self.insert(index, cast string(ch));

   string delete(int index, int length)
      : info("Deletes the substring starting from <b>index</b> and with the specified <b>length</b> from the object and returns the new one")
   {
      int len1 := self.Length;
      int sindex := index + length;
      string retVal := String.allocate(len1-length);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, self, index, len1 - sindex, sindex);
    
      ^ retVal;         
   }

   string Substring(int index, int length)
      : info("Returns the substring from <b>index</b> and with the specified <b>length</b>")
   {
      ^ String.fromByteArray(index, length, array);
   }

   int save(int index, int length, char[] buffer, int destIndex)
        : info("Copies the characters from the string starting from <b>index</b> and with the specified <b>length</b> and copies to <b>buffer</b> starting from <b>destIndex</b>")
      = PrimitiveStringOperations.save(buffer, self, index, length, destIndex);

   internal saveToByteArray(int index, byte[] buffer, int length)
      = PrimitiveStringOperations.save(buffer, this array, index, length);

   Enumerator<char> enumerator()
         : info("Returns the string enumerator")
      = new StringEnumerator(self);

   Indexer<char> indexer() 
         : info("Returns the string indexer")
      = new StringIndexer(self);
    
   string clone() : info("Clones the value")
     = String.copy(self);
}

public const String EmptyString : info("Empty string constant") = String.MinValue;

// obsolete
public const String emptyString : info("<b>obsolete</b> : Empty string constant<br/>Please use EmptyString") = String.MinValue;

// --- StringEnumerator ---

sealed class StringEnumerator : Enumerator<char>,
  info("A UTF-8 string enumerator")
{
   string         literal;
   Reference<int> index;
   Reference<int> diff;
   char           char;  

   constructor(string s)
      : info("Creates an enumerator to the specified <b>s</b> argument")
   {
      literal := s;
      index := -1;
      diff := 1;
      char := $0;
   }

   reset()
      : info("Resets the enumerator")
   {
      index.Value := -1;
      diff.Value := 1
   }

   bool next()
      : info("Goes to the next enumeration member and returns true if it is not the last member")
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

   Enumerator cast() 
      : info("Creates a weak enumerator")
   = new Enumerator
   {
      embeddable dispatch() => self;
   };
   
   enumerable() : info("returns the enumerating string")
      = literal;

   char Value : info("Returns the current character")
      = char;
}

// --- StringIndexer ---

sealed class StringIndexer : Indexer<char>,
   info("A UTF-8 string indexer") 
{
   string         literal;
   Reference<int> index;

   constructor(string s)
      : info("Creates an indexer to the specified <b>s</b> argument")
   {
      literal := s;
      index := 0;
   }

   get bool Available()
      : info("Returns true if there is an element with a given index in the corresponding collection<br/>Otherwise returns false")
   {
      int len := literal.Length;
      int val := *index;
        
      ^ val < len
   }
   
   get char Value()
      : info("Returns the indexed character")
   {
      int val := *index;
      char ch := literal[val];

      ^ ch
   }
   
   set Value(char ch)
      : info("Not supported. Raises the exeception")
   {
      system'NotSupportedException.raise()
   }

   int Index : info("Index property")
   {
      get()
      {
         int retVal := *index;
            
         ^ retVal
      }
        
      set(val)
      {
         index.Value := val
      }
   }
   
   appendIndex(int val)
      : info("Increases the current index by parameter **index**")
   {
      index.append(val)
   }
    
   int Length : info("Returns the length of the indexed string")
   {
      get()
      {
         int retVal := literal.Length;
            
         ^ retVal
      }
   }
        
   Indexer cast() : info("Creates a weak indexer") = new Indexer{ embeddable dispatch() => self; };   
}

// --- WideString ---

public const struct WideString : BaseValue,
   info("A UTF-16 literal value")
{
   __string short[] array;

   const static wide MinValue
        : info("Returns an empty string")
      = ""w;

   constructor allocate(int size)
        : info("creates a string with specific <b>size</b>")
      = new short[](size + 1);

   constructor copy(int size, wide s)
        : info("Creates the copy of a substring of <b>s</b> with specified length <b>size</b> starting at 0")
      <= allocate(size)
   {
      PrimitiveWideStringOperations.copy(array, 0, size, s);

      // add tailing zero
      short z := 0;
      array[size] := z;
   }

   /// Creates the literal string with specified length fills with specified character value        
   constructor fill(int size, char ch)
        : info("Creates the literal string with specified length <b>size</b> fills with character <b>ch</b>")
      <= allocate(ch.Length * size)
   {
      short buffer[4];
      int chVal := ch.Value;
      int  length := ch.Length;
      length := UTFOperations.convertUTF32_2_16(chVal, buffer, 0, length);
        
      int offs := 0;
      for (int i := 0; i < size; i++)
      {
         for (int j := 0; j < length; j := j + 1) {
            array[offs] := buffer[j];
            offs := offs + 1;
         }
      };

      // add tailing zero
      short z := 0;
      array[offs] := z;
   }

   /// Creates the literal string copy              
   constructor copy(wide s)
       : info("Creates the literal string copy of <b>s</b>")
      <= copy(s.Length, s);

   /// Creates the copy of a subarray        
   constructor copy(int index, int length, char[] charArray)
        : info("Creates the copy of a sub array <b>charArray</b> starting from <b>index</b> and with the specified <b>length</b>")
      <= allocate(UTFOperations.calcSubWideStringLength(index, length, charArray))
   {
      PrimitiveArrayOperations.copyCharArray(charArray, index, length, this array);

      // add tailing zero
      int len := self.Length;
      short z := 0;
      array[len] := z;
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
      : info("Converts to UTF-8 string")
   {
      int        i := 0;
      int        m := 0;
      int        page := 0;
      int        l := self.Length;
      byte buffer[256];            
      if (l < 128)
      {
         m := ByteArrayConvertor.convert(self, l, buffer, 0, 256);

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

         m := ByteArrayConvertor.convert(self.Substring(i,page), page, buffer, 0, 256);

         retVal := retVal.add(String.fromByteArray(0, m, buffer));

         i := i + page;
         l := l - page
      };
        
      ^ retVal      
   }

   Enumerable<char> cast() : info("Creates an enumerable wrapper")
      = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

   string toPrintable() 
        : info("Returns UTF-8 copy of the string")
      = self;

   private copySubS(wide dest, wide sour, int index, int length) : external(system'core_routines'__wstr_copysubs);
   private copySubS(wide dest, wide sour, int index, int length, int sourIndex) : external(system'core_routines'__wstr_xcopysubs);

   bool equal(o) 
      : info("Tries to convert the argument <b>o</b> to UTF-16 string and compare with it. If the conversion is not possible it returns false")
   {
      if (o; is wide w)
      {
         ^ self.equal(w)
      };

      ^ false
   }

   bool less(o)
      : info("Tries to convert the argument <b>o</b> to UTF-16 string and check if the object is less then it. If the conversion is not possible it returns false")
   {
      if (o; is wide w)
      {
         ^ self.less(w)
      };

      ^ false
   }

   bool equal(wide s) : external(system'core_routines'__wstr_equal), info("Returns true if the string equals to <b>s</b>; otherwise returns false");

   bool less(wide s) : external(system'core_routines'__wstr_less), info("Returns true if the string is less then <b>s</b>; otherwise returns false");

   bool equal(char ch) : info("Returns true if the string equals to <b>ch</b>; otherwise returns false<br/>Note the character will be converted to UTF-16 string before comparison")
      <= equal(cast wide(ch));

   bool less(char ch) : info("Returns true if the string is less then <b>ch</b>; otherwise returns false<br/>Note the character will be converted to UTF-16 string before comparison")
      <= less(cast wide(ch));

   char at(int index)
      : info("Returns a character at the specified position <b>index</b>")
   {
      int len := self.Length;
      if (index < 0 || index >= len)
      {
         OutOfRangeException.raise()
      };

      UTFOperations.convertUTF16_2_32(array, index, len, out int retVal);

      char ch := retVal;

      ^ ch;
   }

   retoverload short at(int index)
         : info("Returns a word at the specified position <b>index</b>")
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
      : info("Saves the index of the first occurence of the specified character <b>ch</b> starting from <b>index</b> into an output parameter.<br/>If the substring is not found. returns -1")
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
        : info("Inserts the character <b>ch</b> at the specified <b>index</b> and returns the new one")
      = self.insert(index, cast wide(ch));

   wide delete(int index, int length)
      : info("Deletes the substring starting from <b>index</b> and with the specified <b>length</b> from the object and returns the new one")
   {
      int len1 := self.Length;
      int sindex := index + length;
      wide retVal := WideString.allocate(len1-length);

      self.copySubS(retVal, self, 0, index);
      self.copySubS(retVal, self, index, len1 - sindex, sindex);
    
      ^ retVal;         
   }

   wide Substring(int index, int length)
      : info("Returns the substring from <b>index</b> and with the specified <b>length</b>")
   {
      ^ WideString.fromShortArray(index, length, array);
   }

   int save(int index, int length, char[] buffer, int destIndex)
        : info("Copies the characters from the string starting from <b>index</b> and with the specified <b>length</b> and copies to <b>buffer</b> starting from <b>destIndex</b>")
      = PrimitiveWideStringOperations.save(buffer, self, index, length, destIndex);

   internal saveToShortArray(int index, short[] buffer, int length, int outputIndex)
      = PrimitiveWideStringOperations.save(buffer, outputIndex, this array, index, length);

   Enumerator<char> enumerator()
         : info("Returns the string enumerator")
      = new WideStringEnumerator(self);

   Indexer<char> indexer() 
         : info("Returns the string indexer")
      = new WideStringIndexer(self);
    
   wide clone() : info("Clones the value")
      = WideString.copy(self);

   int Length : info("Returns the string length")
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

public const WideString EmptyWideString : info("Empty string constant")
   = WideString.MinValue;

public const WideString emptyWideString : info("<b>obsolete</b> : Empty string constant<br/>Please use EmptyWideString") = WideString.MinValue;

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
       : info("Creates a variant object and assigns it with <b>object</b>")
    {
        value := object
    }
    
    byte cast() : info("Tries to convert the value to a byte number")
        = ByteConvertor.convert(value);

    short cast() : info("Tries to convert the value to a short integer number")
        = ShortConvertor.convert(value);

    char cast() : info("Tries to convert the value to a character")
        = CharConvertor.convert(value);

    int cast() : info("Tries to convert the literal value to the integer number")
        = IntConvertor.convert(value);

    long cast() : info("Tries to convert the literal value to the long integer number")
        = LongConvertor.convert(value);

    real cast() : info("Tries to convert the literal value to the real number")
        = RealConvertor.convert(value);

    string cast() : info("Returns a literal value")
        = StringConvertor.convert(value);
    
    wide cast() : info("Returns a wide literal value")
        = WideConvertor.convert(value);

    string toPrintable() : info("Redirects to the assigned object")
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

    saveTo(target) : info("Tries to convert <b>target</b> value to the type compatible with the variant value, reassigns it and returns the argument.<br/>Target must support Value property")
    {
        target.Value := self.castValueTo(target.Value);
        
        ^ target
    }

    add(p) : info("Tries to convert <b>p</b> argument to the variant type and returns the sum")
        = self.add(self.castValueTo(p));

    subtract(p) : info("Tries to convert <b>p</b> argument to the variant type and returns the difference")
        = self.castValueTo(p).subtract(p);

    multiply(p) : info("Tries to convert <b>p</b> argument to the variant type and returns the product")
        = self.castValueTo(p).multiply(p);

    divide(p) : info("Tries to convert <b>p</b> argument to the variant type and returns the division result")
        = self.castValueTo(p).divide(p);

    bool equal(p) : info("Tries to convert <b>p</b> argument to the variant type and check if they are equals")
        = self.castValueTo(p).equal(p);
}

// --- Array ---

public sealed Array : Indexable, info("A generic array")
{
   __string object[] array;
    
   constructor MinValue : info("Returns an array with a length 0")
      = new object[](0);
    
   constructor allocate(int len) : info("Creates the array with the specified length")
      = new object[](len);

   constructor copy(int index, int length, Array array) : info("Creates a copy of the provided sub <b>array</b> starting at <b>index</b> and the specified <b>length</b>")
      <= allocate(length)
   {
      for(int i := 0; i < length; i += 1)
      {
         this array[i] := array[i + index]
      }
   }
        
   add(Array a) : info("Concatinates the arrays")
   {
      int length1 := self.Length;
      int length2 := a.Length;
        
      auto retVal := Array.allocate(length1 + length2);

      for (int i:=0; i < length1; i += 1) {
         retVal[i] := array[i]
      };

      for (int i:=0; i < length2; i += 1) {
         retVal[length1 + i] := a[i]
      };
            
      ^ retVal
   }

   int Length : info("Returns the array length")
   {
      get()
      {
         int len := $len array;
            
         ^ len
      }
   }

   setAt(int n, object o)
      : info("Assigns <b>item</b> to the element at the position <b>n</b>")
   {
      array[n] := o
   }

   object at(int n)
        : info("Returns an element at the position <b>n</b>")
     = array[n];
 
   Enumerator enumerator() : info("Returns the array enumerator")
      = new ArrayEnumerator(self);

   Indexer indexer() : info("Returns the array indexer")
      = new ArrayIndexer(self);

   object[] Value : info("Returns itself")
      = array;

   Array clone() : info("Creates a shallow copy of the array")
      = Array.copy(0, self.Length, self);
}

// --- ArrrayEnumerator ---

public sealed ArrayEnumerator : Enumerator, info("A generic array enumerator")
{
   Array          array;
   Reference<int> index;

   constructor(Array arr)
      : info("Creates an enumerator to the specified <b>s</b> argument")
   {
      array := arr;
      index := -1;
   }

   reset()
      : info("Resets the enumerator")
   {
      index.Value := -1;
   }

   bool next()
      : info("Goes to the next enumeration member and returns true if it is not the last member")
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

   Value : info("Returns the current element")
      = array[*index];

   enumerable() : info("returns the enumerating array")
      = array;
}

// --- ArrayIndexer ---

public sealed ArrayIndexer : Indexer, info("A generic array indexer")
{
   Array          array;
   Reference<int> index;

   constructor(Array arr)
      : info("Creates an indexer to the specified <b>arr</b> argument")
   {
      array := arr;
      index := 0;
   }

   get bool Available()
      : info("Returns true if there is an element with a given index in the corresponding collection<br/>Otherwise returns false")
   {
      int len := array.Length;
      int val := *index;
        
      ^ val < len
   }
   
   get Value()
      : info("Returns the indexed element")
   {
      int val := *index;

      ^ array[val];
   }
   
   set Value(o)
      : info("Set the element at the current index")
   {
      int val := *index;

      array[val] := o;      
   }

   int Index : info("Index property")
   {
      get()
      {
         int retVal := *index;
            
         ^ retVal
      }
        
      set(val)
      {
         index.Value := val
      }
   }
   
   appendIndex(int val)
      : info("Increases the current index by parameter **index**")
   {
      index.append(val)
   }
    
   int Length : info("Returns the length of the indexed array")
   {
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

#if (__project["_Win"]) // Windows

public const String NewLineConstant : info("New line characters")
    = $13$10;

#elif (__project["_Linux"] || __project["_FreeBSD"])

public const string NewLineConstant : info("A new line character")
    = ""$10;

#endif
