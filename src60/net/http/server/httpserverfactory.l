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
      = SocketFactory.openListener(protocol, port, noDelay);
   
   shutdown() {}
   
}
