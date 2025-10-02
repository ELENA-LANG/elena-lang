import extensions'threading;
import net;
import net'http;
import system'threading;
import system'net;

public sealed class SimpleHttpServer : interface<IHttpServer>
{
   Thread               _listenThread;  

   INetListener         _listener;
   int                  _backLog; 
   
   IHttpHandlerFactory  _handleFactory;   
   IHttpServerProvider  _provider;
   
//   constructor create(IHttpServerProvider provider, IHttpHandlerFactory handleFactory)
//   {
//      _backLog := 100;
//      _handleFactory := handleFactory;
//      _provider := provider
//   }
   
   start(string prefix, short port)
   {
//      _provider.start();
      
//      _listener := _provider.openListener(prefix, port, false);
      
//      _listenThread := Thread.start(
//      {
//         run();
//      });
   }   

//   protected run()
//   {
//      _listener.start(_backLog);
//      
//      while(true) {
//         auto socket := _listener.acceptNetSocket();
//         auto thread := Thread.start(
//            {
//               try
//               {
//                  auto acceptor := new HttpAcceptor(socket, _handleFactory);
//                  acceptor.acceptClient();
//               }
//               catch(Exception ex)
//               {
//                  // !! temporal
//                  Console.printLineConcurrent(ex);
//                  
//                  auto acceptor := new HttpAcceptor(socket, _handleFactory);
//                  acceptor.returnError(ex);
//               }
//            });
//      }
//   }
}

//public abstract class BaseHttpErrorHandler : IHttpErrorHandler
//{
//   HttpServerResponse getNotFoundResponse()
//   {
//      HttpServerResponse response := HttpServerResponse.new("File Not Found");
//      response.Protocol := "HTTP/1.1";
//      response.StatusCode := 404;
//      response.StatusText := "Not Found";
//      response.Headers.add("Content-Type", "text/html");
//      
//      ^ response;
//   }
//   
//   
//   HttpServerResponse getInternalErrorResponse(Exception ex)
//   {
//      HttpServerResponse response := HttpServerResponse.new(ex.toPrintable());
//      response.Protocol := "HTTP/1.1";
//      response.StatusCode := 500;
//      response.StatusText := "Internal Server Error";
//      response.Headers.add("Content-Type", "text/html");
//      
//      ^ response;
//   }
//}
//
//public singleton HttpErrorHandler : BaseHttpErrorHandler
//{
//   HttpServerResponse handle(Exception ex)
//      = getInternalErrorResponse(ex);
//      
//   Task<HttpServerResponse> handleAsync(Exception ex)
//      = ::Task<HttpServerResponse>.run({ ^ handle(ex) });
//}
//
//class HttpAcceptor
//{
//   INetSocket           _socket;
//   NetworkStream        _stream; 
//   IHttpHandlerFactory  _handleFactory;
//   
//   constructor(INetSocket socket, IHttpHandlerFactory handleFactory)
//   {
//      _socket := socket;
//      _stream := NetworkStream.assign(_socket, false);
//      _handleFactory := handleFactory
//   }
//   
//   returnError(Exception ex)
//   {
//      returnErrorAsync(ex).Result
//   }
//   
//   async Task returnErrorAsync(Exception ex)
//   {
//      auto handler := _handleFactory.getErrorHandler();
//      
//      HttpServerResponse response := :await handler.handleAsync(ex);
//      
//      :await response.saveContentAsync(_stream);
//   }
//   
//   acceptClient()
//   {
//      acceptClientAsync().Result
//   }
//   
//   async Task acceptClientAsync()
//   {
//      auto request := HttpServerRequest.assign(_stream);
//      
//      :await request.readHeaderAsync();
//      
//      HttpServerResponse response;      
//      auto handler := _handleFactory.create(request.Method);
//      if (handler != nil) {
//         try {
//            response := :await handler.proceedAsync(request);
//         }
//         catch(Exception ex) {
//            auto handler := _handleFactory.getErrorHandler();
//            
//            response := :await handler.handleAsync(ex);
//         }
//      }
//      else response := _handleFactory.getErrorHandler().getNotFoundResponse();
//      
//      :await response.saveContentAsync(_stream);
//
//      _stream.close();
//   }
//}
