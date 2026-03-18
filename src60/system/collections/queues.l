namespace collections
{
   // --- Queue ---
    
   public sealed class Queue : Enumerable, info("A weak FIFO list")
   {
      ArrayList       _list;
      Reference<int>  _top;
      Reference<int>  _tale;
        
      constructor() : info("Creates an empty queue")
      {
         _list := new ArrayList();
         _top := 0;
         _tale := 0
      }
        
      ArrayList cast() = _list;
        
      Enumerator enumerator() : info("Returns the queue enumerator") = new QueueEnumerator(self);
    
      internal int TopPointer = _top;
      internal int TalePointer = _tale;    
            
      int Length : info("Returns the queue length")
      {
         get()
         {
            int tale := *_tale;
            int top := *_top;
                
            int n := tale - top;
                
            ^ n
         }
      }
        
      push(object) : info("Puts an element at the queue end")
      {
         int capacity := _list.Length;        
         int tale := *_tale;
         if (tale < capacity)
         { 
            _list.setAt(tale, object) 
         }
         else
         { 
             _list.append(object)
         };
                
         _tale.append(1)
      }
        
      peek() : info("Returns an element at the queue top without removing it from the queue<br/>The exception is raised if the queue is empty")
      {
         int top := *_top;
         int tale := _tale;
            
         if (tale == top)
            { InvalidOperationException.new("Queue is empty").raise() };
                
         ^ _list.at(top)
      }
        
      pop() : info("Returns an element at the queue top and removes it from the queue<br/>The exception is raised if the queue is empty")
      {
         int top := *_top;
         int tale := *_tale;
            
         if(tale == top)
            { InvalidOperationException.new("Queue is empty").raise() };
    
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
            
      clear() : info("Removes all elements from the queue")
      {
         _list.clear();
         _top.Value := 0;
         _tale.Value := 0
      }
        
      shift(offset) : info("Moves the element at the queue top down to <b>offset</b> postions")
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
    
   public sealed Queue<T> : Enumerable<T>, info("A strong-typed FIFO list template")
   {
      List<T>        _array;
      Reference<int> _top;
      Reference<int> _tale;
        
      constructor() : info("Creates an empty queue")
      {
         _array := new List<T>(8);
         _top := 0;
         _tale := 0;
      }
        
      constructor allocate(int capacity) : info("Creates an empty queue with preallocated space")
      {
         _array := new List<T>(capacity);
         _top := 0;
         _tale := 0;
      }
            
      int Length : info("Returns the queue length")
      {
         get()
         {
            int top := *_top;
            int tale := *_tale;
                
            int n := tale - top;
                
            ^ n
         }
      }
        
      bool isEmpty() : info("Returns true if the queue is empty")
      {
         int top := *_top;
         int tale := *_tale;
            
         ^ top == tale
      }
        
      bool isNotEmpty() : info("Returns true if the queue is not empty")
      {
         int top := *_top;
         int tale := *_tale;
            
         ^ top != tale
      }
        
      push(T object) : info("Puts an element at the queue end")
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
        
      T peek() : info("Returns an element at the queue top without removing it from the queue<br/>The exception is raised if the queue is empty")
      {
         int top := *_top;
         int tale := *_tale;
            
         if (tale == top)
            { InvalidOperationException.new("Queue is empty").raise() };
                
         ^ _array.at(top)
      }        
        
      T pop() : info("Returns an element at the queue top and removes it from the queue<br/>The exception is raised if the queue is empty")
      {
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
    
      Enumerator<T> enumerator() : info("Returns the queue enumerator") = new BaseEnumerator<T>
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
            
      clear() : info("Removes all elements from the queue")
      {
         _array.clear();
         _top.Value :=0;
         _tale.Value:=0
      }    
   }    

   // --- QueueEnumerator ---
    
   class QueueEnumerator : Enumerator, info("A weak queue enumerator")
   {
      Queue     _queue;
      ArrayList _list;
      int       _index; 
      int       _tale; 
        
      constructor(Queue queue) : info("Creates an enumerator of the given queue")
      {
         _queue := queue;
         _list := queue;
            
         _index := queue.TopPointer - 1;
         _tale := queue.TalePointer
      }
        
      bool next() : info("Goes to the next enumeration member and returns true if it is not the last member")
      {
         _index += 1;
         if (_index < _tale)
         {
            ^ true
         };

         ^ false
      }
            
      get Value() : info("Returns the current queue element")
         = _list[_index];
            
      reset() : info("Resets the enumerator, so it can be used again")
      {
         _list := _queue;
         _index := _queue.TopPointer;
         _tale := _queue.TalePointer
      }   
        
      enumerable() : info("Returns an enumerable queue") = _queue;
   }

   // --- Stack ---
    
   public sealed class Stack : Enumerable, info("A weak LIFO list")
   {
      ArrayList      _list;
      Reference<int> _top;
        
      constructor() : info("Creates an empty queue")
      {
         _list := new ArrayList();
         _top := 0
      }
        
      internal int TopPointer = _top;
        
      ArrayList cast() = _list;
        
      Enumerator enumerator() : info("Returns the stack enumerator") = new StackEnumerator(self);
        
      int Length : info("Returns the number of elements in the stack")
      {
         get()
         {
            int retVal := *_top;
                
            ^ retVal
         }
      }        
        
      bool isEmpty() : info("Returns true if the stack is empty")
      {
         int top := *_top;
            
         ^ top == 0
      }
        
      bool isNotEmpty() : info("Returns true if the stack is not empty")
      {
         int top := *_top;
            
         ^ top != 0
      }
        
      push(object) : info("Puts an element at the stack top")
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
        
      peek() : info("Returns an element at the stack top without removing it from it<br/>The exception is raised if the queue is empty")
      {
         int top := *_top;
            
         if (0 == top)
            { InvalidOperationException.raise() };
                
         ^ _list[top - 1]
      }        
        
      pop() : info("Returns an element at the stack top and removes it from the stack<br/>The exception is raised if the queue is empty")
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
        
      clear() : info("Removes all elements from the stack")
      {
         _list.clear();
         _top.Value := 0
      }
   }
    
   // --- Stack Template ---
    
   public sealed Stack<T> : Enumerable<T>, info("A strong-typed LIFO list template")
   {
      List<T>        _array;
      Reference<int> _top;
        
      constructor() : info("Creates an empty queue")
      {
         _array := new List<T>(8);
         _top := 0;
      }
        
      constructor allocate(int capacity) : info("Creates an empty stack with preallocated space")
      {
         _array := new List<T>(capacity);
         _top := 0;
      }
        
      int Length : info("Returns the number of elements in the stack")
      {
         get()
         {
            int retVal := *_top;
                
            ^ retVal
         }
      }
            
      bool isEmpty() : info("Returns true if the stack is empty")
         = 0 == *_top;
        
      bool isNotEmpty() : info("Returns true if the stack is not empty")
         = 0 != *_top;
        
      push(T o) : info("Puts an element at the stack top")
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
        
      T peek() : info("Returns an element at the stack top without removing it from it<br/>The exception is raised if the queue is empty")
      {
         if (0 == *_top)
            { InvalidOperationException.new("Stack is empty").raise() };
                
         ^ _array.at(*_top - 1)
      }
        
      T pop() : info("Returns an element at the stack top and removes it from the stack<br/>The exception is raised if the queue is empty")
      {
         if (0 == *_top)
            { InvalidOperationException.new("Stack is empty").raise() };
                
         _top.reduce(1);
            
         T item := _array.at(*_top);
         //theArray setAt int:theTop type:$nil.
            
         ^ item
      }
        
      clear() : info("Removes all elements from the stack")
      {
         _array.clear();
         _top.Value := 0
      }    
        
      Enumerator<T> enumerator() : info("Returns the queue enumerator") = new BaseEnumerator<T>
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
    
   public class StackEnumerator : Enumerator, info("A weak stack enumerator")
   {
      Stack     _stack;
      ArrayList _list;
      int       _index; 
        
      constructor(Stack stack) : info("Creates an enumerator of the given stack")
      {
         _stack := stack;
         _list := stack;
         _index := stack.TopPointer;
      }
        
      bool next() : info("Goes to the next enumeration member and returns true if it is not the last member")
      {
         if (_index > 0)
         {
            _index -= 1;
                
            ^ true
         };

         ^ false
      }
            
      get Value() : info("Returns the current stack element")
         = _list[_index];
            
      reset() : info("Resets the enumerator, so it can be used again")
      {
         _list := _stack;
         _index := _stack.TopPointer
      }   
        
      enumerable() : info("Returns an enumerable stack") = _stack;
   }
}