#import system.

// === types ===
#subject stack = system'collections'Stack.
#subject queue = system'collections'Queue.

// --- Stack

sealed dispatchable(enumerable) class Stack
{
    array_list theList.
    vint       theTop.
    
    constructor new
    [
        theList := ArrayList new.
        theTop := 0.
    ]
    
    array_list = theList.
    
    enumerator = StackEnumerator new &stack:$self.
    
    stacksafe outnprop(length) readLength &vint:aLength
    [
        aLength := theTop.
    ]
    
    bool is &empty
        = 0 == theTop.
    
    push : anObject
    [
        int aCapacity := theList length.
        
        (theTop < aCapacity)
            ? [ theList setAt &int:theTop &object:anObject. ]
            ! [ theList append:anObject. ].
            
        theTop += 1.
    ]
    
    peek
    [
        (0 == theTop)
            ? [ InvalidOperationException new &message:"Stack is empty" raise. ].
            
        ^ theList getAt &int:(theTop - 1).
    ]        
    
    pop
    [
        (0 == theTop)
            ? [ InvalidOperationException new &message:"Stack is empty" raise. ].
            
        theTop -= 1.
        
        var item := theList getAt &int:theTop.                        
        theList setAt &int:theTop &object:$nil.
        
        ^ item.
    ]
    
    clear
    [
        theList clear.
        theTop set &int:0.
    ]
}

// --- Queue ---

sealed dispatchable(enumerable) class Queue
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
    
    enumerator = QueueEnumerator new &queue:$self.
    
    push : anObject
    [
        int aCapacity := theList length.
        
        (theTale < aCapacity)
            ? [ theList setAt &int:theTale &object:anObject. ]
            ! [ theList append:anObject. ].
            
        theTale += 1.
    ]
    
    peek
    [
        (theTale == theTop)
            ? [ InvalidOperationException new &message:"Queue is empty" raise. ].
            
        ^ theList getAt &int:theTop.  
    ]
    
    pop
    [
        (theTale == theTop)
            ? [ InvalidOperationException new &message:"Queue is empty" raise. ].

        var item := theList getAt &int:theTop.
                                    
        theList setAt &int:theTop &object:$nil.
        theTop += 1.
        
        // reuse the space if the queue is empty
        (theTale == theTop)
            ? [
                theTop set &int:0.
                theTale set &int:0.
            ].
            
        ^ item.            
    ]
    
    stacksafe outnprop(length) readLength &vint:aRetVal
    [
        int n := theTale - theTop.
        
        aRetVal := n.
    ]
    
    clear
    [
        theList clear.
        theTop set &int:0.
        theTale set &int:0.
    ]
    
    shift : offset
    [
        int i := theTop.
        int n := theTale - 1.
        
        var topItem := theList getAt &int:i.
        
        while (i < n)
        [
            theList setAt &int:i &object:(theList getAt &int:(i + 1)).
            
            i += 1.
        ].
        
        theList setAt &int:i &object:topItem.
    ]
}

// --- StackEnumerator ---

class StackEnumerator :: Enumerator
{
    stack      theStack.
    enumerator theEnumerator.
    
    constructor new &stack:aStack
    [
        theStack := aStack.
        theEnumerator := aStack array_list enumerator.
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
    
    constructor new &queue:aQueue
    [
        theQueue := aQueue.
        theEnumerator := aQueue array_list enumerator.
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