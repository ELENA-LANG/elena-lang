import system'net;
import net'http;
import mbedtls;

namespace registration
{
   internal class MbedTlsClientSocketFactory : INetSocketFactory
   {
      internal static register()
      {
//         SocketFactory.assignHttpsFactoryInvoker({ ^ MbedTlsClientSocketFactory.create() });
//         
//         SocketFactory.assignHttpsFactoryInvoker(( => MbedTlsClientSocketFactory.create()) );
//         
//         SocketFactory.assignHttpsFactoryInvoker(([]=> MbedTlsClientSocketFactory.create()) );
//         
//         SocketFactory.assignHttpsFactoryInvoker::([]=> MbedTlsClientSocketFactory.create());
      }

      constructor create()
      {
         MbedTlsClientStartup;
      }
   
      INetSocket openSocket(string host, short port)
         = MbedTlsSocket.open(host, port);
         
      INetListener openListener(short port, bool noDelay)
         = NotSupportedException.raise("Functionality is currently not supported");
   }   
}

static Registration : preloaded = registration'MbedTlsSocketFactory.register();
