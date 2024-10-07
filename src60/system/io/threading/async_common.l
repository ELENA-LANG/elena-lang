import system'io;
import system'threading;

public extension asyncStreamOp : Stream
{
   Task writeAsync(byte[] dump, int length)
      = Task.run( { self.write(dump, length); });

   Task<int> readAsync(byte[] dump, int length)
   {
      ^ class Task<int>.run( { ^ self.read(dump, length); });
   }
}