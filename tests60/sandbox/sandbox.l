import extensions;
 
A
{
   yieldable long next()
   {
        long n_2 := 1l; 
        long n_1 := 1l;

        $yield n_2;             
        $yield n_1;


   }

   yieldable long next2() {}
}

public program()
{
   A a := new A();

   for(int i := 0, i < 2, i += 1) 
      console.printLine(a());
}