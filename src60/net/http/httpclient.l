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

   private Task<Socket> openAsync(Uri uri)
      = ::Task<Socket>.run({ ^ Socket.connect(uri.Host, uri.Port) });

   async Task<HttpResponse> getAsync(Uri uri)
   {
      /* Connect */
       _socket := :await openAsync(uri);

      /* Send headers to server */
      NetworkStream stream := new NetworkStream(_socket);

      string host := uri.Host;
      string headers := _headers.Value;
      string request := $"GET / HTTP/1.1"$13$10"Host:{host}"$13$10"Connection:close"$13$10"{headers}"$13$10;

      :await stream.writeAsync(request);

      HttpResponse response := HttpResponse.assign(stream);

      :await response.readHeader();

      ^ response;
   }

   close()
   {
      if:not:nil(_socket)
         _socket.close()
   }
}