import system'net;
import net'http;

namespace registration
{
   internal class MbedTlsSocketFactory : INetSocketFactory
   {
      internal static register()
      {
         SocketFactory.assignHttpsFactory(MbedTlsSocketFactory.create());
      }

      constructor create()
      {
      }
   
      INetSocket openSocket(string host, short port)
         = MbedTlsSocket.open(host, port);
   }   
}

static Registration : preloaded = registration'MbedTlsSocketFactory.register();

