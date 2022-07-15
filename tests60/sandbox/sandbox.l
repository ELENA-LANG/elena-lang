import system;

A
{
   test2()
       <= test1();

   test1()
   {
      console.writeLine("A.test1");
   }
}

public const struct XString : BaseValue
{
    __string byte[] theArray;

    string toPrintable() = self;

    constructor allocate(int size)
        = new byte[](size + 1);

//    /*internal */constructor fromByteArray(int index, int size, byte[] buffer)
//        <= allocate(size)
//    {
//        extern elenart.core_subcopyz(self, index, size, buffer);
//    }
}

singleton Tester
{
   run()
   {
      var a := new A();
      a.test2();

//       var s := XString.allocate(3);

//       byte b := 55;
//       byte z := 0;

//       byte tmp[10];
//       tmp[0] := b;
//       b := tmp[0];
//       tmp[1] := b;
//       tmp[2] := b;
//       tmp[3] := z;


//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
