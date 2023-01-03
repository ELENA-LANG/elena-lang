// --- Array template ---

public sealed Array<T> //: Indexable<T>
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

//   Enumerator<T> enumerator() = new BaseEnumerator<T>
//   {
//      this index := -1;
        
//      this length := self.Length;
        
//      get T Value() = self.at(this index.Value);
        
//      enumerable() = self;
        
//      reset()
//      {
  //       this index.Value := -1
//      }
//   };
}

// --- Variadic Array template ---

public sealed nonstructural VariadicArray<T> //: Indexable<T>
{
    embeddable __string T[] theArray;
}

// --- String ---

public const struct String //: BaseValue
{
   __string byte[] theArray;

   string toPrintable() = self;

   constructor allocate(int size)
      = new byte[](size + 1);

   internal constructor fromByteArray(int index, int size, byte[] buffer)
      <= allocate(size)
   {
      PrimitiveOperations.copy(theArray, index, size, buffer);

      // add tailing zero
      byte z := 0;
      theArray[size] := z;
   }

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

   bool equal(string s) : external<system'core_routines'__str_equal>;

   private copySubS(string dest, string sour, int index, int length) : external<system'core_routines'__str_copysubs>;

   char at(int index)
      = PrimitiveOperations.readChar(theArray, index);

   retoverload byte at(int index)
      = theArray[index];

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
