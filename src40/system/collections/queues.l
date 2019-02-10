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
    
    vprop int Length
    {
        readTo(ref int retVal)
        {
            theTop.readValueTo(ref retVal)
        }
    }        
    
    bool isEmpty()
        = 0 == theTop;
    
    bool isNotEmpty()
        = 0 != theTop;
    
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
    
    vprop int Length
    {
        readTo(ref retVal)
        {
            theTop.readValueTo(ref retVal)
        }
    }
        
//    bool isEmpty
//        = 0 == theTop.
//    
//    bool isNotEmpty
//        = 0 != theTop.
    
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
    
    Enumerator<T> enumerator() = new BaseEnumerator<T>:
    {
        this theIndex := -1;
        
        this theLength := self.Length;
        
        T get() = theArray.at(cast int(theTop) - (this theIndex) - 1);
        
        enumerable() = self;
        
        reset()
        {
            this theIndex := -1
        }
    };
}

//// --- Queue ---
//
//public sealed class Queue :: Enumerable
//{
//    T<ArrayList> theList.
//    ref<int>     theTop.
//    ref<int>     theTale.
//    
//    constructor new
//    [
//        theList := ArrayList new.
//        theTop := 0.
//        theTale := 0.
//    ]
//    
//    cast T<ArrayList> = theList.
//    
//    T<Enumerator> enumerator = QueueEnumerator new(self).
//    
//    int rvprop length :: readLengthTo.
//    
//    push : anObject
//    [
//        int aCapacity := theList length.
//        
//        if (theTale < aCapacity)
//            [ theList setAt(theTale, anObject) ];
//            [ theList append:anObject ].
//            
//        theTale append(1).
//    ]
//    
//    peek
//    [
//        if (theTale == theTop)
//            [ InvalidOperationException new:"Queue is empty"; raise ].
//            
//        ^ theList getAt(T<int>(theTop))
//    ]
//    
//    pop
//    [
//        if(theTale == theTop)
//            [ InvalidOperationException new:"Queue is empty"; raise ].
//
//        var item := theList getAt(T<int>(theTop)).
//                                    
//        theTop append(1).
//        
//        // reuse the space if the queue is empty
//        if(theTale == theTop)
//            [
//                theTop  value := 0.
//                theTale value := 0.
//            ].
//            
//        ^ item
//    ]
//    
//    readLengthTo(ref<int> aRetVal)
//    [
//        int n := T<int>(theTale) - T<int>(theTop).
//        
//        aRetVal value := n.
//    ]
//    
//    clear
//    [
//        theList clear.
//        theTop value :=0.
//        theTale value:=0
//    ]
//    
//    shift : offset
//    [
//        int i := theTop.
//        int n := theTale - 1.
//        
//        var topItem := theList getAt(i).
//        
//        while (i < n)
//        [
//            theList setAt(i, theList getAt(i + 1)).
//            
//            i := i + 1
//        ].
//        
//        theList setAt(i, topItem)
//    ]
//}
//
//// --- Queue Template ---
//
//public sealed Queue<type> :: Enumerable<type>
//{
//    collections'List<type> theArray.
//    ref<int>       theTop.
//    ref<int>       theTale.
//    
//    constructor
//    [
//        theArray := collections'List<type>(8).
//        theTop := 0.
//        theTale := 0.
//    ]
//    
//    constructor(int capacity)
//    [
//        theArray := collections'List<type>(capacity).
//        theTop := 0.
//        theTale := 0.
//    ]
//        
//    int rvprop length :: readLengthTo.
//    
//    readLengthTo(ref<int> aLength)
//    [
//        int n := T<int>(theTale) - T<int>(theTop).
//        
//        aLength value := n.
//    ]
//    
//    bool isEmpty
//        = theTop == theTale.
//    
//    bool isNotEmpty
//        = theTop != theTale.
//    
//    push(type anObject)
//    [
//        int aCapacity := theArray length.
//        
//        if (theTale < aCapacity)
//            [ theArray setAt(T<int>(theTale), anObject) ];
//            [ theArray append(anObject) ].
//            
//        theTale append(1).
//    ]
//    
//    type peek
//    [
//        if (theTale == theTop)
//            [ InvalidOperationException new:"Queue is empty"; raise ].
//            
//        ^ theArray getAt(T<int>(theTop))
//    ]        
//    
//    type pop
//    [
//        if (theTale == theTop)
//            [ InvalidOperationException new:"Queue is empty"; raise ].
//                        
//        type item := theArray getAt(T<int>(theTop)).
//                        
//        theTop append(1).
//        
//        // reuse the space if the queue is empty
//        if(theTale == theTop)
//            [
//                theTop  value := 0.
//                theTale value := 0.
//            ].        
//        
//        ^ item
//    ]
//
//    Enumerator<type> enumerator = BaseEnumerator<type>::
//    {
//        @theIndex := -1.
//        
//        @theLength := self length.
//        
//        type get = theArray getAt(@theIndex).
//        
//        enumerable = self.
//        
//        reset
//        [
//            @theIndex := -1
//        ]
//    }.    
//        
//    clear
//    [
//        theArray clear.
//        theTop value :=0.
//        theTale value:=0
//    ]    
//}

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
        
    get()
        = theList[theIndex];
        
    reset()
    {
        theList := theStack;
        theIndex := theStack.TopPointer
    }   
    
    enumerable() = theStack;
}

//// --- QueueEnumerator ---
//
//class QueueEnumerator :: Enumerator
//{
//    T<Queue>      theQueue.
//    T<Enumerator> theEnumerator.
//    
//    constructor new(Queue aQueue)
//    [
//        theQueue := aQueue.
//        theEnumerator := T<ArrayList>(aQueue) enumerator.
//    ]
//    
//    bool next 
//        = theEnumerator next.
//        
//    get
//        = theEnumerator get.        
//        
//    reset
//    [
//        theEnumerator reset.
//    ]        
//    
//    enumerable = theQueue.
//}