/* *********************************************************************
ELENA Project : MbedTls based SSL Socket

NOTE : the prject requires wrpmbedtls.dll (>= 1.2) 

It can be downloaded from https://github.com/ELENA-LANG/mbedtls-as-dll

********************************************************************* */

import system'net;
import system'threading;

internal class MbedTlsRemoteSocket : MbedTlsBaseSocket
{
   indexed internal pointer Value 
      = _context;

   constructor open()
   {
      _context := extern wrpmbedtls.mbedtls_new_context();
   }
}

public class MbedTlsListener : INetListener
{
   pointer  _env;
   pointer  _context;         
   short    _port; 

   constructor open(short port, bool noDelay, pointer env)
   {
      _env := env;
      _port := port;

      _context := extern wrpmbedtls.mbedtls_new_net_context();
   }

   start()
   {
      string portStr := _port.toPrintable();

      int ret := extern wrpmbedtls.mbedtls_bind_net_context(_context, portStr);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_net_bind returned {ret}");
      };
   }

   start(int backLog)
   {
      string portStr := _port.toPrintable();

      int ret := extern wrpmbedtls.mbedtls_bind_net_context(_context, portStr);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_net_bind returned {ret}");
      };
   }
   
   INetSocket acceptNetSocket()
   {
      MbedTlsRemoteSocket socket := MbedTlsRemoteSocket.open();

      pointer socketContext := *socket;

      int ret := extern wrpmbedtls.mbedtls_accept_net_context(_context, socketContext);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_net_accept returned {ret}");
      };

      ret := extern wrpmbedtls.mbedtls_context_setup(_env, socketContext);
      if (ret != 0) 
      {
         Exception.raise($"Failed. context_setup returned {ret}");
      };

      extern wrpmbedtls.mbedtls_context_ssl_set_bio_def(socketContext);

      ret := extern wrpmbedtls.mbedtls_socket_handshake(socketContext);
      if (ret != 0) 
      {
         Exception.raise($"Failed. mbedtls_ssl_handshake returned {ret}");
      };

      ^ socket
   }

   async Task<INetSocket> acceptNetSocketAsync()
      = ::Task<INetSocket>.run({ ^ acceptNetSocket(); });

   close()
   {
      extern wrpmbedtls.mbedtls_delete_net_context(_context);

      _context := default;
   }
}