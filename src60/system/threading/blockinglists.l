import system'collections'threadsafe;

public sealed class BlockingQueue<T>
{
   ConcurrentQueue<T> _list := new ConcurrentQueue<T>();
   Semaphore          _semaphore;

   constructor()
   {
      _semaphore := Semaphore.create(0, IntNumber.MaxValue);
   }

   push(T value)
   {
      _list.push(value);

      _semaphore.release();
   }

   T pop()
   {
      _semaphore.wait();

      ^ _list.pop();
   }
}