// --- Thread ---

#class Thread
{
    #field theHandle.
    
    #constructor new &action:anAction
    [
        theHandle := ThreadHandle new &action:anAction.
    ]
    
    #method start
    [
        theHandle start.
    ]

    #method join
    [
        theHandle join.
    ]
            
    #method close
    [
        theHandle close.
    ]
}

// --- threadControl ---

#symbol threadControl =
{
    start : anAction
    [
        #var aThread := Thread new &action::anAction.
        
        aThread start.
        
        ^ aThread.
    ]
}.