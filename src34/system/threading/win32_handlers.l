public const int INFINITE = 0FFFFFFFFh.

// --- ThreadHandle ---

public sealed struct ThreadHandle
{
    int    theHandle.
    rt_ptr theActionPtr.
    
    constructor new(Func f)
    [
        theActionPtr := RTPointer new:f.
        
        int index := theActionPtr index.

        int procPtr := 0.
        system'core_routines'__thread readThreadProc(&procPtr).

        theHandle := system'external'KERNEL32 CreateThread(
                        0, 
                        0, 
                        procPtr, 
                        index, 
                        4, 
                        0).
    ]

    start
    [
        system'external'KERNEL32 ResumeThread(theHandle).
    ]

    join
        <= join(INFINITE).
                                                                   
    join(int timeOut)
    [
        system'external'KERNEL32 WaitForSingleObject
            (theHandle, timeOut).
    ]     
                                                                   
    close
    [
        
    ]
}

public singleton threadControl
{
    sleep(int interval)
    [
        extern()
        [
            system'external'KERNEL32 Sleep(interval).
        ].
    ]
}


public extension threadOp
{
    enterCriticalSection
        => system'core_routines'__thread. 
        
    leaveCriticalSection
        => system'core_routines'__thread. 
} 