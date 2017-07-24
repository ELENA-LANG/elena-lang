// === types ===
type stack :: system'collections'Stack.
type queue :: system'collections'Queue.

// --- Stack

sealed class Stack :: dispatchable(enumerable)
{
    array_list theList.
    vint       theTop.
    
    constructor new
    [
        theList := ArrayList new.
        theTop := 0.
    ]
    
    array_list = theList.
    
    enumerator = StackEnumerator new stack:$self.
    
    int length :: outnprop(readLengthTo).
    
    stacksafe readLengthTo vint:aLength
    [
        aLength int := theTop.
    ]
    
    bool is empty
        = 0 == theTop.
    
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

// --- Queue ---

sealed class Queue :: dispatchable(enumerable)
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
    
    int length :: outnprop(readLengthTo).
    
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

// --- StackEnumerator ---

class StackEnumerator :: Enumerator
{
    stack      theStack.
    enumerator theEnumerator.
    
    constructor new stack:aStack
    [
        theStack := aStack.
        theEnumerator := aStack array_list; enumerator.
    ]
    
    next 
        = theEnumerator next.
        
    get
        = theEnumerator get.
        
    reset
    [
        theEnumerator reset.
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
    
    next 
        = theEnumerator next.
        
    get
        = theEnumerator get.        
        
    reset
    [
        theEnumerator reset.
    ]        
}