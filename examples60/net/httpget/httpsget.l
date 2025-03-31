import system'text;

struct mbedtls_aes_context 
{
   int      nr;
   pointer  rk_offset; // size_t
   int      buf[68];
}

struct mbedtls_ctr_drbg_context
{
   byte                 counter[16];
   int                  reseed_counter;
   int                  prediction_resistance;
   pointer              entropy_len; // size_t
   int                  reseed_interval;
   mbedtls_aes_context  aes_ctx;
   pointer              f_entropy;
   pointer              p_entropy; 
   
}

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

const string DRBG_PERSONALIZED_STR = "ELENA API6 client";

const string SERVER_NAME = "os.mbed.com";
const string SERVER_PORT = "443";

const int MBEDTLS_NET_PROTO_TCP = 0;

const int MBEDTLS_SSL_IS_CLIENT = 0;
const int MBEDTLS_SSL_TRANSPORT_STREAM = 0;
const int MBEDTLS_SSL_PRESET_DEFAULT   = 0;
const int MBEDTLS_SSL_VERIFY_NONE      = 0;

const string HTTP_REQUEST_FILE_PATH =
                                    "/media/uploads/mbed_official/hello.txt";


const int MBEDTLS_ERR_SSL_WANT_READ                         = -26880;
const int MBEDTLS_ERR_SSL_WANT_WRITE                        = -26752;

public test()
{
   int ret := 0;

   mbedtls_net_context server_fd := default;
   mbedtls_entropy_context entropy := default;
   mbedtls_ctr_drbg_context ctr_drbg := default;
   mbedtls_ssl_context ssl := default;
   mbedtls_ssl_config conf := default;
/*   mbedtls_x509_crt cacert;
*/

   int size := $size conf;
   int size3 := $size ssl;
   int size4 := $size ctr_drbg;
   int size5 := $size entropy;

   extern wrpmbedtls.net_init(server_fd);
   extern wrpmbedtls.ssl_init(ssl);
   extern wrpmbedtls.ssl_config_init(conf);
//   mbedtls_x509_crt_init(&cacert);
   extern wrpmbedtls.ctr_drbg_init(ctr_drbg);

   string pers := DRBG_PERSONALIZED_STR;

   extern wrpmbedtls.entropy_init(entropy);

   ret := extern wrpmbedtls.ctr_drbg_seed_def(ctr_drbg, entropy, DRBG_PERSONALIZED_STR, DRBG_PERSONALIZED_STR.Length + 1);

   if (ret != 0)
   {
      Exception.raise($"Failed. mbedtls_ctr_drbg_seed returned {ret}");
   };

   ret := extern wrpmbedtls.net_connect(server_fd, SERVER_NAME, SERVER_PORT, MBEDTLS_NET_PROTO_TCP);
   if (ret != 0)
   {
      Exception.raise($"Failed. mbedtls_net_connect returned {ret}");
   };

   ret := extern wrpmbedtls.ssl_config_defaults(conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

   if (ret != 0)
   {
      Exception.raise($"Failed. mbedtls_ssl_config_defaults returned {ret}");
   };

   extern wrpmbedtls.ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);

   extern wrpmbedtls.ssl_conf_rng_def(conf, ctr_drbg);
//   extern wrpmbedtls.ssl_conf_dbg(conf, my_debug, stdout);

   ret := extern wrpmbedtls.ssl_setup(ssl, conf);
   if (ret != 0) 
   {
      Exception.raise($"Failed. mbedtls_ssl_setup returned {ret}");
   };

   ret := extern wrpmbedtls.ssl_set_hostname(ssl, SERVER_NAME);
   if (ret != 0)
   {
      Exception.raise($"Failed. mbedtls_ssl_set_hostname returned {ret}");
   };

   extern wrpmbedtls.ssl_set_bio_def(ssl, server_fd);

   string request := $"GET {HTTP_REQUEST_FILE_PATH} HTTP/1.1"$13$10"Host:{SERVER_NAME}"$13$10$13$10;

//       int e = MBEDTLS_ERR_SSL_WANT_WRITE;

   auto buffer := new StringBuilder();
   buffer.write(request);

   // Send the HTTP request to the server over TLS
   int len := request.Length;
   while (len > 0) {
      ret := extern wrpmbedtls.ssl_write(ssl, buffer.Value, len);

      if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
         ret := 0;
      }
      else if (ret < 0) {
         Exception.raise($"Failed. mbedtls_ssl_write returned {ret}");
      };

      len -= ret;
      if (len > 0) {
         buffer.delete(0, ret);
      }
   };
/*
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
*/   
       // Print information about the TLS connection
       byte gp_buf[1024];
       ret := extern wrpmbedtls.x509_crt_info(gp_buf, 1024,
                                   ""$10"  ", extern wrpmbedtls.ssl_get_peer_cert(ssl));
       if (ret < 0) {
           Exception.raise($"Failed. mbedtls_x509_crt_info returned {ret}");
       };
       //mbedtls_printf("Server certificate:\n%s\n", gp_buf);
   
       // Ensure certificate verification was successful
       int flags := extern wrpmbedtls.ssl_get_verify_result(ssl);
       if (flags != 0) {
           ret := extern wrpmbedtls.x509_crt_verify_info(gp_buf, 1024,
                                              ""$10"  ! ", flags);
           if (ret < 0) {
               Exception.raise($"Failed. mbedtls_x509_crt_verify_info returned {ret}");
           } 
           else {
               Exception.raise("Certificate verification failed");
           }
       } ;
//       else {
           //mbedtls_printf("Certificate verification passed\n");
//       }
   
       //mbedtls_printf("Established TLS connection to %s\n", server_name);
   
       // Read response from the server
//       int resp_offset := 0;
       bool resp_200 := false;
       bool resp_hello := false;
//       do {
           ret := extern wrpmbedtls.ssl_read(ssl,
                       gp_buf,
                       /*sizeof(gp_buf) - resp_offset - 1*/1024);
//           if (ret > 0)
//               resp_offset += static_cast<size_t>(ret);
   
//           gp_buf[resp_offset] = '\0';
   
//           resp_200 = resp_200 || strstr(gp_buf, HTTP_OK_STR) != NULL;
//           resp_hello = resp_hello || strstr(gp_buf, HTTP_HELLO_STR) != NULL;
//       } while((!resp_200 || !resp_hello) &&
//               (ret > 0 ||
//               ret == MBEDTLS_ERR_SSL_WANT_READ || MBEDTLS_ERR_SSL_WANT_WRITE));
       if (ret < 0) {
           Exception.raise($"Failed. mbedtls_ssl_read returned {ret}");
       };
   
   string s := UTF8Encoding.toString(0, ret, gp_buf);

   extern wrpmbedtls.net_free(server_fd);
   extern wrpmbedtls.ssl_free(ssl);
   extern wrpmbedtls.ssl_config_free(conf);
   extern wrpmbedtls.ctr_drbg_free(ctr_drbg);
   extern wrpmbedtls.entropy_free(entropy);
}