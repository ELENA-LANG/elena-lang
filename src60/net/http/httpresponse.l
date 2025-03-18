import system'io;
import system'net;
import system'text;
import system'threading;

public class HttpResponse
{
   NetworkStream _stream;
   MemoryBuffer  _buffer;

   private int seekEOL(byte[] buffer, int start, int length)
   {
      for (int i := start; i < length; i++) {
         if (buffer[i] ==13) {
            if (i + 1 < length){
               if (buffer[i + 1] == 10) {
                  ^ i + 2;
               }
            }
         }
      };

      ^ 0
   }

   private int seekHeaderEnd()
   {
      byte[] buffer := _buffer.Content;

      int length := _buffer.Length;
      int eol := seekEOL(buffer, 0, length);
      if (eol > 0 && length - eol > 2) {
         if (buffer[eol]==13 && buffer[eol+1]==10) {
            ^ eol + 2;
         };

         int start := eol;
         while (start < length) {
            eol := seekEOL(buffer, start, length - start);
            if (eol > 0 && length - eol > 2) {
               if (buffer[eol]==13 && buffer[eol+1]==10) {
                  ^ eol + 2;
               };

               start := eol;
            }
            else :break;
         }         
      };

      ^ 0
   }

   internal constructor assign(NetworkStream stream)
   {
      _stream := stream;
      _buffer := MemoryBuffer.allocate();
   }

   async internal Task readHeader()
   {
      int received := :await _stream.readAsync(_buffer, 512);
      while (received > 0) {
         int eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := :await _stream.readAsync(_buffer, 512);
      };
   }

   async Task<string> readAsStringAsync()
   {
      int received := :await _stream.readAsync(_buffer, 512);
      while (received > 0) {
         received := :await _stream.readAsync(_buffer, 512);
      };

      int length := _buffer.Length;
      int eoh := seekHeaderEnd();
      if (eoh == 0)
         HttpException.raise("Invalid response");

      ^ UTF8Encoding.toString(eoh, length - eoh, _buffer.Content);
   }
}