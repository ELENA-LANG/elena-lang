import system;

public struct XString
{
    __string byte[] theArray;

    /*internal */constructor fromByteArray(/*int index, int size, */byte[] buffer)
        <= allocate(size)
    {
       byte b;
       /*theArray[0]*/b := buffer[0];

//       for (int i := 0, i < size, i := i + 1) {
//          theArray[i] := buffer[index + i];
//       }

//        extern elenart.core_subcopyz(self, index, size, buffer);
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

//   for (int n := 0, n < 5, n := n + 1) {
//      console.write("*");
//   }

   }
}

public symbol program = Tester.run();
