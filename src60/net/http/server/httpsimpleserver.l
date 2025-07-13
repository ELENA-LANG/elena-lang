import extensions'threading;
import net;
import net'http;
import system'threading;

public class HttpSimpleServer : IHttpServer
{
   TcpListener _listener;
   int         _backLog; 
   
   constructor create()
   {
      _listener := TcpListener.new(HTTP_PORT, false);
      _backLog := 100
   }
   
   run()
   {
      _listener.start(_backLog);
      
      while(true) {
         auto client := _listener.acceptTcpClient();
         auto thread := Thread.assign(
            {
               try
               {
                  auto acceptor := new HttpSimpleAcceptor(client);
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

class HttpSimpleAcceptor
{
   TcpClient _client;
   
   constructor(TcpClient client)
   {
      _client := client
   }
   
   acceptClient()
   {
      auto stream := _client.stream();
      
      auto request := HttpServerRequest.assign(stream);
      
      request.readHeader();
      
              
   }   
}
