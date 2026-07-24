import extensions;

singleton Helper
{
   int test(real y)
   {
      real z := 2;
      if (y == 0) {
         ^ 3;
      };
      
      ^ 2
   }
}

public Program()
{
   int n := Helper.test(2)
}


