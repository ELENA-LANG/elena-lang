import system'threading;

public sealed ChatServer
{
   Thread _broadcastThread;

   constructor new(int port)
   {
   }

   private bool proceed()
   {
      ^ true
   }

   start()
   {
      _broadcastThread := Thread.assign(
      {
         while (proceed()) {}
      });
   }
}