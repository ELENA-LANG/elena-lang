import extensions;

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

public program()
{
//   var a := "www.a.b";
   var b := "Http:Agent"$13$10;

   console.writeLine(b);

//   var s:= $"GET / HTTP/1.1\r\nHost:{uri.Host}"$13$10"Connection:close"$13$10"{_headers.Value}"$13$10;
}