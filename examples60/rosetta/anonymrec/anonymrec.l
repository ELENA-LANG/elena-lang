/// Anonymous recursion

import extensions;

fib(n)
{
   if (n < 0)
      { InvalidArgumentException.raise() };
        
   ^ (n) {
      if (n > 1)
      { 
         ^ this self(n - 2) + (this self(n - 1))
      }
      else
      { 
         ^ n 
      }
   }(n)
}

public program()
{
   for (int i := -1; i <= 10; i += 1) 
   {
      console.print("fib(",i,")=");
      try
      {
         console.printLine(fib(i))
      }
      catch(Exception e)
      {
         console.printLine("invalid")
      }
   };
    
   console.readChar()
}
