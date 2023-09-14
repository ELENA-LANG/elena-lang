import extensions;
 
A
{
   int x := 0;

   yieldable function()
   {
      int n := 0;

      while (true) {
         x := x + 1;

         $yield x;
      }
   }
}

public program()
{
   A a := new A();

   for(int i := 0, i < 200, i += 1) 
      console.printLine(a());
}