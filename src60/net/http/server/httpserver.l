import extensions'threading;
import net;
import net'http;
import system'threading;

public sealed class HttpServer : interface<IHttpServer>
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
      
      _listenThread.join();
   }   

   protected run()
   {
      _listener.start(_backLog);
      
      while(true) {
         auto client := _listener.acceptTcpClient();
         auto thread := Thread.start(
            {
               try
               {
                  auto acceptor := new HttpAcceptor(client, _handleFactory);
                  acceptor.acceptClient();
               }
               catch(Exception ex)
               {
                  // !! temporal
                  Console.printLineConcurrent(ex);
                  
                  auto acceptor := new HttpAcceptor(client, _handleFactory);
                  acceptor.returnError(ex);
               }
            });
      }
   }
}

public abstract class BaseHttpErrorHandler : IHttpErrorHandler
{
   HttpServerResponse getNotFoundResponse()
   {
      HttpServerResponse response := HttpServerResponse.new("File Not Found");
      response.Protocol := "HTTP/1.1";
      response.StatusCode := 404;
      response.StatusText := "Not Found";
      response.Headers.add("Content-Type", "text/html");
      
      ^ response;
   }
   
   
   HttpServerResponse getInternalErrorResponse(Exception ex)
   {
      HttpServerResponse response := HttpServerResponse.new(ex.toPrintable());
      response.Protocol := "HTTP/1.1";
      response.StatusCode := 500;
      response.StatusText := "Internal Server Error";
      response.Headers.add("Content-Type", "text/html");
      
      ^ response;
   }
}

public singleton HttpErrorHandler : BaseHttpErrorHandler
{
   HttpServerResponse handle(Exception ex)
      = getInternalErrorResponse(ex);
      
   Task<HttpServerResponse> handleAsync(Exception ex)
      = ::Task<HttpServerResponse>.assign({ ^ handle(ex) });
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
   
   returnError(Exception ex)
   {
      returnErrorAsync(ex).Result
   }
   
   async Task returnErrorAsync(Exception ex)
   {
      auto stream := _client.stream();
      
      auto handler := _handleFactory.getErrorHandler();
      
      HttpServerResponse response := :await handler.handleAsync(ex);
      
      :await response.saveContentAsync(stream);
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

      _client.close();
   }
}
