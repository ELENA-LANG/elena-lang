import extensions'threading;
import net;
import net'http;
import system'threading;

public class HttpServer : IHttpServer
{
   Thread               _listenThread;  

   TcpListener          _listener;
   int                  _backLog; 
   
   IHttpHandlerFactory  _handleFactory;
   
   constructor create(IHttpHandlerFactory handleFactory)
   {
      _listener := TcpListener.new(HTTP_PORT, false);
      _backLog := 100;
      _handleFactory := handleFactory
   }
   
   start()
   {
      _listenThread := Thread.start(
      {
         run();
      });
   }

   run()
   {
      _listener.start(_backLog);
      
      while(true) {
         auto client := _listener.acceptTcpClient();
         auto thread := Thread.start(
            {
               try
               {
                  auto acceptor := new HttpAcceptor(client);
                  acceptor.acceptClient();
               }
               catch(Exception ex)
               {
                  // !! temporal
                  Console.printLineConcurrent(ex)
               }
            });
      }
   }
}

public class BaseHttpErrorHandler : IHttpErrorHandler
{
   HttpServerResponse handle(Exception ex)
      = getInternalErrorResponse(ex);
   
   
   HttpServerResponse getNotFoundResponse()
   {
      HttpServerResponse response;
      
      ^ response;
   }
   
   
   HttpServerResponse getInternalErrorResponse(Exception ex)
   {
      HttpServerResponse response;
      
      ^ response;
   }
}

class HttpAcceptor
{
   TcpClient            _client;
   IHttpHandlerFactory  _handleFactory;
   
   constructor(TcpClient client, IHttpHandlerFactory handleFactory)
   {
      _client := client;
      _handleFactory := handleFactory
   }
   
   acceptClient()
   {
      acceptClientAsync().Result
   }
   
   async Task acceptClientAsync()
   {
      auto stream := _client.stream();
      
      auto request := HttpServerRequest.assign(stream);
      
      :await request.readHeaderAsync();
      
      HttpServerResponse response;      
      auto handler := _handleFactory.create(request.Method);
      if (handler != nil) {
         try {
            response := :await handler.proceedAsync(request);
         }
         catch(Exception ex) {
            auto handler := _handleFactory.getErrorHandler();
            
            response := :await handler.handleAsync(ex);
         }         
      }
      else response := _handleFactory.getErrorHandler().getNotFoundResponse();
      
      :await response.saveContentAsync(stream);
   }
}
