//import extensions;
/*
singleton FibonacciEnumerable : Enumerable
{
   Enumerator enumerator()
      = FibonacciEnumerable.infinitEnumerator(2);

   yield Enumerator infinitEnumerator(int r)
   {
        long n_2 := 1l; 
        long n_1 := 1l;

        :yield n_2;             
        :yield n_1;

        while(true)
        {
            long n := n_2 + n_1;

            :yield n;

            n_2 := n_1;
            n_1 := n
        }
   }
}


public program()
{
    auto e := FibonacciEnumerable.enumerator();
    
    if(!e.next())
       InvalidOperationException.raise();
    
    for(int i := 0; i < 10 && e.next(); i += 1) {
        Console.printLine(*e)
    };
    
    Console.readChar()
}
*/


public program()
{
   int n := 2;
   int m := 3;
   if (n == 2 && m == 3) {
     Console.writeLine("n == 2 && m == 3");
   }
   else Console.writeLine("n == 2 && m == 3");
}
