public class HttpsServer: interface<IHttpServer>
{
   protected run()
   {
      NotSupportedException.raise()
   }
   
   start()
   {
      NotSupportedException.raise()
      
//      _listenThread := Thread.start(
//      {
//         run();
//      });
   }
}