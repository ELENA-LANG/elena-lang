import system'net;
import system'threading;

public class HttpClient
{      
   HttpHeaders _headers;
   Socket      _socket;

   constructor new()
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

      :await stream.writeAsync(_headers.Value);

      ^ HttpResponse.assign(stream);
      
   }

   close()
   {
      _socket.close()
   }
}