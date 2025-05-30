import system'io;
import system'net;
import system'text;
import system'threading;
import system'routines;
import system'culture;
import extensions;

public sealed class HttpResponse
{
   NetworkStream _stream;
   MemoryBuffer  _buffer;

   HttpHeaders   _headers;

   string        _protocol;
   int           _statusCode;
   string        _statusText;

   string StatusText
   {
      get()
      {
         ^ _statusText;
      }
   }
   
   int StatusCode
   {
      get()
      {
         ^ _statusCode;
      }
   }   

   HttpHeaders Headers
   {
      get()
      {
         ^ _headers;
      }
   }

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
      for (int i := 0; i < length; i++) {
         if (buffer[start + i] == 58) {
            ^ start + i + 1;
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

   private parseHeaders(int eoh)
   {
      byte[] buffer := _buffer.Content;
      int length := _buffer.Length;

      int index := 0;
      // status 
      int eol := seekEOL(buffer, 0, length);      
      var line := UTF8Encoding.toString(index, eol - index - 2, buffer);

      var elements := line.split();
      _protocol := elements[0];
      _statusCode := elements[1].toInt();
      _statusText := elements[2];

      index := eol;

      while (index < eoh) {
         int next := seekEOL(buffer, index, length);
         if (next - index == 2)
            :break;

         int headerContent := seekHeaderContent(buffer, index, next - index - 2);
         string key := UTF8Encoding.toString(index, headerContent - index - 1, buffer).trimRight();
         if (buffer[headerContent + 1] == 32)
            headerContent++;

         string value := UTF8Encoding.toString(headerContent + 1, next - headerContent - 3, buffer);

         _headers.add(key, value);

         index := next;
      }
   }

   async internal Task readHeaderAsync()
   {
      int eoh := 0;
      int received := :await _stream.readAsync(_buffer, 512);
      while (received > 0) {
         eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := :await _stream.readAsync(_buffer, 512);
      };

      parseHeaders(eoh)
   }

   internal readHeader()
   {
      int eoh := 0;
      int received := _stream.readToBuffer(_buffer, 512);
      while (received > 0) {
         eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := _stream.readToBuffer(_buffer, 512);
      };

      parseHeaders(eoh)
   }

   private string returnContentAsString()
   {
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
            string chunkStr := UTF8Encoding.toString(current, eol - current - 2, buffer);
            int chunkSize := chunkStr.toUpper().toInt(16);

            if (chunkSize == 0)
               :break;

            output.write(eol, chunkSize, buffer);

            current := eol + chunkSize + 2;
         };

         ^ UTF8Encoding.toString(0, output.Length, output.Content);
      }
      else {
         ^ UTF8Encoding.toString(eoh, length - eoh, _buffer.Content);
      }      
   }

   async Task<string> readAsStringAsync()
   {
      int received := :await _stream.readAsync(_buffer, 512);
      while (received > 0) {
         received := :await _stream.readAsync(_buffer, 512);
      };

      ^ returnContentAsString();
   }

   string readAsString()
   {
      int received := _stream.readToBuffer(_buffer, 512);
      while (received > 0) {
         received := _stream.readToBuffer(_buffer, 512);
      };

      ^ returnContentAsString();
   }
}