import system;

singleton Tester
{
   run()
   {
       byte b := 55;
       byte tmp[5];
       tmp[0] := b;
       tmp[1] := b;
       tmp[2] := b;

       var s := String.fromByteArray(0,3,tmp);
   }
}

public program() 
{
   Tester.run();
}
