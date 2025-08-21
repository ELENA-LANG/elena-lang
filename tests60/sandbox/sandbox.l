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

public program()
{
    var t := 0;
//    int t2 := 0;
    Console.then(
    {
//        Assert.ifTrue(t == 0);
//        Assert.ifTrue(t2 == 0);
        
        t := 1;
//        t2 += 1;
    }).then(
    {
//        Assert.ifTrue(t == 1);
//        Assert.ifTrue(t2 == 1);
        
        t := 2;
//        t2 += 1;
    });
    Console.write(".");
/*
    Assert.ifTrue(t == 2);
    Assert.ifTrue(t2 == 2);
    Console.write(".");

    var mode := -1;
    var value := false;
    (value) ? { mode := 0 } ! { mode := 1 };
    
    Assert.ifTrue(mode == 1);
    Console.write(".");
    
    mode := -1;
    value := true;
    (value) ? { mode := 0 } ! { mode := 1 };
    
    Assert.ifTrue(mode == 0);

    Console.write(".")
*/
}
