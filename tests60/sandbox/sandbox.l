import extensions;
 
A
{
   int x := 0;

   yieldable function()
   {
      while (true) {
         x := x + 1;

         $yield x;
      }
   }
}

public program()
{
   A a := new A();

   console.printLine(a());
   console.printLine(a());
   console.printLine(a());
   console.printLine(a());
   console.printLine(a());
   console.printLine(a());
   console.printLine(a());
}