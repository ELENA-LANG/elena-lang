import system'net;
import system'threading;

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

      :await stream.writeStringAsync(request);

      HttpResponse response := HttpResponse.assign(stream);

      :await response.readHeaderAsync();

      ^ response;
   }

   async Task<HttpResponse> postAsync(string path, HttpRequest request)
   {
      int length := request.ContentLength;
      
      /* Send headers to server */
      NetworkStream stream := new NetworkStream(_socket);

      HttpHeaders postHeaders := default;
      if (request.Headers.Length == 0) {
         postHeaders := _headers;
      }
      else {
         postHeaders := HttpHeaders.combine(_headers, request.Headers);
      };
      
      string headers := postHeaders.Value;
      string content := request.Content;
      
      string request := $"POST {path} HTTP/1.1"$13$10"Host:{_host}"$13$10"Connection:close"$13$10"Content-Length:{length}"$13$10"Content-Type:application/x-www-form-urlencoded"$13$10"{headers}"$13$10"{content}";

      :await stream.writeStringAsync(request);

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

      stream.writeString(request);

      HttpResponse response := HttpResponse.assign(stream);

      response.readHeader();

      ^ response;
   }


   HttpResponse post(HttpRequest request)
      <= post("/", request);

   HttpResponse post(string path, HttpRequest request)
   {
      int length := request.ContentLength;
      
      /* Send headers to server */
      NetworkStream stream := new NetworkStream(_socket);

      HttpHeaders postHeaders := default;
      if (request.Headers.Length == 0) {
         postHeaders := _headers;
      }
      else {
         postHeaders := HttpHeaders.combine(_headers, request.Headers);
      };
      
      string headers := postHeaders.Value;
      string content := request.Content;
      
      string request := $"POST {path} HTTP/1.1"$13$10"Host:{_host}"$13$10"Connection:close"$13$10"Content-Length:{length}"$13$10"Content-Type:application/x-www-form-urlencoded"$13$10"{headers}"$13$10"{content}";

      stream.writeString(request);

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