import net;
import net'http;
import system'io;
import system'net;
import system'text;
import system'threading;

public sealed class HttpServerRequest
{
   HttpBody          _body;
   
   HttpFirstHeader   _firstLine;
   HttpHeaders       _headers;
   
   string Protocol
   {
      get()
      {
         ^ _firstLine.Argument3;
      }
   }
   
   string Method
   {
      get()
      {
         ^ _firstLine.Argument1;
      }
   }
   
   string Path
   {
      get()
      {
         ^ _firstLine.Argument2;
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

   Task<string> readAsStringAsync()
      = _body.readAsStringAsync(_headers);

   string readAsString()
      = _body.readAsString(_headers);
}