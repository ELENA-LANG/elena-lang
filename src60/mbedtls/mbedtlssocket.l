/* *********************************************************************
ELENA Project : MbedTls based SSL Socket

NOTE : the prject requires wrpmbedtls.dll 

It can be downloaded from https://github.com/ELENA-LANG/mbedtls-as-dll

********************************************************************* */

import system'net;

public class MbedTlsSocket : INetSocket
{
   mbedtls_net_context        _server_fd;
   mbedtls_entropy_context    _entropy;
   mbedtls_ctr_drbg_context   _ctr_drbg;
   mbedtls_ssl_context        _ssl;
   mbedtls_ssl_config         _conf;

   private prepare(string host, string port)
   {
      mbedtls_net_context        server_fd := default;
      mbedtls_entropy_context    entropy := default;
      mbedtls_ctr_drbg_context   ctr_drbg := default;
      mbedtls_ssl_context        ssl := default;
      mbedtls_ssl_config         conf := default;

      extern wrpmbedtls.net_init(server_fd);
      extern wrpmbedtls.ssl_init(ssl);
      extern wrpmbedtls.ssl_config_init(conf);
   //   mbedtls_x509_crt_init(&cacert);
      extern wrpmbedtls.ctr_drbg_init(ctr_drbg);
   
      string pers := DRBG_PERSONALIZED_STR;
   
      extern wrpmbedtls.entropy_init(entropy);
   
      int ret := extern wrpmbedtls.ctr_drbg_seed_def(ctr_drbg, entropy, DRBG_PERSONALIZED_STR, DRBG_PERSONALIZED_STR.Length + 1);
   
      if (ret != 0)
      {
         MbedTlsException.raise($"Failed. mbedtls_ctr_drbg_seed returned {ret}");
      };
   
      ret := extern wrpmbedtls.net_connect(server_fd, host, port, MBEDTLS_NET_PROTO_TCP);
      if (ret != 0)
      {
         MbedTlsException.raise($"Failed. mbedtls_net_connect returned {ret}");
      };
   
      ret := extern wrpmbedtls.ssl_config_defaults(conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
   
      if (ret != 0)
      {
         MbedTlsException.raise($"Failed. mbedtls_ssl_config_defaults returned {ret}");
      };
   
      extern wrpmbedtls.ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
   
      extern wrpmbedtls.ssl_conf_rng_def(conf, ctr_drbg);
   //   extern wrpmbedtls.ssl_conf_dbg(conf, my_debug, stdout);
   
      ret := extern wrpmbedtls.ssl_setup(ssl, conf);
      if (ret != 0) 
      {
         MbedTlsException.raise($"Failed. mbedtls_ssl_setup returned {ret}");
      };

      ret := extern wrpmbedtls.ssl_set_hostname(ssl, host);
      if (ret != 0)
      {
         MbedTlsException.raise($"Failed. mbedtls_ssl_set_hostname returned {ret}");
      };
   
      extern wrpmbedtls.ssl_set_bio_def(ssl, server_fd);      

      _server_fd := server_fd;
      _entropy := entropy;
      _ctr_drbg := ctr_drbg;
      _ssl := ssl;
      _conf := conf;
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
            retVal := extern wrpmbedtls.ssl_read(_ssl,
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
         retVal := extern wrpmbedtls.ssl_write(_ssl, buffer, length);
   
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
      // currently it is not supported
      NotSupportedException.raise();

      ^ false
   }

   close()
   {
      extern wrpmbedtls.net_free(_server_fd);
      extern wrpmbedtls.ssl_free(_ssl);
      extern wrpmbedtls.ssl_config_free(_conf);
      extern wrpmbedtls.ctr_drbg_free(_ctr_drbg);
      extern wrpmbedtls.entropy_free(_entropy);
   }
}