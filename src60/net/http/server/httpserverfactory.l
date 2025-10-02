import system'net;
import system'threading;

public interface IHttpServerProvider
{
   abstract start();
   
   abstract INetListener openListener(string protocol, short port, bool noDelay);
         
   Task<INetListener> openListenerAsync(string prefix, short port, bool noDelay)
      = ::Task<INetListener>.run({ ^ self.openListener(prefix, port, noDelay); });
   
   abstract shutdown();
}