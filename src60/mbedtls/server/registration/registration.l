import system'net;
import net'http;
import mbedtls;

namespace registration
{
   internal class MbedTlsSocketFactory : INetSocketFactory
   {
      internal static register()
      {
         SocketFactory.assignHttpsFactoryInvoker::([INetSocketFactory] => MbedTlsSocketFactory.create());
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

static Registration : preloaded = registration'MbedTlsSocketFactory.register();

