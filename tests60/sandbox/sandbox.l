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
   var f1 := { Console.writeLine("The traditional lambda function without arguments"); };
   var f2 := (arg){ Console.writeLine($"The new lambda function with argument {arg}"); };
   var f3 := (){ Console.writeLine("The new lambda function without arguments to be in line with syntax like c(x){ ... }"); };

   f1();
   f2("value");
   f3();
}
