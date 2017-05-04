const int INFINITE = 0FFFFFFFFh.

// --- ThreadHandle ---

sealed embeddable struct ThreadHandle
{
    int    theHandle.
    rt_ptr theActionPtr.
    
    constructor new &func:anAction
    [
        theActionPtr := RTPointer new:anAction.

        theHandle := system'external'KERNEL32 CreateThread
                        &int:0
                        &int:0
                        &symbol_ref:system'core_routines'win_thread_start
                        &int:theActionPtr
                        &int:4
                        &int:0.
    ]

    start
    [
        system'external'KERNEL32 ResumeThread
            &int:theHandle.
    ]

    join
        <= join &int:INFINITE.
                                                                   
    join &int:aTimeOut
    [
        system'external'KERNEL32 WaitForSingleObject
            &int:theHandle
            &int:aTimeOut.
    ]     
                                                                   
    close
    [
        
    ]
}

class thread =
{
    sleep : interval
        <= sleep &int:interval.
    
    sleep &int:interval
    [
        #extern
        [
            system'external'KERNEL32 Sleep &int:interval.
        ].
    ]
}.

extension threadOp
{
    enterCriticalSection
        => system'core_routines'__thread. 
        
    leaveCriticalSection
        => system'core_routines'__thread. 
} 