// ==== ver 3.4.1 ===

#import system.

// --- Thread ---

#class Thread
{
    #field theHandle.
    
    #constructor new &func:anAction
    [
        theHandle := ThreadHandle new &func:anAction.
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
