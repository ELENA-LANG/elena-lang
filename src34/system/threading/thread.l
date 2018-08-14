// --- Thread ---

public class Thread
{
    T<ThreadHandle> theHandle.
    
    constructor new(Func f)
    [
        theHandle := ThreadHandle new(f).
    ]
    
    start => theHandle.

    join => theHandle.
            
    close => theHandle.
}
