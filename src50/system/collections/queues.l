namespace collections
{

    // --- Stack ---
    
    public sealed class Stack : Enumerable
    {
        ArrayList theList;
        ref<int>  theTop;
        
        constructor()
        {
            theList := new ArrayList();
            theTop := 0
        }
        
        internal int TopPointer = theTop;
        
        ArrayList cast() = theList;
        
        Enumerator enumerator() = new StackEnumerator(self);
        
        int Length
        {
            get()
            {
                int retVal := theTop.Value;
                
                ^ retVal
            }
        }        
        
        bool isEmpty()
        {
            int top := theTop.Value;
            
            ^ top == 0
        }
        
        bool isNotEmpty()
        {
            int top := theTop.Value;
            
            ^ top != 0
        }
        
        push(object)
        {
            int capacity := theList.Length;
            int top := theTop.Value;
            
            if (top < capacity)
            {
                theList.setAt(top, object)
            }
            else
            {
                theList.append(object)
            };
                
            theTop.append(1)
        }
        
        peek()
        {
            int top := theTop.Value;
            
            if (0 == top)
                { InvalidOperationException.raise() };
                
            ^ theList[top - 1]
        }        
        
        pop()
        {
            int top := theTop.Value;
            
            if (0 == top)
                { InvalidOperationException.new("Stack is empty").raise() };
                
            theTop.reduce(1);
            
            top -= 1;
            
            var item := theList[top];
            theList[top] := nil;
            
            ^ item
        }
        
        clear()
        {
            theList.clear();
            theTop.Value := 0
        }
    }
    
    // --- Stack Template ---
    
    public sealed Stack<T> : Enumerable<T>
    {
        List<T>  theArray;
        ref<int> theTop;
        
        constructor()
        {
            theArray := new List<T>(8);
            theTop := 0;
        }
        
        constructor allocate(int capacity)
        {
            theArray := new List<T>(capacity);
            theTop := 0;
        }
        
        int Length
        {
            get()
            {
                int retVal := theTop.Value;
                
                ^ retVal
            }
        }
            
        bool isEmpty()
            = 0 == theTop.Value;
        
        bool isNotEmpty()
            = 0 != theTop.Value;
        
        push(T o)
        {
            int capacity := theArray.Length;
            
            if (cast int(theTop) < capacity)
            { 
                theArray.setAt(cast int(theTop), o) 
            }
            else
            {
                theArray.append(o)
            };
                
            theTop.append(1)
        }
        
        T peek()
        {
            if (0 == (cast int(theTop)))
                { InvalidOperationException.new:"Stack is empty".raise() };
                
            ^ theArray.at(cast int(theTop) - 1)
        }
        
        T pop()
        {
            if (0 == (cast int(theTop)))
                { InvalidOperationException.new:"Stack is empty".raise() };
                
            theTop.reduce(1);
            
            T item := theArray.at(cast int(theTop));
            //theArray setAt int:theTop type:$nil.
            
            ^ item
        }
        
        clear()
        {
            theArray.clear();
            theTop.Value := 0
        }    
        
        Enumerator<T> enumerator() = new BaseEnumerator<T>
        {
            this theIndex := -1;
            
            this theLength := self.Length;
            
            get T Value() = theArray.at(cast int(theTop) - (this theIndex) - 1);
            
            enumerable() = self;
            
            reset()
            {
                this theIndex := -1
            }
        };
    }
    
    // --- Queue ---
    
    public sealed class Queue : Enumerable
    {
        ArrayList theList;
        ref<int>  theTop;
        ref<int>  theTale;
        
        constructor()
        {
            theList := new ArrayList();
            theTop := 0;
            theTale := 0
        }
        
        ArrayList cast() = theList;
        
        Enumerator enumerator() = new QueueEnumerator(self);
    
        internal int TopPointer = theTop;
        internal int TalePointer = theTale;    
            
        int Length
        {
            get()
            {
                int tale := theTale.Value;
                int top := theTop.Value;
                
                int n := tale - top;
                
                ^ n
            }
        }
        
        push(object)
        {
            int capacity := theList.Length;        
            int tale := theTale.Value;
            if (tale < capacity)
            { 
                theList.setAt(theTale, object) 
            }
            else
            { 
                theList.append:object 
            };
                
            theTale.append(1)
        }
        
        peek()
        {
            int top := theTop.Value;
            int tale := theTale.Value;
            
            if (tale == top)
                { InvalidOperationException.new:"Queue is empty".raise() };
                
            ^ theList.at(top)
        }
        
        pop()
        {
            int top := theTop.Value;
            int tale := theTale.Value;
            
            if(tale == top)
                { InvalidOperationException.new:"Queue is empty".raise() };
    
            var item := theList.at(top);
                                        
            theTop.append(1);
            
            // reuse the space if the queue is empty
            if(tale == top)
            {
                theTop.Value := 0;
                theTale.Value := 0
            };
                
            ^ item
        }
            
        clear()
        {
            theList.clear();
            theTop.Value := 0;
            theTale.Value := 0
        }
        
        shift(offset)
        {
            int i := theTop;
            int n := theTale - 1;
            
            var topItem := theList.at(i);
            
            while (i < n)
            {
                theList.setAt(i, theList.at(i + 1));
                                
                i := i + 1
            };
            
            theList.setAt(i, topItem)
        }
    }
    
    // --- Queue Template ---
    
    public sealed Queue<T> : Enumerable<T>
    {
        List<T>  theArray;
        ref<int> theTop;
        ref<int> theTale;
        
        constructor()
        {
            theArray := new List<T>(8);
            theTop := 0;
            theTale := 0;
        }
        
        constructor allocate(int capacity)
        {
            theArray := new List<T>(capacity);
            theTop := 0;
            theTale := 0;
        }
            
        int Length
        {
            get()
            {
                int top := theTop.Value;
                int tale := theTale.Value;
                
                int n := tale - top;
                
                ^ n
            }
        }
        
        bool isEmpty()
        {
            int top := theTop.Value;
            int tale := theTale.Value;
            
            ^ top == tale
        }
        
        bool isNotEmpty()
        {
            int top := theTop.Value;
            int tale := theTale.Value;
            
            ^ top != tale
        }
        
        push(T object)
        {
            int capacity := theArray.Length;
            int tale := theTale.Value;
            
            if (tale < capacity)
            { 
                theArray.setAt(tale, object) 
            }
            else
            { 
                theArray.append(object) 
            };
                
            theTale.append(1)
        }
        
        T peek()
        {
            int top := theTop.Value;
            int tale := theTale.Value;
            
            if (tale == top)
                { InvalidOperationException.new:"Queue is empty".raise() };
                
            ^ theArray.at(top)
        }        
        
        T pop()
        {
            int top := theTop.Value;
            int tale := theTale.Value;
            
            if (tale == top)
                { InvalidOperationException.new:"Queue is empty".raise() };
                            
            T item := theArray.at(top);
                            
            theTop.append(1);
            
            // reuse the space if the queue is empty
            if(tale == top)
            {
                theTop.Value := 0;
                theTale.Value := 0
            };
            
            ^ item
        }
    
        Enumerator<T> enumerator() = new BaseEnumerator<T>
        {
            this theIndex := -1;
            
            this theLength := self.Length;
            
            get T Value() = theArray.at(this theIndex);
            
            enumerable() = self;
            
            reset()
            {
                this theIndex := -1
            }
        };
            
        clear()
        {
            theArray.clear();
            theTop.Value :=0;
            theTale.Value:=0
        }    
    }
    
    // --- StackEnumerator ---
    
    public class StackEnumerator : Enumerator
    {
        Stack     theStack;
        ArrayList theList;
        int       theIndex; 
        
        constructor(Stack stack)
        {
            theStack := stack;
            theList := stack;
            theIndex := stack.TopPointer;
        }
        
        bool next()
        {
            if (theIndex > 0)
            {
                theIndex -= 1;
                
                ^ true
            }
            else
            {
                ^ false
            }
        }
            
        get Value()
            = theList[theIndex];
            
        reset()
        {
            theList := theStack;
            theIndex := theStack.TopPointer
        }   
        
        enumerable() = theStack;
    }
    
    // --- QueueEnumerator ---
    
    class QueueEnumerator : Enumerator
    {
        Queue     theQueue;
        ArrayList theList;
        int       theIndex; 
        int       theTale; 
        
        constructor(Queue queue)
        {
            theQueue := queue;
            theList := queue;
            
            theIndex := queue.TopPointer - 1;
            theTale := queue.TalePointer
        }
        
        bool next()
        {
            theIndex += 1;
            if (theIndex < theTale)
            {
                ^ true
            }
            else
            {
                ^ false
            }    
        }
            
        get Value()
            = theList[theIndex];
            
        reset()
        {
            theList := theQueue;
            theIndex := theQueue.TopPointer;
            theTale := theQueue.TalePointer
        }   
        
        enumerable() = theQueue;
    }

}