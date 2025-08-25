import system'io;
import system'net;
import system'text;
import system'threading;
import system'routines;
import system'culture;
import extensions;

// !! must be internal
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
   async /*internal*/ Task readHeaderAsync(HttpFirstHeader firstLine, HttpHeaders headers)
   {
      int eoh := 0;
      int received := :await _stream.readAsync(_buffer);
         
      while (received > 0) {
         eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := 0;
         if (_stream.isDataAvailable) 
            received := :await _stream.readAsync(_buffer);
      };

      // !! temporal
      var s := UTF8Encoding.toString(0, _buffer.Length, _buffer.Content);
      Console.writeLine(s);      

      parseHeaders(firstLine, headers, eoh)
   }

   // !! must be internal
   /*internal*/ readHeader(HttpFirstHeader firstLine, HttpHeaders headers)
   {
      int eoh := 0;
      int received := :await _stream.readAsync(_buffer);
         
      while (received > 0) {
         eoh := seekHeaderEnd();
         if (eoh > 0)
            :break;

         received := 0;
         if (_stream.isDataAvailable) 
            received := _stream.read(_buffer);
      };

      parseHeaders(firstLine, headers, eoh)
   }

   // !! must be internal
   async /*internal*/ Task<string> readAsStringAsync(HttpHeaders headers)
   {
      int received := 0;
      if (_stream.isDataAvailable) 
         received := :await _stream.readAsync(/*dump, 512*/_buffer);
         
      while (received > 0) {
         received := 0;
         if (_stream.isDataAvailable) 
            received := :await _stream.readAsync(/*dump, 512*/_buffer);
      };

      ^ returnContentAsString(headers);
   }

   // !! must be internal
   /*internal*/ string readAsString(HttpHeaders headers)
   {
      byte dump[512];
      int received := 0;
      if (_stream.isDataAvailable) 
         received := _stream.read(dump, 512);
         
      while (received > 0) {
         _buffer.write(0, received, dump);

         received := 0;
         if (_stream.isDataAvailable) 
            received := _stream.read(dump, 512);
      };

      ^ returnContentAsString(headers);
   }   
   
   private string returnContentAsString(HttpHeaders headers)
   {
      byte[] buffer := _buffer.Content;
      int length := _buffer.Length;
      int eoh := seekHeaderEnd();
      if (eoh == 0)
         HttpException.raise("Invalid response");

      if (headers["Transfer-Encoding"]=="chunked") {
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
   
   private parseHeaders(HttpFirstHeader firstLine, HttpHeaders headers, int eoh)
   {
      byte[] buffer := _buffer.Content;
      int length := _buffer.Length;

      int index := 0;
      // status 
      int eol := seekEOL(buffer, 0, length);      
      var line := UTF8Encoding.toString(index, eol - index - 2, buffer);

      var elements := line.split();
      if (elements.Length != 3)
         InvalidOperationException.raise("Invalid format");
      
      firstLine.Argument1 := elements[0];
      firstLine.Argument2 := elements[1];
      firstLine.Argument3 := elements[2];

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

         headers.add(key, value);

         index := next;
      }
   }   

   private int seekSpace(byte[] buffer, int start, int length)
   {
      for (int i := start; i < length; i++) {
         if (buffer[i] ==32) {
            ^ i;
         }
      };
      
      ^ -1
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
      if (eol > 0 && length - eol >= 2) {
         if (buffer[eol]==13 && buffer[eol+1]==10) {
            ^ eol + 2;
         };

         int start := eol;
         while (start < length) {
            eol := seekEOL(buffer, start, length);
            if (eol > 0 && length - eol >= 2) {
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
}

public sealed class HttpResponse
{
   HttpBody          _body;
   
   HttpFirstHeader   _firstLine;
   HttpHeaders       _headers;

   string Protocol
   {
      get()
      {
         ^ _firstLine.Argument1;
      }
   }
   
   string StatusText
   {
      get()
      {
         ^ _firstLine.Argument3;
      }
   }
   
   int StatusCode
   {
      get()
      {
         ^ _firstLine.Argument2.toInt();
      }
   }   

   HttpHeaders Headers
   {
      get()
      {
         ^ _headers;
      }
   }

   internal constructor assign(NetworkStream stream)
   {
      _firstLine := new HttpFirstHeader();
      _headers := new HttpHeaders();
      _body := new HttpBody(stream);
   }

   internal Task readHeaderAsync()
      = _body.readHeaderAsync(_firstLine, _headers);

   internal readHeader()
      = _body.readHeader(_firstLine, _headers);

   async Task<string> readAsStringAsync()
      = _body.readAsStringAsync(_headers);

   string readAsString()
      = _body.readAsString(_headers);
}