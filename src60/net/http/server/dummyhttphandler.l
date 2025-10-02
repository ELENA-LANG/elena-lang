//import system'threading;
//
//public class DummyHttpGetHandler : IHttpHandler
//{
//   IHttpErrorHandler _errorHandler;
//   
//   constructor(IHttpErrorHandler errorHandler)
//   {
//      _errorHandler := errorHandler
//   }
//   
//   HttpServerResponse proceed(HttpServerRequest request)
//   {
//      HttpServerResponse response;
//      if (request.Path == "/") {
//         response := HttpServerResponse.new("<html><header><title>ELENA Web Server Dummy</title></header><body><div>ELENA Web Server Dummy Response</div></body></html>");
//         response.Protocol := "HTTP/1.1";
//         response.StatusCode := 200;
//         response.StatusText := "OK";
//         response.Headers.add("Content-Type", "text/html");
//      }
//      else {
//         response := _errorHandler.getNotFoundResponse();
//      };      
//      
//      ^ response
//   }
//   
//   Task<HttpServerResponse> proceedAsync(HttpServerRequest request)
//      = ::Task<HttpServerResponse>.run({ ^ proceed(request) });
//}
//
//public singleton DummyHttpHandlerFactory : IHttpHandlerFactory
//{
//   IHttpHandler? create(string method)
//   {
//      if (method == "GET") {
//         ^ new DummyHttpGetHandler(HttpErrorHandler);
//      };
//      
//      ^ nil
//   }
//   
//   IHttpErrorHandler getErrorHandler()
//      = HttpErrorHandler;
//}
