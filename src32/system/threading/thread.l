// --- Thread ---

class Thread
{
    object theHandle.
    
    constructor new:anAction
    [
        theHandle := ThreadHandle new:anAction.
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
