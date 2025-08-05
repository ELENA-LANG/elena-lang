import net'http'server;
import webapi;

public singleton Bootstrap : IBootstap
{
   //static IHttpServer Server : rprop(_server);
   static IHttpServer _server;
   
   get IHttpServer Server()
      = _server;
   
   initDummy()
   {
      _server := HttpServer.create(DummyHttpHandlerFactory);
   }
}