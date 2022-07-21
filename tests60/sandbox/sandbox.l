import system;

/*internal*/ singleton PrimitiveOperations
{
   copy(byte[] dest, int index, int size, byte[] sour)
   {
      for (int i := 0, i < size, i := i + 1) {
         dest[i] := sour[index + i];
      }
   }
}

public struct XString
{
    __string byte[] theArray;

    /*internal */constructor fromByteArray(int index, int size, byte[] buffer)
        <= allocate(size)
    {
       PrimitiveOperations.copy(theArray, index, size, buffer);
    }

    string toPrintable() = self;

    constructor allocate(int size)
        = new byte[](size + 1);
}

singleton Tester
{
   run()
   {
       var s := XString.allocate(3);

//       byte b := 55;
//       byte z := 0;

//       byte tmp[10];
//       tmp[0] := b;
//       b := tmp[0];
//       tmp[1] := b;
//       tmp[2] := b;
//       tmp[3] := z;
   }
}

public program() 
{
   Tester.run();
}
