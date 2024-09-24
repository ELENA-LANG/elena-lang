import system'io;
import system'threading;

public extension asyncStreamOp : Stream
{
   Task writeAsync(byte[] dump, int length)
      = Task.Run( { self.write(dump, length); });

//   Task<int> readAsync(byte[] dump, int length)
//   {
//      ^ Task.Run( { self.read(dump, ref length); });
//   }   
}