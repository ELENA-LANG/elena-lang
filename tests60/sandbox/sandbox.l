import extensions;

singleton Tester
{
   doSomeTest() 
   {
      int j := 4;
      int tmp[3];
      int n := 4;

      tmp[0] := n;
   }
}

public Program()
{
   short l := 34;
   byte tmp[264];
   int n := 3;

   Tester.doSomeTest();
}