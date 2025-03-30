struct mbedtls_entropy_source_state
{  
   pointer f_source;
   pointer p_source;
   pointer size;       // !! must be size_t
   pointer threshold;  // !! must be size_t
   int     strong;
}

struct mbedtls_md_context_t
{
   pointer md_info; // mbedtls_md_info_t*
   pointer md_ctx;
   pointer hmac_ctx;
}

struct mbedtls_entropy_context
{
   mbedtls_md_context_t          accumulator;
   int                           accumulator_started;
   int                           source_count;
   mbedtls_entropy_source_state  source[20];
}

public test()
{
/*
   int ret = 0;

   mbedtls_net_context server_fd;*/
   mbedtls_entropy_context entropy;
/*   mbedtls_ctr_drbg_context ctr_drbg;
   mbedtls_ssl_context ssl;
   mbedtls_ssl_config conf;
   mbedtls_x509_crt cacert;

   mbedtls_net_init(&server_fd);
   mbedtls_ssl_init(&ssl);
   mbedtls_ssl_config_init(&conf);
   mbedtls_x509_crt_init(&cacert);
   mbedtls_ctr_drbg_init(&ctr_drbg);

   const char* pers = DRBG_PERSONALIZED_STR;
*/
   extern mbedtls.mbedtls_entropy_init(entropy);
/*
   if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
      reinterpret_cast<const unsigned char*>(DRBG_PERSONALIZED_STR),
      strlen(DRBG_PERSONALIZED_STR) + 1)) != 0)
   {
      printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
      //goto exit;
      return 0;
   }

   if ((ret = mbedtls_net_connect(&server_fd, SERVER_NAME,
      SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0)
   {
      printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
      //goto exit;
      return 0;
   }

   if ((ret = mbedtls_ssl_config_defaults(&conf,
      MBEDTLS_SSL_IS_CLIENT,
      MBEDTLS_SSL_TRANSPORT_STREAM,
      MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
   {
      return 0;
   }

   mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);

   mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
   mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

   if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
      printf("mbedtls_ssl_setup() returned -0x%04X\n", -ret);
      return ret;
   }

   if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0)
   {
      //mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
      //goto exit;
      return 0;
   }

   mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

   char gp_buf[1024];


       // Fill the request buffer
       ret = snprintf(gp_buf, sizeof(gp_buf),
                      "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", HTTP_REQUEST_FILE_PATH,
                     SERVER_NAME);
       int req_len = static_cast<size_t>(ret);
       if (ret < 0 || req_len >= sizeof(gp_buf)) {
           //mbedtls_printf("Failed to compose HTTP request using snprintf: %d\n",
           //               ret);
           return ret;
       }
   
       int e = MBEDTLS_ERR_SSL_WANT_WRITE;

       // Send the HTTP request to the server over TLS
       int req_offset = 0;
       do {
           ret = mbedtls_ssl_write(&ssl,
                   reinterpret_cast<const unsigned char*>(gp_buf + req_offset),
                   req_len - req_offset);
           if (ret > 0)
               req_offset += static_cast<size_t>(ret);
       }
       while(req_offset < req_len &&
             (ret > 0 ||
             ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
             ret == MBEDTLS_ERR_SSL_WANT_READ));
       if (ret < 0) {
           //mbedtls_printf("mbedtls_ssl_write() returned -0x%04X\n", -ret);
           return ret;
       }
   
       // Print information about the TLS connection
       ret = mbedtls_x509_crt_info(gp_buf, sizeof(gp_buf),
                                   "\r  ", mbedtls_ssl_get_peer_cert(&ssl));
       if (ret < 0) {
           //mbedtls_printf("mbedtls_x509_crt_info() returned -0x%04X\n", -ret);
           return ret;
       }
       //mbedtls_printf("Server certificate:\n%s\n", gp_buf);
   
       // Ensure certificate verification was successful
       int flags = mbedtls_ssl_get_verify_result(&ssl);
       if (flags != 0) {
           ret = mbedtls_x509_crt_verify_info(gp_buf, sizeof(gp_buf),
                                              "\r  ! ", flags);
           if (ret < 0) {
               //mbedtls_printf("mbedtls_x509_crt_verify_info() returned "
               //               "-0x%04X\n", -ret);
               return ret;
           } else {
               //mbedtls_printf("Certificate verification failed (flags %lu):"
               //               "\n%s\n", flags, gp_buf);
               return -1;
           }
       } else {
           //mbedtls_printf("Certificate verification passed\n");
       }
   
       //mbedtls_printf("Established TLS connection to %s\n", server_name);
   
       // Read response from the server
       int resp_offset = 0;
       bool resp_200 = false;
       bool resp_hello = false;
       do {
           ret = mbedtls_ssl_read(&ssl,
                       reinterpret_cast<unsigned char *>(gp_buf  + resp_offset),
                       sizeof(gp_buf) - resp_offset - 1);
           if (ret > 0)
               resp_offset += static_cast<size_t>(ret);
   
           gp_buf[resp_offset] = '\0';
   
           resp_200 = resp_200 || strstr(gp_buf, HTTP_OK_STR) != NULL;
           resp_hello = resp_hello || strstr(gp_buf, HTTP_HELLO_STR) != NULL;
       } while((!resp_200 || !resp_hello) &&
               (ret > 0 ||
               ret == MBEDTLS_ERR_SSL_WANT_READ || MBEDTLS_ERR_SSL_WANT_WRITE));
       if (ret < 0) {
           return ret;
       }
   
   mbedtls_net_free(&server_fd);
   mbedtls_ssl_free(&ssl);
   mbedtls_ssl_config_free(&conf);
   mbedtls_ctr_drbg_free(&ctr_drbg);*/
   mbedtls_entropy_free(&entropy);
}