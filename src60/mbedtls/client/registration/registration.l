import system'net;
import net'http;
import mbedtls;

namespace registration
{
   internal class MbedTlsClientSocketFactory : INetSocketFactory
   {
      internal static register()
      {
         SocketFactory.assignHttpsFactoryInvoker::([INetSocketFactory] => MbedTlsClientSocketFactory.create());
      }

      constructor create()
      {
         MbedTlsEnvironment.startUpClient()
      }
   
      INetSocket openSocket(string host, short port)
         = MbedTlsSocket.open(host, port, *MbedTlsClientStartup);
         
      INetListener openListener(short port, bool noDelay)
         = MbedTlsListener.open(port, noDelay, *MbedTlsServerStartup);
   }   
}

static Registration : preloaded = registration'MbedTlsClientSocketFactory.register();
