
public sealed class HttpServerResponse
{
   string Protocol : prop(_protocol);
   
   int StatusCode : prop(_statusText);
   
   string StatusText : prop(_statusCode);
   
   HttpHeaders Headers : rprop(_headers);
   
   MemoryBuffer  _buffer;
   
   constructor new(byte[] body, int length)
   {
      _headers := new HttpHeaders();
      _buffer := MemoryBuffer.allocate();
      
      Base64Encoding.encode(0, length, body, _buffer);
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
      
      _await stream.writeAsync(*output);
      
      _await stream.writeAsync(_buffer.Content, _buffer.Length);
   }
}