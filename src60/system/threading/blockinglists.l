import system'collections'threadsafe;

public sealed class BlockingQueue<T>
{
   ThreadsafeQueue<T> _list := new ThreadsafeQueue<T>();

   constructor()
   {
   }
}