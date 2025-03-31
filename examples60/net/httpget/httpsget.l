struct mbedtls_mpi
{
   pointer                     p;
   short                       s;
   short                       n; 
}

struct mbedtls_ssl_config
{
   int                          max_tls_version;
   int                          min_tls_version;
   byte                         endpoint;
   byte                         transport;
   byte                         authmode;
   byte                         allow_legacy_renegotiation;
   byte                         mfl_code;
   byte                         encrypt_then_mac;
   byte                         extended_ms;
   byte                         anti_replay;
   byte                         disable_renegotiation;
   byte                         session_tickets;
   byte                         cert_req_ca_list;
   byte                         respect_cli_pref;
   byte                         ignore_unexpected_cid;
   pointer                      ciphersuite_list;
   pointer                      f_dbg;
   pointer                      p_dbg;
   pointer                      f_rng;
   pointer                      p_rng;
   pointer                      f_get_cache;
   pointer                      f_set_cache;
   pointer                      p_cache;
   pointer                      f_sni;
   pointer                      p_sni;
   pointer                      f_vrfy;
   pointer                      p_vrfy;
   pointer                      f_psk;
   pointer                      p_psk;
   pointer                      f_cookie_write;
   pointer                      f_cookie_check;
   pointer                      p_cookie;
   pointer                      f_ticket_write;
   pointer                      f_ticket_parse;
   pointer                      p_ticket;
   pointer                      cid_len;  // size_t
   pointer                      cert_profile;
   pointer                      key_cert;
   pointer                      ca_chain;
   pointer                      ca_crl;
   pointer                      sig_hashes;
   pointer                      sig_algs;
   pointer                      curve_list;
   pointer                      group_list;
   mbedtls_mpi                  dhm_P; 
   mbedtls_mpi                  dhm_G;
   pointer                      psk;
   pointer                      psk_len; // size_t
   pointer                      psk_identity;
   pointer                      psk_identity_len; // size_t
   pointer                      alpn_list;
   int                          read_timeout;
   int                          hs_timeout_min;
   int                          hs_timeout_max;
   int                          renego_max_records;
   byte                         renego_period[8];
   int                          badmac_limit;
   int                          dhm_min_bitlen;
   pointer                      user_data;
   pointer                      f_cert_cb;
   pointer                      dn_hints; 
}

struct mbedtls_ssl_context
{
   pointer                      conf;
   int                          state;
   int                          renego_status;
   int                          renego_records_seen;
   int                          tls_version; 
   int                          badmac_seen;
   pointer                      f_vrfy;
   pointer                      p_vrfy;
   pointer                      f_send;
   pointer                      f_recv;
   pointer                      f_recv_timeout;
   pointer                      p_bio;
   pointer                      session_in;
   pointer                      session_out;
   pointer                      session;
   pointer                      session_negotiate;
   pointer                      handshake;
   pointer                      transform_in;
   pointer                      transform_out;
   pointer                      transform;
   pointer                      transform_negotiate;
   pointer                      p_timer;
   pointer                      f_set_timer;
   pointer                      f_get_timer;
   pointer                      in_buf;
   pointer                      in_ctr;
   pointer                      in_hdr;
   pointer                      in_cid;
   pointer                      in_len;
   pointer                      in_iv;
   pointer                      in_msg;
   pointer                      in_offt;
   int                          in_msgtype;
   pointer                      in_msglen; // size_t
   pointer                      in_left;   // size_t
   short                        in_epoch;
   pointer                      next_record_offset; // size_t
   long                         in_window_top;
   long                         in_window;
   pointer                      in_hslen;  // size_t
   int                          nb_zero;
   int                          keep_current_message;
   byte                         send_alert;
   byte                         alert_type;
   int                          alert_reason;
   byte                         disable_datagram_packing;

   pointer                      out_buf;
   pointer                      out_ctr;
   pointer                      out_hdr;
   pointer                      out_cid;
   pointer                      out_len;
   pointer                      out_iv;
   pointer                      out_msg;

   int                          out_msgtype;
   pointer                      out_msglen; // size_t
   pointer                      out_left;   // size_t

   byte                         cur_out_ctr[8];
   short                        mtu;
   pointer                      hostname;
   pointer                      alpn_chosen;
   pointer                      cli_id;
   pointer                      cli_id_len; // size_t
   int                          secure_renegotiation;
   pointer                      verify_data_len; // size_t    
   byte                         own_verify_data[12]; 
   byte                         peer_verify_data[12]; 
   byte                         own_cid[32]; 
   byte                         own_cid_len;
   byte                         negotiate_cid;
   pointer                      f_export_keys;
   pointer                      p_export_keys;
   pointer                      user_data;

   int                          struct_padding; // !! to align the struct size with required
}

struct mbedtls_net_context
{
   int fd;
}

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
*/
   mbedtls_net_context server_fd := default;
/*   mbedtls_entropy_context entropy;
   mbedtls_ctr_drbg_context ctr_drbg;*/
   mbedtls_ssl_context ssl;
   mbedtls_ssl_config conf;
/*   mbedtls_x509_crt cacert;
*/

   int size := $size conf;
   int size3 := $size ssl;

   extern wrpmbedtls.net_init(server_fd);
   extern wrpmbedtls.ssl_init(ssl);
   extern wrpmbedtls.ssl_config_init(conf);
/*   mbedtls_x509_crt_init(&cacert);
   mbedtls_ctr_drbg_init(&ctr_drbg);

   const char* pers = DRBG_PERSONALIZED_STR;

   extern mbedtls.mbedtls_entropy_init(entropy);

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
   mbedtls_ctr_drbg_free(&ctr_drbg);
   mbedtls_entropy_free(&entropy);*/
}