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
import system'threading;
import system'net;
import system'io;

public sealed class HttpBody
{
   NetworkStream _stream;
   MemoryBuffer  _buffer;
   
   constructor(NetworkStream stream)
   {
      _stream := stream;
      _buffer := MemoryBuffer.allocate();
   }
   
   // !! must be internal
   async /*internal*/ Task readHeaderAsync(/*HttpFirstHeader firstLine, HttpHeaders headers*/)
   {
      byte[] dump := new byte[](512);
      
      Console.writeLine("readHeaderAsync:");

      int eoh := 0;
      int received := :await _stream.readAsync(dump, 512);
         
      while (received > 0) {
         _buffer.write(0, received, dump);

         eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := 0;
         if (_stream.isDataAvailable) 
            received := :await _stream.readAsync(dump, 512);
      };
   }

   private int seekHeaderEnd()
      = 0;
}

singleton Tester
{
   async Task<int> getValueAsync(byte[] tmp)
   {
      tmp[1] := 2;

      ^ 3
   }

   async Task<int> addValueAsync()
   {
      byte tmp[512];
      int val := :await getValueAsync(tmp);

      int n := tmp[1];

      val := val + n;

      ^ val
   }
}

public program()
{
   int n := Tester.addValueAsync().Result;
   Console.printLine("the result is ", n);
}
