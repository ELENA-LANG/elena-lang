namespace collections
{
   // --- Queue ---
    
   public sealed class Queue : Enumerable
   {
      ArrayList       _list;
      Reference<int>  _top;
      Reference<int>  _tale;
        
      constructor()
      {
         _list := new ArrayList();
         _top := 0;
         _tale := 0
      }
        
      ArrayList cast() = _list;
        
      Enumerator enumerator() = new QueueEnumerator(self);
    
      internal int TopPointer = _top;
      internal int TalePointer = _tale;    
            
      int Length
      {
         get()
         {
            int tale := *_tale;
            int top := *_top;
                
            int n := tale - top;
                
            ^ n
         }
      }
        
      push(object)
      {
         int capacity := _list.Length;        
         int tale := *_tale;
         if (tale < capacity)
         { 
            _list.setAt(tale, object) 
         }
         else
         { 
             _list.append:object 
         };
                
         _tale.append(1)
      }
        
      peek()
      {
         int top := *_top;
         int tale := _tale;
            
         if (tale == top)
            { InvalidOperationException.new:"Queue is empty".raise() };
                
         ^ _list.at(top)
      }
        
      pop()
      {
         int top := *_top;
         int tale := *_tale;
            
         if(tale == top)
            { InvalidOperationException.new:"Queue is empty".raise() };
    
         var item := _list.at(top);
                                        
         _top.append(1);
            
         // reuse the space if the queue is empty
         if(tale == top)
         {
            _top.Value := 0;
            _tale.Value := 0
         };
                
         ^ item
      }
            
      clear()
      {
         _list.clear();
         _top.Value := 0;
         _tale.Value := 0
      }
        
      shift(offset)
      {
         int i := *_top;
         int n := (*_tale) - 1;
            
         var topItem := _list.at(i);
            
         while (i < n)
         {
            _list.setAt(i, _list.at(i + 1));
                                
            i := i + 1
         };
            
         _list.setAt(i, topItem)
      }
   }
    
   // --- Queue Template ---
    
   public sealed Queue<T> : Enumerable<T>
   {
      List<T>        _array;
      Reference<int> _top;
      Reference<int> _tale;
        
      constructor()
      {
         _array := new List<T>(8);
         _top := 0;
         _tale := 0;
      }
        
      constructor allocate(int capacity)
      {
         _array := new List<T>(capacity);
         _top := 0;
         _tale := 0;
      }
            
      int Length
      {
         get()
         {
            int top := *_top;
            int tale := *_tale;
                
            int n := tale - top;
                
            ^ n
         }
      }
        
      bool isEmpty()
      {
         int top := *_top;
         int tale := *_tale;
            
         ^ top == tale
      }
        
      bool isNotEmpty()
      {
         int top := *_top;
         int tale := *_tale;
            
         ^ top != tale
      }
        
      push(T object)
      {
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
        
      T peek()
      {
         int top := *_top;
         int tale := *_tale;
            
         if (tale == top)
            { InvalidOperationException.new:"Queue is empty".raise() };
                
         ^ _array.at(top)
      }        
        
      T pop()
      {
         int top := *_top;
         int tale := *_tale;
            
         if (tale == top)
            { InvalidOperationException.new:"Queue is empty".raise() };
                            
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
    
      Enumerator<T> enumerator() = new BaseEnumerator<T>
      {
         this index := -1;
            
         this length := self.Length;
            
         get T Value() = _array.at(this index);
            
         enumerable() = self;
            
         reset()
         {
                this index := -1
         }
      };
            
      clear()
      {
         _array.clear();
         _top.Value :=0;
         _tale.Value:=0
      }    
   }    

   // --- QueueEnumerator ---
    
   class QueueEnumerator : Enumerator
   {
      Queue     _queue;
      ArrayList _list;
      int       _index; 
      int       _tale; 
        
      constructor(Queue queue)
      {
         _queue := queue;
         _list := queue;
            
         _index := queue.TopPointer - 1;
         _tale := queue.TalePointer
      }
        
      bool next()
      {
         _index += 1;
         if (_index < _tale)
         {
            ^ true
         };

         ^ false
      }
            
      get Value()
         = _list[_index];
            
      reset()
      {
         _list := _queue;
         _index := _queue.TopPointer;
         _tale := _queue.TalePointer
      }   
        
      enumerable() = _queue;
   }

   // --- Stack ---
    
   public sealed class Stack : Enumerable
   {
      ArrayList      _list;
      Reference<int> _top;
        
      constructor()
      {
         _list := new ArrayList();
         _top := 0
      }
        
      internal int TopPointer = _top;
        
      ArrayList cast() = _list;
        
      Enumerator enumerator() = new StackEnumerator(self);
        
      int Length
      {
         get()
         {
            int retVal := *_top;
                
            ^ retVal
         }
      }        
        
      bool isEmpty()
      {
         int top := *_top;
            
         ^ top == 0
      }
        
      bool isNotEmpty()
      {
         int top := *_top;
            
         ^ top != 0
      }
        
      push(object)
      {
         int capacity := _list.Length;
         int top := *_top;
            
         if (top < capacity)
         {
            _list.setAt(top, object)
         }
         else
         {
            _list.append(object)
         };
                
         _top.append(1)
      }
        
      peek()
      {
         int top := *_top;
            
         if (0 == top)
            { InvalidOperationException.raise() };
                
         ^ _list[top - 1]
      }        
        
      pop()
      {
         int top := *_top;
            
         if (0 == top)
            { InvalidOperationException.new("Stack is empty").raise() };
                
         _top.reduce(1);
            
         top -= 1;
            
         var item := _list[top];
         _list[top] := nil;
            
         ^ item
      }
        
      clear()
      {
         _list.clear();
         _top.Value := 0
      }
   }
    
   // --- Stack Template ---
    
   public sealed Stack<T> : Enumerable<T>
   {
      List<T>        _array;
      Reference<int> _top;
        
      constructor()
      {
         _array := new List<T>(8);
         _top := 0;
      }
        
      constructor allocate(int capacity)
      {
         _array := new List<T>(capacity);
         _top := 0;
      }
        
      int Length
      {
         get()
         {
            int retVal := *_top;
                
            ^ retVal
         }
      }
            
      bool isEmpty()
         = 0 == *_top;
        
      bool isNotEmpty()
         = 0 != *_top;
        
      push(T o)
      {
         int capacity := _array.Length;
            
         if (*_top < capacity)
         { 
            _array.setAt(*_top, o) 
         }
         else
         {
            _array.append(o)
         };
                
         _top.append(1)
      }
        
      T peek()
      {
         if (0 == *_top)
            { InvalidOperationException.new:"Stack is empty".raise() };
                
         ^ _array.at(*_top - 1)
      }
        
      T pop()
      {
         if (0 == *_top)
            { InvalidOperationException.new:"Stack is empty".raise() };
                
         _top.reduce(1);
            
         T item := _array.at(*_top);
         //theArray setAt int:theTop type:$nil.
            
         ^ item
      }
        
      clear()
      {
         _array.clear();
         _top.Value := 0
      }    
        
      Enumerator<T> enumerator() = new BaseEnumerator<T>
      {
         this index := -1;
            
         this length := self.Length;
            
         get T Value() = _array.at(_top - this index - 1);
            
         enumerable() = self;
            
         reset()
         {
            this index := -1
         }
      };
   }
    
   // --- StackEnumerator ---
    
   public class StackEnumerator : Enumerator
   {
      Stack     _stack;
      ArrayList _list;
      int       _index; 
        
      constructor(Stack stack)
      {
         _stack := stack;
         _list := stack;
         _index := stack.TopPointer;
      }
        
      bool next()
      {
         if (_index > 0)
         {
            _index -= 1;
                
            ^ true
         };

         ^ false
      }
            
      get Value()
         = _list[_index];
            
      reset()
      {
         _list := _stack;
         _index := _stack.TopPointer
      }   
        
      enumerable() = _stack;
   }
}