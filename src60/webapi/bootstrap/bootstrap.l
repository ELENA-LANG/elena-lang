import net'http'server;
import webapi;

internal singleton Bootstrap : IBootstap
{
   static IHttpServer _server;
   
   initDummy()
   {
      _server := HttpServer.create(DummyHttpHandlerFactory);
   }
   
   protected IHttpServer getServer()
      = _server;
}

public singleton WebApiFactory : BootstrapFactoryBase
{
   static Action<IBootstap> _serverStartUp;
   
   registerServer(Action<IBootstap> action)
   {
      _serverStartUp := action;
   }
   
   protected IBootstap create()
   {
      IBootstap bootstrap := Bootstrap;
      
      _serverStartUp(bootstrap);
      
      ^ bootstrap;
   }   
}
