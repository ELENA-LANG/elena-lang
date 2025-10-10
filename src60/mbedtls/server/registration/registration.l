import system'net;
import net'http;
import mbedtls;

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
         MbedTlsEnvironment.startUpServer()
      }
   
      INetSocket openSocket(string host, short port)
         = MbedTlsSocket.open(host, port);
         
      INetListener openListener(short port, bool noDelay)
         = NotSupportedException.raise();
   }   
}

static Registration : preloaded = registration'MbedTlsSocketFactory.register();

