import system'threading;

public class DummyHttpGetHandler : IHttpHandler
{
   IHttpErrorHandler _errorHandler;
   
   constructor(IHttpErrorHandler errorHandler)
   {
      _errorHandler := errorHandler
   }
   
   HttpServerResponse proceed(HttpServerRequest request)
   {
      HttpServerResponse response;
      if (request.Path == "/") {
         response := HttpServerResponse.new("<html><header>ELENA Web Server Dummy</header><body><div>ELENA Web Server Dummy Response</div></body></html>");
      }
      else {
         response := _errorHandler.getNotFoundResponse();
      };      
      
      ^ response
   }
   
   Task<HttpServerResponse> proceedAsync(HttpServerRequest request)
      = ::Task<HttpServerResponse>.assign({ ^ proceed(request) });
   
}

public class DummyHttpHandlerFactory : IHttpHandlerFactory
{
   IHttpHandler? create(string method)
   {
      if (method == "GET") {
         ^ new DummyHttpGetHandler(HttpErrorHandler);
      };
      
      ^ nil
   }
   
   IHttpErrorHandler getErrorHandler()
      = HttpErrorHandler;
}
