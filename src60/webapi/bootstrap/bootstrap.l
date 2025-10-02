import system'collections;
import net'http'server;
import webapi;

internal singleton Bootstrap //: IBootstap
{
   static IHttpServer?           _server;
//   static IHandleFactory?        _handleFactory;
   static IHttpServerProvider?   _provier; 
   
   initSimpleServer()
   {
//      _server := SimpleHttpServer.create(_provider ?? HttpServerProvider, _handleFactory ?? DummyHttpHandlerFactory);
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
