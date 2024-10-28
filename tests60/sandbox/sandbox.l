import extensions;
import system'threading;

public sealed const struct Socket
{
   handle _handle;

   int write(byte[] buffer)
   {
      int len := buffer.Length;

//      ^ self.write(buffer, len, 0);
   
      ^ len
   }

   Task<int> writeAsync(byte[] buffer)
      = ::Task<int>.run(
      {
         int written := write(buffer);

         ^ written
      });
}

public program()
{
}