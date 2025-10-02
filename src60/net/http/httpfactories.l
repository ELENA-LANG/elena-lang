import system'net;

public class SocketFactory
{
   static object _sync := new object();

   static INetSocketFactory _httpFactory;
   static INetSocketFactory _httpsFactory;

   static constructor()
   {
      _httpFactory := NativeSocketFactory.create();
   }

   static assignHttpsFactory(INetSocketFactory factory)
   {
      lock(_sync) {
         _httpsFactory := factory;
      }
   }

   static assignHttpFactory(INetSocketFactory factory)
   {
      lock(_sync) {
         _httpFactory := factory;
      }
   }

   static INetSocket openSocket(Uri uri)
   {
      INetSocket socket;
      string scheme := uri.Scheme;
      short port := uri.Port;
      scheme =>
         "http" : 
         {
            if (port==0)
               port := 80;

            lock(_sync) {
               socket := _httpFactory.openSocket(uri.Host, port);
            }
         } 
         "https" : 
         {
            if (port==0)
               port := 443;

            lock(_sync) {
               socket := _httpsFactory.openSocket(uri.Host, port);
            }
         }
         ! : {
            InvalidArgumentException.raise("uri");
         };
      
      ^ socket 
   }
   
   static INetListener openListener(string prefix, short port, bool noDelay)
   {
      INetListener socket;
      prefix =>
         "http" :
         {
            socket := _httpFactory.openListener(port, noDelay);
         }
         "https" :
         {
            socket := _httpsFactory.openListener(port, noDelay);
         }
         ! : {
            InvalidArgumentException.raise("prefix");
         };
      
      ^ socket 
   }
}

