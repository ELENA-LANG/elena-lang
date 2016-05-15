#import system.

// === types ===
#subject stack = system'collections'Stack.
#subject queue = system'collections'Queue.

// --- Stack

#class(sealed,dispatchable:enumerable) Stack
{
    #field(array_list)theList.
    #field(vint)theTop.
    
    #constructor(suppress:w3) new
    [
        theList := ArrayList new.
        theTop := 0.
    ]
    
    #method array_list = theList.
    
    #method enumerator = StackEnumerator new &stack:$self.
    
    #method(stacksafe,outnprop:length) readLength &vint:aLength
    [
        aLength := theTop.
    ]
    
    #method push : anObject
    [
        #var(int)aCapacity := theList length.
        
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
        
        #var item := theList getAt &int:theTop.                        
        theList setAt &int:theTop &object:$nil.
        
        ^ item.
    ]
    
    #method clear
    [
        theList clear.
        theTop set &int:0.
    ]
}

// --- Queue ---

#class(sealed,dispatchable:enumerable) Queue
{
    #field(array_list)theList.
    #field(vint)theTop.
    #field(vint)theTale.
    
    #constructor(suppress:w3) new
    [
        theList := ArrayList new.
        theTop := 0.
        theTale := 0.
    ]
    
    #method array_list = theList.
    
    #method enumerator = QueueEnumerator new &queue:$self.
    
    #method push : anObject
    [
        #var(int)aCapacity := theList length.
        
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

        #var item := theList getAt &int:theTop.
                                    
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
    
    #method(stacksafe,outnprop:length) readLength &vint:aRetVal
    [
        #var(int)n := theTale - theTop.
        
        aRetVal := n.
    ]
    
    #method clear
    [
        theList clear.
        theTop set &int:0.
        theTale set &int:0.
    ]
}

// --- StackEnumerator ---

#class StackEnumerator :: Enumerator
{
    #field(stack)theStack.
    #field(enumerator)theEnumerator.
    
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
    #field(queue)theQueue.
    #field(enumerator)theEnumerator.
    
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