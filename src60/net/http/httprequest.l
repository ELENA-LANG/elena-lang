import system'io;
import system'text;

public sealed class HttpRequest
{
   MemoryBuffer  _buffer;

   HttpHeaders   _headers;

   HttpHeaders Headers
   {
      get()
      {
         ^ _headers;
      }
   }
   
   int ContentLength
      = _buffer.Length;
   
   string Content
      = UTF8Encoding.toString(0, _buffer.Length, *_buffer);
   
   constructor new(byte[] body, int length)
   {
      _buffer := MemoryBuffer.allocate();

      _headers := new HttpHeaders();
      
      _buffer.write(0, length, body);
   }
}