import system'net;
import system'threading;
import net'http;

public interface IHttpServerProvider
{
   abstract INetListener start(string protocol, short port, bool noDelay);

   abstract shutdown();
}

public singleton HttpServerProvider : IHttpServerProvider
{
   INetListener start(string protocol, short port, bool noDelay)
   {
      if (protocol == "http") {
         ^ SocketFactory.openListener(protocol, port, noDelay)
      };
      
      ^ InvalidArgumentException.raise("protocol");            
   }
   
   shutdown() {}
   
}
