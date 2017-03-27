#subject thread = Thread.

// --- Thread ---

class Thread
{
    object theHandle.
    
    constructor new &func:anAction
    [
        theHandle := ThreadHandle new &func:anAction.
    ]
    
    start
    [
        theHandle start.
    ]

    join
    [
        theHandle join.
    ]
            
    close
    [
        theHandle close.
    ]
}
