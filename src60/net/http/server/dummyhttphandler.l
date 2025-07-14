public class DummyHttpGetHandler : IHttpHandler
{
   HttpServerResponse proceed(HttpServerRequest request)
   {
      HttpServerResponse response;
      
      ^ response
   }
}

public class DummyHttpHandlerFactory : IHttpHandlerFactory
{
   IHttpHandler? create(string method)
   {
      if (method == "GET") {
         ^ new DummyHttpGetHandler();
      };
      
      ^ nil
   }
   
   
   HttpServerResponse getErrorHandler()
      = new BaseHttpErrorHandler();
}
