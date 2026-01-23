import system'text;
import system'io;

namespace text
{
   public extension EncoderOp : Encoder
   {
      byte[] toByteArray(int start, int length, string s)
      {
         MemoryBuffer output := MemoryBuffer.allocate(length);
         
         byte tmp[1024];
         int tmpLen := 1024;
         int total := length;
         int index := start;
         int pageLen := 1024;
         while (total > 0) {
            if (pageLen > total)
               pageLen := total;
            
            int converted := self.saveToByteArray(s, index, ref pageLen, tmp, 0, 1024);
            total -= pageLen;
            index += pageLen;
            
            output.write(0, converted, tmp)
         };
         
         ^ *output
      }
   }
}