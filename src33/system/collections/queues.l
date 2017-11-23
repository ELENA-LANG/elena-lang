// === types ===
type stack = system'collections'Stack.
type queue = system'collections'Queue.

// --- Stack ---

sealed class Stack :: BaseEnumerable
{
    array_list theList.
    vint       theTop.
    
    constructor new
    [
        theList := ArrayList new.
        theTop := 0.
    ]
    
    int $top_pointer = theTop.
    
    array_list = theList.
    
    enumerator = StackEnumerator new stack:$self.
    
    int rvprop length :: readLengthTo.
    
    stacksafe readLengthTo vint:aLength
    [
        aLength int := theTop.
    ]
    
    bool isEmpty
        = 0 == theTop.
    
    bool isNotEmpty
        = 0 != theTop.
    
    push : anObject
    [
        int aCapacity := theList length.
        
        if (theTop < aCapacity)
            [ theList setAt int:theTop object:anObject ];
            [ theList append:anObject ].
            
        theTop append int:1
    ]
    
    peek
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        ^ theList getAt int:(theTop - 1)
    ]        
    
    pop
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        theTop reduce int:1.
        
        var item := theList getAt int:theTop.                        
        theList setAt int:theTop object:$nil.
        
        ^ item
    ]
    
    clear
    [
        theList clear.
        theTop int := 0.
    ]
}

// --- Stack Template ---

template<BaseEnumerable> stack :: type
{
    list<type> theArray.
    vint        theTop.
    
    explicit
    [
        theArray := list<type>(8).
        theTop := 0.
    ]
    
    explicit int:capacity
    [
        theArray := list<type>(capacity).
        theTop := 0.
    ]
        
    int rvprop length :: readLengthTo.
    
    stacksafe readLengthTo vint:aLength
    [
        aLength int := theTop.
    ]
    
    bool isEmpty
        = 0 == theTop.
    
    bool isNotEmpty
        = 0 != theTop.
    
    push type:anObject
    [
        int aCapacity := theArray length.
        
        if (theTop < aCapacity)
            [ theArray setAt int:theTop type:anObject ];
            [ theArray append type:anObject ].
            
        theTop append int:1
    ]
    
    type peek
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        ^ theArray getAt int(theTop - 1)
    ]        
    
    type pop
    [
        if (0 == theTop)
            [ InvalidOperationException new:"Stack is empty"; raise ].
            
        theTop reduce int:1.
        
        type item := theArray getAt int:theTop.                        
        //theArray setAt int:theTop type:$nil.
        
        ^ item
    ]
    
    clear
    [
        theArray clear.
        theTop int := 0.
    ]    
}

// --- Queue ---

sealed class Queue :: BaseEnumerable
{
    array_list theList.
    vint       theTop.
    vint       theTale.
    
    constructor new
    [
        theList := ArrayList new.
        theTop := 0.
        theTale := 0.
    ]
    
    array_list = theList.
    
    enumerator = QueueEnumerator new queue:$self.
    
    int rvprop length :: readLengthTo.
    
    push : anObject
    [
        int aCapacity := theList length.
        
        if (theTale < aCapacity)
            [ theList setAt int:theTale object:anObject ];
            [ theList append:anObject ].
            
        theTale append int:1.
    ]
    
    peek
    [
        if (theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].
            
        ^ theList getAt int:theTop
    ]
    
    pop
    [
        if(theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].

        var item := theList getAt int:theTop.
                                    
        theList setAt int:theTop object:$nil.
        theTop append int:1.
        
        // reuse the space if the queue is empty
        if(theTale == theTop)
            [
                theTop  int := 0.
                theTale int := 0.
            ].
            
        ^ item
    ]
    
    stacksafe readLengthTo vint:aRetVal
    [
        int n := theTale - theTop.
        
        aRetVal int := n.
    ]
    
    clear
    [
        theList clear.
        theTop int :=0.
        theTale int:=0
    ]
    
    shift : offset
    [
        int i := theTop.
        int n := theTale - 1.
        
        var topItem := theList getAt int:i.
        
        while (i < n)
        [
            theList setAt int:i object:(theList getAt int:(i + 1)).
            
            i := i + 1
        ].
        
        theList setAt int:i object:topItem
    ]
}

// --- Queue Template ---

template<BaseEnumerable> queue :: type
{
    list<type> theArray.
    vint       theTop.
    vint       theTale.
    
    explicit
    [
        theArray := list<type>(8).
        theTop := 0.
        theTale := 0.
    ]
    
    explicit int:capacity
    [
        theArray := list<type>(capacity).
        theTop := 0.
        theTale := 0.
    ]
        
    int rvprop length :: readLengthTo.
    
    stacksafe readLengthTo vint:aLength
    [
        int n := theTale - theTop.
        
        aLength int := n.
    ]
    
    bool isEmpty
        = theTop == theTale.
    
    bool isNotEmpty
        = theTop != theTale.
    
    push type:anObject
    [
        int aCapacity := theArray length.
        
        if (theTale < aCapacity)
            [ theArray setAt int:theTale type:anObject ];
            [ theArray append type:anObject ].
            
        theTale append int:1.
    ]
    
    type peek
    [
        if (theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].
            
        ^ theArray getAt int:theTop
    ]        
    
    type pop
    [
        if (theTale == theTop)
            [ InvalidOperationException new:"Queue is empty"; raise ].
                        
        type item := theArray getAt int:theTop.                        
                        
        theArray setAt int:theTop type:$nil.                                    
        theTop append int:1.
        
        // reuse the space if the queue is empty
        if(theTale == theTop)
            [
                theTop  int := 0.
                theTale int := 0.
            ].        
        
        ^ item
    ]
    
    clear
    [
        theArray clear.
        theTop int :=0.
        theTale int:=0
    ]    
}

// --- StackEnumerator ---

class StackEnumerator :: Enumerator
{
    stack      theStack.
    array_list theList.
    int        theIndex. 
    
    constructor new stack:aStack
    [
        theStack := aStack.
        theList := aStack array_list.
        theIndex := aStack $top_pointer.
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
        theList := theStack array_list.
        theIndex := theStack $top_pointer.
    ]        
}

// --- QueueEnumerator ---

class QueueEnumerator :: Enumerator
{
    queue      theQueue.
    enumerator theEnumerator.
    
    constructor new queue:aQueue
    [
        theQueue := aQueue.
        theEnumerator := aQueue array_list; enumerator.
    ]
    
    bool next 
        = theEnumerator next.
        
    get
        = theEnumerator get.        
        
    reset
    [
        theEnumerator reset.
    ]        
}