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
import extensions;
//import system'threading;
//
//singleton Tester
//{
//   async Task<int> getValueAsync(byte[] tmp)
//   {
//      tmp[1] := 2;
//
//      ^ 3
//   }
//
//   async Task<int> addValueAsync()
//   {
//      byte tmp[512];
//      int val := :await getValueAsync(tmp);
//
//      int n := tmp[1];
//
//      val := val + n;
//
//      ^ val
//   }
//}
//
//public program()
//{
//   int n := Tester.addValueAsync().Result;
//   Console.printLine("the result is ", n);
//}

/*
struct XY
{
   int X : prop(_x);
   
   int Y : prop(_y);
}

singleton XYSetter
{
   setX(o, arg)
   {
      o.X := arg
   }
   
   
   setY(o, arg)
   {
      o.Y := arg
   }
}

public program()
{
   XY xy := default;
   
   XYSetter.setX(xy, 1).setY(xy, 2);
   
   Console.printLine("xy (", xy.X, ",", xy.Y);   
}
*/

use extern cxx : importing("libc++");

public program()
{
    var mangled := "_Z3fooi";
    int status;
    pointer demangled := cxx.__cxa_demangle(mangled, nil, nil, ref status);
    
    Console.printLine(demangled as:string);
}
