import system'net;

public class HttpClient
{
   constructor new()
   {
   }

   get(Uri uri)
   {
      /* Create TCP socket */
      auto socket := new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      /* Set remote->sin_addr.s_addr */
      SOCKADDR_IN remote := default;   

      remote.Ip_address := uri.IP;
      remote.Port := uri.Port;

      /* Connect */
      socket.open(remote);

      /* Send headers to server */
   }
}