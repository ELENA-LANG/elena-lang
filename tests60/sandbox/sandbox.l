import extensions;
 
A
{
   int x := 0;

   yieldable function()
   {
      var n := 0;

      while (true) {
         n := n + 1;

         $yield n;
      }
   }
}

public program()
{
   A a := new A();

   for(int i := 0, i < 200, i += 1) 
      console.printLine(a());
}