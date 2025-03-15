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

   private int seekHeaderEnd(byte[] buffer, int length)
   {
      int eol := seekEOL(buffer, 0, length);
      if (eol > 0 && length - eol > 2) {
         if (buffer[eol]==13 && buffer[eol+1]==10) {
            ^ eol + 2;
         };

         int start := eol;
         while (start < length) {
            eol := seekEOL(buffer, start, length);
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
      byte buffer[512];
      int received := :await _stream.readAsync(buffer, 512);      
      _buffer.write(0, received, buffer);

      int eol := seekHeaderEnd(buffer, received);
      while (eol == 0) {
         received := :await _stream.readAsync(buffer, 512);
         _buffer.write(0, received, buffer);

         eol := seekHeaderEnd(_buffer.Value, _buffer.Length);
      }
   }

   async Task<string> readAsStringAsync()
   {
      byte buffer[512];
      int received := :await _stream.readAsync(buffer, 512);
      while (received > 0) {
         _buffer.write(0, received, buffer);

         received := :await _stream.readAsync(buffer, 512);
      };

      int eoh := seekHeaderEnd(_buffer.Value, _buffer.Length);
      
      ^ UTF8Encoding.toString(eoh, buffer.Length - eoh, _buffer.Value);
   }
}