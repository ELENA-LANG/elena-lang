import system'net;
import system'threading;

public class SocketFactory
{
   static object _sync := new object();

   static INetSocketFactory _httpFactory;
   static INetSocketFactory _httpsFactory;

   static constructor()
   {
      _httpFactory := NativeSocketFactory.create();
   }

   static assignHttpsFactory(INetSocketFactory factory)
   {
      lock(_sync) {
         _httpsFactory := factory;
      }
   }

   static assignHttpFactory(INetSocketFactory factory)
   {
      lock(_sync) {
         _httpFactory := factory;
      }
   }

   static INetSocket openSocket(Uri uri)
   {
      INetSocket socket := nil;
      string scheme := uri.Scheme;
      short port := uri.Port;
      scheme =>
         "http" : 
         {
            if (port==0)
               port := 80;

            lock(_sync) {
               socket := _httpFactory.openSocket(uri.Host, port);
            }
         } 
         "https" : 
         {
            if (port==0)
               port := 443;

            lock(_sync) {
               socket := _httpsFactory.openSocket(uri.Host, port);
            }
         }
         ! : {
            InvalidArgumentException.raise("uri");
         };
      
      ^ socket 
   }
}

public class HttpClient
{      
   string      _host;
   HttpHeaders _headers;
   INetSocket  _socket;

   static Task<HttpClient> openAsync(Uri uri)
      = ::Task<HttpClient>.run({ ^ HttpClient.assign(SocketFactory.openSocket(uri), uri.Host); });

   static Task<HttpClient> openAsync(string url)
      = HttpClient.openAsync(Uri.parse(url));

   static open(Uri uri)
      = HttpClient.assign(SocketFactory.openSocket(uri), uri.Host);

   static open(string url)
      = HttpClient.open(Uri.parse(url));

   internal constructor assign(INetSocket  socket, string host)
   {
      _socket := socket;
      _host := host;
      _headers := new HttpHeaders();
   }

   HttpHeaders Headers
      = _headers;

   Task<HttpResponse> getAsync()
      <= getAsync("/");

   async Task<HttpResponse> getAsync(string path)
   {
      /* Send headers to server */
      NetworkStream stream := new NetworkStream(_socket);

      string headers := _headers.Value;
      string request := $"GET {path} HTTP/1.1"$13$10"Host:{_host}"$13$10"Connection:close"$13$10"{headers}"$13$10;

      :await stream.writeAsync(request);

      HttpResponse response := HttpResponse.assign(stream);

      :await response.readHeaderAsync();

      ^ response;
   }

   HttpResponse get()
      <= get("/");

   HttpResponse get(string path)
   {
      /* Send headers to server */
      NetworkStream stream := new NetworkStream(_socket);

      string headers := _headers.Value;
      string request := $"GET {path} HTTP/1.1"$13$10"Host:{_host}"$13$10"Connection:close"$13$10"{headers}"$13$10;

      stream.write(request);

      HttpResponse response := HttpResponse.assign(stream);

      response.readHeader();

      ^ response;
   }

   close()
   {
      if:not:nil(_socket)
         _socket.close()
   }
}