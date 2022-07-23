import system;

singleton Tester
{
   run()
   {
      int n := 11;
      int r := 12;

      bool b := n >= r;
      console.writeLine(b);

//       var n := 3 * 2 * 3;

//       byte b := 55;
//       byte tmp[5];
//       tmp[0] := b;
//       tmp[1] := b;
//       tmp[2] := b;

//       var s := String.fromByteArray(0,3,tmp);

//       var s := stringConvertor.convert(3, 10);
//       console.writeLine(s);
   }
}

public program() 
{
   Tester.run();
}
