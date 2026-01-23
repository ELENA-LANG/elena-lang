import system'net;
import net'http;
import mbedtls;

namespace registration
{
   internal class MbedTlsServerSocketFactory : INetSocketFactory
   {
      internal static register()
      {
         SocketFactory.assignHttpsFactoryInvoker::([INetSocketFactory] => MbedTlsServerSocketFactory.create());
      }

      constructor create()
      {
         MbedTlsEnvironment.startUpServer()
      }
   
      INetSocket openSocket(string host, short port)
         = MbedTlsSocket.open(host, port, *MbedTlsServerStartup);
         
      INetListener openListener(short port, bool noDelay)
         = MbedTlsListener.open(port, noDelay, *MbedTlsServerStartup);
   }   
}

static Registration : preloaded = registration'MbedTlsServerSocketFactory.register();

