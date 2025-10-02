import net'http'server;
/*
public interface IControllerFactory
{
   IHttpHandlerFactory 
}
*/

//public interface IHttpHandlerFactory 
//{
//   
//}

public interface IBootstap
{
   abstract initDummy();
   
   protected abstract IHttpServer getServer();
   
   internal start()
   {
      IHttpServer server := getServer();
      
      server.start();
   }
}