#import system.

// === types ===
#subject(class:system'collections'Stack)stack.
#subject(class:system'collections'Queue)queue.

// --- Stack

#class(sealed) Stack
{
    #field(type:array_list)theList.
    #field(type:vint)theTop.
    
    #constructor new
    [
        theList := ArrayList new.
        theTop := Integer new &int:0.
    ]
    
    #method array_list = theList.
    
    #method enumerator = StackEnumerator new &stack:$self.
    
    #method length = theTop int.
    
    #method(stacksafe) readLength &vint:aLength
    [
        aLength << theTop.
    ]
    
    #method push : anObject
    [
        #var(type:int)aCapacity.
        theList readLength &vint:aCapacity.
        
        (theTop < aCapacity)
            ? [ theList setAt &int:theTop &object:anObject. ]
            ! [ theList append:anObject. ].
            
        theTop += 1.
    ]
    
    #method peek
        = theList getAt &int:(theTop - 1).
    
    #method pop
    [
        (0 == theTop)
            ? [ #throw InvalidOperationException new &message:"Stack is empty". ].
            
        theTop -= 1.
        theList setAt &int:theTop &object:$nil.
    ]
}

// --- Queue ---

#class(sealed) Queue
{
    #field(type:array_list)theList.
    #field(type:vint)theTop.
    #field(type:vint)theTale.
    
    #constructor new
    [
        theList := ArrayList new.
        theTop := Integer new &int:0.
        theTale := Integer new &int:0.
    ]
    
    #method array_list = theList.
    
    #method enumerator = QueueEnumerator new &queue:$self.
    
    #method push : anObject
    [
        #var(type:int)aCapacity.
        theList readLength &vint:aCapacity.
        
        (theTale < aCapacity)
            ? [ theList setAt &int:theTale &object:anObject. ]
            ! [ theList append:anObject. ].
            
        theTale += 1.
    ]
    
    #method peek
        = theList getAt &int:(theTop - 1).
    
    #method pop
    [
        (theTale == theTop)
            ? [ #throw InvalidOperationException new &message:"Stack is empty". ].
            
        theList setAt &int:theTop &object:$nil.
        theTop += 1.
        
        // reuse the space if the queue is empty
        (theTale == theTop)
            ? [
                theTop << 0.
                theTale << 0.
            ].
    ]
    
    #method(stacksafe) readLength &vint:aRetVal
    [
        #var(type:int)n := theTale - theTop.
        
        aRetVal << n.
    ]
    
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
}

// --- StackEnumerator ---

#class StackEnumerator :: Enumerator
{
    #field(type:stack)theStack.
    #field(type:enumerator)theEnumerator.
    
    #constructor new &stack:aStack
    [
        theStack := aStack.
        theEnumerator := aStack array_list enumerator.
    ]
    
    #method next 
        = theEnumerator next.
        
    #method get
        = theEnumerator get.
        
    #method reset
    [
        theEnumerator reset.
    ]        
}

// --- QueueEnumerator ---

#class QueueEnumerator :: Enumerator
{
    #field(type:queue)theQueue.
    #field(type:enumerator)theEnumerator.
    
    #constructor new &queue:aQueue
    [
        theQueue := aQueue.
        theEnumerator := aQueue array_list enumerator.
    ]
    
    #method next 
        = theEnumerator next.
        
    #method get
        = theEnumerator get.        
        
    #method reset
    [
        theEnumerator reset.
    ]        
}