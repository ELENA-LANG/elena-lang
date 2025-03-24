import extensions;

import system'collections;

/*
public singleton FibonacciGenerator
{
   yield Enumerator enumerator()
   {
      long n_2 := 1l; 
      long n_1 := 1l;

      $yield n_2;             
      $yield n_1;

      while(true)
      {
         long n := n_2 + n_1;

         $yield n;

         n_2 := n_1;
         n_1 := n
      }
   }
}

public program()
{
   auto e := FibonacciGenerator.enumerator();
    
   int n := console.print("Enter the length of fibonacci series:").readLine().toInt();
   for(int i := 0; i < n; i++) {
      e.next();

      console.printLine(*e)
   };
    
   console.readChar()
}
*/
/*
public class Tester
{
   field start;

   constructor()
   {
      start := 1;
   }

   yield Enumerator enumerator()
   {
      var previous := nil;
      var current := start + 1;

      while (true) {
         Console.printLine("The previous is ", previous ?? "<nil>");

         :yield current;      
   
         previous := current;
         current := current + 1;
      };
   }
}
*/

public A
{
   object _sync;

   List<int>      _array;
   Reference<int> _top;
   Reference<int> _tale;
     
   push(int object)
   {
      lock(_sync) {
         int capacity := _array.Length;
         int tale := *_tale;
            
         if (tale < capacity)
         { 
            _array.setAt(tale, object) 
         }
         else
         { 
            _array.append(object) 
         };
                
         _tale.append(1)
      }
   }
}

public program()
{
   var o := new Object();
   try
   {
      o.fail()
         //o.toPrintable()
   }
/*   catch::
   {
      function(Exception err)
      {
         console.writeLine(err);
      }
   }*/
   finally
   {
      console.writeLine("Finally triggered")
   }
}