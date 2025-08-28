import net'http;
import system'io;
import system'net;
import system'text;
import system'threading;

public sealed class HttpServerResponse
{
   string Protocol : prop(_protocol);
   
   int StatusCode : prop(_statusText);
   
   string StatusText : prop(_statusCode);
   
   HttpHeaders Headers : rprop(_headers);
   
   MemoryBuffer  _buffer;
   
   constructor new(string content)
      <= new(UTF8Encoding.toByteArray(content), content.Length);
   
   constructor new(byte[] body, int length)
   {
      _headers := new HttpHeaders();
      _buffer := MemoryBuffer.allocate();
      
      //Base64Encoding.encode(0, length, body, _buffer);
      _buffer.write(0, length, body);
   }
   
   async Task saveContentAsync(NetworkStream stream)
   {
      auto output := new StringBuilder();
      
      output.write(_protocol);
      output.write($32);
      output.write(_statusCode);
      output.write($32);
      output.write(_statusText);
      output.write($13$10);
      
      _headers.saveTo(output);
      output.write($13$10);

      Console.writeLine(*output);
      var s := UTF8Encoding.toString(0, _buffer.Length, _buffer.Content);
      Console.writeLine(s);      
      
      :await stream.writeStringAsync(*output);

      :await stream.writeAsync(_buffer.Content, _buffer.Length);
   }
}