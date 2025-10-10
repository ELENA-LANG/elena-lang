/* *********************************************************************
ELENA Project : MbedTls based SSL Socket

NOTE : the prject requires wrpmbedtls.dll (>= 1.2) 

It can be downloaded from https://github.com/ELENA-LANG/mbedtls-as-dll

********************************************************************* */

import system'net;

public class MbedTlsSocket : INetSocket
{
   pointer  _context;         

   private prepare(string host, string port)
   {
      MbedTlsClientStartup;

      _context := extern wrpmbedtls.mbedtls_new_context();

      extern wrpmbedtls.mbedtls_init_context(_context);
      //   mbedtls_x509_crt_init(&cacert);

      int ret := 0;

      ret := extern wrpmbedtls.mbedtls_context_net_connect(_context, host, port, MBEDTLS_NET_PROTO_TCP);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_net_connect returned {ret}");
      };

      ret := extern wrpmbedtls.mbedtls_context_setup(_context);
      if (ret != 0) 
      {
         Exception.raise($"Failed. context_setup returned {ret}");
      };

      ret := extern wrpmbedtls.mbedtls_context_ssl_set_hostname(_context, host);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_ssl_set_hostname returned {ret}");
      };

      extern wrpmbedtls.mbedtls_context_ssl_set_bio_def(_context);
   }

   constructor open(string host, short port)
   {
      prepare(host, port.toPrintable());
   }

   int read(byte[] buffer, int length)
   {
      int retVal := 0;

      while (true) {
         extern {
            retVal := extern wrpmbedtls.mbedtls_context_read(_context,
                          buffer, length);
         };

         if (retVal < 0) {
            if (retVal == MBEDTLS_ERR_SSL_WANT_WRITE || retVal == MBEDTLS_ERR_SSL_WANT_READ) {
               retVal := 0;
            }
            else MbedTlsException.raise($"Failed. mbedtls_ssl_write returned {retVal}");
         }
         else :break;
      };

      ^ retVal
   }

   int write(byte[] buffer, int length)
   {
      int retVal := 0;
      while (true) {
         retVal := extern wrpmbedtls.mbedtls_context_write(_context, buffer, length);
   
         if (retVal < 0) {
            if (retVal == MBEDTLS_ERR_SSL_WANT_WRITE || retVal == MBEDTLS_ERR_SSL_WANT_READ) {
               retVal := 0;
            }
            else MbedTlsException.raise($"Failed. mbedtls_ssl_write returned {retVal}");
         }
         else :break;
      };

      ^ retVal
   }

   verify()
   {
//       byte gp_buf[1024];
//       ret := extern wrpmbedtls.x509_crt_info(gp_buf, 1024,
//                                   ""$10"  ", extern wrpmbedtls.ssl_get_peer_cert(ssl));
//       if (ret < 0) {
//           Exception.raise($"Failed. mbedtls_x509_crt_info returned {ret}");
//       };
//       //mbedtls_printf("Server certificate:\n%s\n", gp_buf);
//   
//       // Ensure certificate verification was successful
//       int flags := extern wrpmbedtls.ssl_get_verify_result(ssl);
//       if (flags != 0) {
//           ret := extern wrpmbedtls.x509_crt_verify_info(gp_buf, 1024,
//                                              ""$10"  ! ", flags);
//           if (ret < 0) {
//               Exception.raise($"Failed. mbedtls_x509_crt_verify_info returned {ret}");
//           } 
//           else {
//               Exception.raise("Certificate verification failed");
//           }
//       } ;
////       else {
//           //mbedtls_printf("Certificate verification passed\n");
////       }
   }

   get bool AvailableToRead()
   {
      int retVal := 0;
      extern {
         retVal := extern wrpmbedtls.mbedtls_socket_data_available(_context);
      };

      ^ retVal != 0;
   }

   close()
   {
      extern wrpmbedtls.mbedtls_free_context(_context);
      extern wrpmbedtls.mbedtls_delete_context(_context);

      _context := default;
   }
}