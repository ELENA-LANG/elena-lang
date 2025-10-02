import system'collections;
import net'http;
import net'http'server;
import webapi;

internal singleton Bootstrap : IBootstap
{
   static IHttpServer?           _server;
   static IHttpHandlerFactory?   _handleFactory;
   static IHttpServerProvider?   _provider; 
   
   initSimpleServer(string prefix, short port)
   {
      _server := SimpleHttpServer.create(prefix, port, _provider ?? HttpServerProvider, _handleFactory ?? DummyHttpHandlerFactory);
   }
   
   initSimpleServer()
   {
      _server := SimpleHttpServer.create("http", HTTP_PORT, _provider ?? HttpServerProvider, _handleFactory ?? DummyHttpHandlerFactory);
   }
   
   protected IHttpServer? getServer()
      = _server;
}

public singleton WebApiFactory : BootstrapFactoryBase
{
   static List<Action<IBootstap>> _setupSteps;

   private List<Action<IBootstap>> getSteps()
   {
      if (_setupSteps != nil)
         _setupSteps := new List<Action<IBootstap>>();

      ^ _setupSteps
   }
         
   register(Action<IBootstap> action)
   {
      getSteps().append(action)
   }
   
   protected IBootstap create()
   {
      IBootstap bootstrap := Bootstrap;
      
      foreach(auto action;in getSteps()) {
         action(bootstrap);
      };
      
      ^ bootstrap;
   }   
}
