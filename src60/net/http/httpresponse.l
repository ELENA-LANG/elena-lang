import system'io;
import system'net;
import system'text;
import system'threading;
import extensions;

public class HttpResponse
{
   NetworkStream _stream;
   MemoryBuffer  _buffer;

   HttpHeaders   _headers;

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

   private int seekHeaderContent(byte[] buffer, int start, int length)
   {
      for (int i := start; i < length; i++) {
         if (buffer[i] == $58) {
            ^ i + 1;
         }
      };

      ^ start + length
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

      _headers := new HttpHeaders();
   }

   async internal Task readHeader()
   {
      int eoh := 0;
      int received := :await _stream.readAsync(_buffer, 512);
      while (received > 0) {
         eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := :await _stream.readAsync(_buffer, 512);
      };

      byte[] buffer := _buffer.Content;
      int length := _buffer.Length;

      int index := 0;
      while (index < eoh) {
         int next := seekEOL(buffer, 0, length);

         int headerContent := seekHeaderContent(buffer, index, next - index - 2);
         string key := UTF8Encoding.toString(index, headerContent - index - 2, buffer);
         string value := UTF8Encoding.toString(headerContent, next - headerContent - 2, buffer);

         _headers.add(key, value);
      }
   }

   async Task<string> readAsStringAsync()
   {
      int received := :await _stream.readAsync(_buffer, 512);
      while (received > 0) {
         received := :await _stream.readAsync(_buffer, 512);
      };

      byte[] buffer := _buffer.Content;
      int length := _buffer.Length;
      int eoh := seekHeaderEnd();
      if (eoh == 0)
         HttpException.raise("Invalid response");

      if (_headers["Transfer-Encoding"]=="chunked") {
         int current := eoh;
         auto output := MemoryBuffer.allocate();
         while (current < length) {
            int eol := seekEOL(buffer, current, length);
            string chunkStr := UTF8Encoding.toString(current, eol - 2, buffer);
            int chunkSize := chunkStr.toInt();

            if (chunkSize == 0)
               :break;

            output.write(eol, chunkSize, buffer);

            current := eol + chunkSize;
         };
         
         ^ UTF8Encoding.toString(0, output.Length, output.Content);            
      }
      else {
         ^ UTF8Encoding.toString(eoh, length - eoh, _buffer.Content);
      }
      
   }
}