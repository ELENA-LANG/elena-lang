import system'net;
import system'threading;

public class HttpClient
{      
   HttpHeaders _headers;
   Socket      _socket;

   constructor()
   {
      _headers := new HttpHeaders();
   }

   HttpHeaders Headers
      = _headers;

   Task<HttpResponse> getAsync(string url)
      = getAsync(Uri.parse(url));

   async Task<HttpResponse> getAsync(Uri uri)
   {
      /* Connect */
       _socket := :await Socket.connectAsync(uri.Host, uri.Port);

      /* Send headers to server */
      NetworkStream stream := new NetworkStream(_socket);

      string request := $"GET / HTTP/1.1\r\nHost:{uri.Host}$1310Connection:close$1310{_headers.Value}$13$10";

      :await stream.writeAsync(request);

      HttpResponse response := HttpResponse.assign(stream);

      :await response.readHeader();

      ^ response;
   }

   close()
   {
      _socket.close()
   }
}