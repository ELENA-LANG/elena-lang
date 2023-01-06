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

   int Length
   {
      get()
      {
         int len := $len array;
            
         ^ len
      }
   }
}

// --- Variadic Array template ---

public sealed nonstructural VariadicArray<T> //: Indexable<T>
{
    embeddable __string T[] array;
}

// --- String ---

public const struct String : //BaseValue
   info<"A UTF-8 literal value">   
{
   __string byte[] array;

    const static string MinValue
        = "";

   string toPrintable() = self;

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

   bool equal(string s) : external<system'core_routines'__str_equal>;

   private copySubS(string dest, string sour, int index, int length) : external<system'core_routines'__str_copysubs>;

   char at(int index)
      = PrimitiveOperations.readChar(array, index);

   retoverload byte at(int index)
      = array[index];

   string add(string subs)
   {
      int l1 := self.Length;
      int l2 := subs.Length;
      string retVal := String.allocate(l1 + l2);

      self.copySubS(retVal, self, 0, l1);
      self.copySubS(retVal, subs, l1, l2);    

      ^ retVal
   }
}

// --- WideString ---

public const struct WideString //: BaseValue
{
    __string short[] theArray;

    int Length
    {
        /// Reads the literal length into an output parameter
        get()
        {
           int l := $len theArray;
           l := l - 1;

           ^ l;
        }
    }
}
