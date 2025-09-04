/* *********************************************************************
ELENA Project : MbedTls based SSL Socket

NOTE : the prject requires wrpmbedtls.dll (>= 1.1) 

It can be downloaded from https://github.com/ELENA-LANG/mbedtls-as-dll

********************************************************************* */

import system'net;

public class MbedTlsSocket : INetSocket
{
   pointer  _context;         

   private prepare(string host, string port)
   {
      _context := extern wrpmbedtls.new_context();

      extern wrpmbedtls.init_context(_context);
      //   mbedtls_x509_crt_init(&cacert);

      int ret := 0;

      string pers := DRBG_PERSONALIZED_STR;
      ret := extern wrpmbedtls.context_drbg_seed_def(_context, DRBG_PERSONALIZED_STR, DRBG_PERSONALIZED_STR.Length + 1);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_ctr_drbg_seed returned {ret}");
      };

      ret := extern wrpmbedtls.context_net_connect(_context, host, port, MBEDTLS_NET_PROTO_TCP);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_net_connect returned {ret}");
      };

      ret := extern wrpmbedtls.context_ssl_config_defaults(_context, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_ssl_config_defaults returned {ret}");
      };

      ret := extern wrpmbedtls.context_setup(_context, MBEDTLS_SSL_VERIFY_NONE);
      if (ret != 0) 
      {
         Exception.raise($"Failed. mbedtls_ssl_setup returned {ret}");
      };

      ret := extern wrpmbedtls.context_ssl_set_hostname(_context, host);
      if (ret != 0)
      {
         Exception.raise($"Failed. mbedtls_ssl_set_hostname returned {ret}");
      };

      extern wrpmbedtls.context_ssl_set_bio_def(_context);
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
            retVal := extern wrpmbedtls.context_read(_context,
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
         retVal := extern wrpmbedtls.context_write(_context, buffer, length);
   
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
         retVal := extern wrpmbedtls.socket_data_available(_context);
      };

      ^ retVal != 0;
   }

   close()
   {
      extern wrpmbedtls.free_context(_context);
      extern wrpmbedtls.delete_context(_context);

      _context := default;
   }
}