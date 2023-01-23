import extensions;

public sealed Array //: Indexable
{
    __string object[] array;
    
    /// Creates a copy of the provided sub array
    constructor copy(int index, int length, Array array)
        <= allocate(length)
    {
      for(int i := 0 , i < length, i += 1)
      {
         this array[i] := array[i + index]
      }
    }
}

public program()
{
//   console.writeLine("Test from ELENAVM")
}




