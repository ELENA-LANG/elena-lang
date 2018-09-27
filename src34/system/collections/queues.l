// --- Stack ---

public sealed class Stack :: Enumerable
{
    T<ArrayList> theList.
    ref<int>     theTop.
    
    constructor new
    [
        theList := ArrayList new.
        theTop := 0.
    ]
    
    int internal top_pointer = theTop.
    
    cast T<ArrayList> = theList.
    
    T<Enumerator> enumerator = StackEnumerator new(self).
    
    int rvprop length :: readLengthTo.
    
    stacksafe readLengthTo(ref<int> aLength)
    [
        theTop readValueTo(aLength).
    ]
    
    bool isEmpty
        = 0 == theTop.
    
    bool isNotEmpty
        = 0 != theTop.
    
    push : anObject
    [
        int aCapacity := theList length.
        
        if (theTop < aCapacity)
            [ theList setAt(T<int>(theTop), anObject) ];
            [ theList append:anObject ].
            
        theTop append(1)
    ]
    
    peek
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        ^ theList getAt(T<int>(theTop) - 1)
    ]        
    
    pop
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        theTop reduce(1).
        
        var item := theList getAt(T<int>(theTop)).
        theList setAt(T<int>(theTop), nil).
        
        ^ item
    ]
    
    clear
    [
        theList clear.
        theTop value := 0.
    ]
}

// --- Stack Template ---

public sealed Stack<type> :: Enumerable<type>
{
    collections'List<type> theArray.
    ref<int>   theTop.
    
    constructor
    [
        theArray := collections'List<type>(8).
        theTop := 0.
    ]
    
    constructor(int capacity)
    [
        theArray := collections'List<type>(capacity).
        theTop := 0.
    ]
        
    int rvprop length :: readLengthTo.
    
    stacksafe readLengthTo(ref<int> aLength)
    [
        theTop readValueTo(aLength).
    ]
    
    bool isEmpty
        = 0 == theTop.
    
    bool isNotEmpty
        = 0 != theTop.
    
    push(type anObject)
    [
        int aCapacity := theArray length.
        
        if (theTop < aCapacity)
            [ theArray setAt(T<int>(theTop), anObject) ];
            [ theArray append(anObject) ].
            
        theTop append(1)
    ]
    
    type peek
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        ^ theArray getAt(T<int>(theTop) - 1)
    ]        
    
    type pop
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        theTop reduce(1).
        
        type item := theArray getAt(T<int>(theTop)).
        //theArray setAt int:theTop type:$nil.
        
        ^ item
    ]
    
    clear
    [
        theArray clear.
        theTop value := 0.
    ]    
    
    Enumerator<type> enumerator = BaseEnumerator<type>::
    {
        @theIndex := -1.
        
        @theLength := self length.
        
        type get = theArray getAt(T<int>(theTop) - @theIndex - 1).
        
        enumerable = self.
        
        reset
        [
            @theIndex := -1
        ]
    }.    
}

// --- Queue ---

public sealed class Queue :: Enumerable
{
    T<ArrayList> theList.
    ref<int>     theTop.
    ref<int>     theTale.
    
    constructor new
    [
        theList := ArrayList new.
        theTop := 0.
        theTale := 0.
    ]
    
    cast T<ArrayList> = theList.
    
    T<Enumerator> enumerator = QueueEnumerator new(self).
    
    int rvprop length :: readLengthTo.
    
    push : anObject
    [
        int aCapacity := theList length.
        
        if (theTale < aCapacity)
            [ theList setAt(theTale, anObject) ];
            [ theList append:anObject ].
            
        theTale append(1).
    ]
    
    peek
    [
        if (theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].
            
        ^ theList getAt(T<int>(theTop))
    ]
    
    pop
    [
        if(theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].

        var item := theList getAt(T<int>(theTop)).
                                    
        theTop append(1).
        
        // reuse the space if the queue is empty
        if(theTale == theTop)
            [
                theTop  value := 0.
                theTale value := 0.
            ].
            
        ^ item
    ]
    
    stacksafe readLengthTo(ref<int> aRetVal)
    [
        int n := T<int>(theTale) - T<int>(theTop).
        
        aRetVal value := n.
    ]
    
    clear
    [
        theList clear.
        theTop value :=0.
        theTale value:=0
    ]
    
    shift : offset
    [
        int i := theTop.
        int n := theTale - 1.
        
        var topItem := theList getAt(i).
        
        while (i < n)
        [
            theList setAt(i, theList getAt(i + 1)).
            
            i := i + 1
        ].
        
        theList setAt(i, topItem)
    ]
}

// --- Queue Template ---

public sealed Queue<type> :: Enumerable<type>
{
    collections'List<type> theArray.
    ref<int>       theTop.
    ref<int>       theTale.
    
    constructor
    [
        theArray := collections'List<type>(8).
        theTop := 0.
        theTale := 0.
    ]
    
    constructor(int capacity)
    [
        theArray := collections'List<type>(capacity).
        theTop := 0.
        theTale := 0.
    ]
        
    int rvprop length :: readLengthTo.
    
    stacksafe readLengthTo(ref<int> aLength)
    [
        int n := T<int>(theTale) - T<int>(theTop).
        
        aLength value := n.
    ]
    
    bool isEmpty
        = theTop == theTale.
    
    bool isNotEmpty
        = theTop != theTale.
    
    push(type anObject)
    [
        int aCapacity := theArray length.
        
        if (theTale < aCapacity)
            [ theArray setAt(T<int>(theTale), anObject) ];
            [ theArray append(anObject) ].
            
        theTale append(1).
    ]
    
    type peek
    [
        if (theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].
            
        ^ theArray getAt(T<int>(theTop))
    ]        
    
    type pop
    [
        if (theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].
                        
        type item := theArray getAt(T<int>(theTop)).
                        
        theTop append(1).
        
        // reuse the space if the queue is empty
        if(theTale == theTop)
            [
                theTop  value := 0.
                theTale value := 0.
            ].        
        
        ^ item
    ]

    Enumerator<type> enumerator = BaseEnumerator<type>::
    {
        @theIndex := -1.
        
        @theLength := self length.
        
        type get = theArray getAt(@theIndex).
        
        enumerable = self.
        
        reset
        [
            @theIndex := -1
        ]
    }.    
        
    clear
    [
        theArray clear.
        theTop value :=0.
        theTale value:=0
    ]    
}

// --- StackEnumerator ---

public class StackEnumerator :: Enumerator
{
    T<Stack>     theStack.
    T<ArrayList> theList.
    int          theIndex. 
    
    constructor new(Stack aStack)
    [
        theStack := aStack.
        theList := aStack .
        theIndex := aStack top_pointer.
    ]
    
    bool next 
    [
        if (theIndex > 0)
        [
            theIndex -= 1.
            
            ^ true
        ];
        [
            ^ false
        ]
    ]
        
    get
        = theList[theIndex].
        
    reset
    [
        theList := theStack.
        theIndex := theStack top_pointer.
    ]   
    
    enumerable = theStack.     
}

// --- QueueEnumerator ---

class QueueEnumerator :: Enumerator
{
    T<Queue>      theQueue.
    T<Enumerator> theEnumerator.
    
    constructor new(Queue aQueue)
    [
        theQueue := aQueue.
        theEnumerator := T<ArrayList>(aQueue) enumerator.
    ]
    
    bool next 
        = theEnumerator next.
        
    get
        = theEnumerator get.        
        
    reset
    [
        theEnumerator reset.
    ]        
    
    enumerable = theQueue.
}