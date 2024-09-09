import system;
import system'collections;

// --- Queue Template ---
 
public sealed ThreadsafeQueue<T>
{
   object _sync;

   List<T>        _array;
   Reference<int> _top;
   Reference<int> _tale;
     
   constructor()
   {
      _sync := new object();

      _array := new List<T>(8);
      _top := 0;
      _tale := 0;
   }
         
   int Length
   {
      get()
      {
         int n := 0;

         lock(_sync) {
            int top := *_top;
            int tale := *_tale;
                
            n := tale - top;
         };
             
         ^ n
      }
   }
     
   push(T object)
   {
      lock(_sync) {
         int capacity := _array.Length;
         int tale := *_tale;
            
         if (tale < capacity)
         { 
            _array.setAt(tale, object) 
         }
         else
         { 
            _array.append(object) 
         };
                
         _tale.append(1)
      }
   }
     
   T pop()
   {
      lock(_sync) {
         int top := *_top;
         int tale := *_tale;
            
         if (tale == top)
            { InvalidOperationException.new("Queue is empty").raise() };

         T item := _array.at(top);
                            
         _top.append(1);
            
         // reuse the space if the queue is empty
         if(tale == top)
         {
            _top.Value := 0;
            _tale.Value := 0
         };
            
         ^ item
      }
   }

   get Queue<T> Snapshot()
      : info("Returns a copy of the queue")
   {
      int top := 0;
      int tale := 0;
      List<T> copy := nil;

      lock(_sync) {
         top := *_top;
         tale := *_tale;
         copy := _array.clone();
      };

      var Queue<T> q := new Queue<T>();
      for (int i := top; i <= tale; i++) {
         q.push(copy[i]);
      };

      ^ q
   }

   clear()
   {
      lock(_sync) {
         _array.clear();
         _top.Value :=0;
         _tale.Value:=0
      }
   }    
}    
