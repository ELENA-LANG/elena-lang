// --- Thread ---

public sealed class Thread
{
    ThreadHandle theHandle;
    
    constructor assign(Func f)
    {
        theHandle := new ThreadHandle(f);
    }
    
    start() => theHandle;

    join() => theHandle;
            
    close() => theHandle;
}
