/* *********************************************************************
ELENA Project : MbedTls based SSL Socket

NOTE : the prject requires wrpmbedtls.dll 

It can be downloaded from https://github.com/ELENA-LANG/mbedtls-as-dll

********************************************************************* */

const string DRBG_PERSONALIZED_STR = "ELENA API6 client";

const int MBEDTLS_NET_PROTO_TCP = 0;

const int MBEDTLS_SSL_IS_CLIENT = 0;
const int MBEDTLS_SSL_IS_SERVER = 1;

const int MBEDTLS_SSL_TRANSPORT_STREAM = 0;
const int MBEDTLS_SSL_PRESET_DEFAULT   = 0;
const int MBEDTLS_SSL_VERIFY_NONE      = 0;

const int MBEDTLS_ERR_SSL_WANT_READ                         = -26880;
const int MBEDTLS_ERR_SSL_WANT_WRITE                        = -26752;

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

//--- MbedTlsException ---

public class MbedTlsException : Exception
{
   constructor new(string s)
      <= super new(s);
}

public class MbedTlsEnvironment
{
   constructor init(string customStr, int endpoint, int transport, int preset)
   {
      extern wrpmbedtls.mbedtls_startup();

      int ret := extern wrpmbedtls.mbedtls_drbg_seed_def(customStr, customStr.Length + 1);
      if (ret != 0)
      {
         MbedTlsException.raise($"Failed. mbedtls_ctr_drbg_seed returned {ret}");
      };

      ret := extern wrpmbedtls.mbedtls_config_ssl(endpoint, transport, preset);
      if (ret != 0)
      {
         MbedTlsException.raise($"Failed. mbedtls_ssl_config_defaults returned {ret}");
      };
   }

   internal setupClient(int authmode)
   {
      extern wrpmbedtls.mbedtls_client_setup(authmode);
   }

   internal setupServer()
   {
   }
}

public static MbedTlsEnvironment MbedTlsClientStartup
   = MbedTlsEnvironment.init(DRBG_PERSONALIZED_STR, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT).setupServer();

public static MbedTlsEnvironment MbedTlsServerStartup
   = MbedTlsEnvironment.init(DRBG_PERSONALIZED_STR, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT).setupServer();
