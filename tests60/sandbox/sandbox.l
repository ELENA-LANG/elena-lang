//import extensions;
//
//singleton FibonacciEnumerable : Enumerable
//{
//   Enumerator enumerator()
//      = FibonacciEnumerable.infinitEnumerator(2);
//
//   yield Enumerator infinitEnumerator(int r)
//   {
//        long n_2 := 1l; 
//        long n_1 := 1l;
//
//        :yield n_2;             
//        :yield n_1;
//
//        while(true)
//        {
//            long n := n_2 + n_1;
//
//            :yield n;
//
//            n_2 := n_1;
//            n_1 := n
//        }
//   }
//}
//
//
//public program()
//{
//    auto e := FibonacciEnumerable.enumerator();
//    
//    if(!e.next())
//       InvalidOperationException.raise();
//    
//    for(int i := 0; i < 10 && e.next(); i += 1) {
//        Console.printLine(*e)
//    };
//    
//    Console.readChar()
//}

A
{
   int x := 3;
   
   testMe()
   {
      var off := 5;
      auto f := (int arg) 
         {
            int n := arg + x + off;
            
            Console.writeLine(n)
         };
         
      f(4);         
   }
}

public program()
{
   var a := new A();
   
   a.testMe();
}