import net'http'server;

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