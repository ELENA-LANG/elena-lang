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

   async Task<HttpResponse> getAsync(Uri uri)
   {
      /* Create TCP socket */
      _socket := new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      /* Set remote->sin_addr.s_addr */
      SOCKADDR_IN remote := default;   

      remote.Ip_address := uri.IP;
      remote.Port := uri.Port;

      /* Connect */
      _socket.open(remote);

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