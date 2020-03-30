public const int INFINITE = 0FFFFFFFFh;

// --- ThreadHandle ---

public sealed struct ThreadHandle
{
    int    theHandle;
    rt_ptr theActionPtr;
    
    constructor(Func f)
    {
        theActionPtr := RTPointer.assign(f);
        
        int index := theActionPtr.Index;

        int procPtr := 0;
        intern system'core_routines'__thread.readThreadProc(ref procPtr);

        theHandle := extern KERNEL32.CreateThread(
                        0, 
                        0, 
                        procPtr, 
                        index, 
                        4, 
                        0)
    }

    start()
    {
        extern KERNEL32.ResumeThread(theHandle)
    }

    join()
        <= join(INFINITE);
                                                                   
    join(int timeOut)
    {
        extern KERNEL32.WaitForSingleObject
            (theHandle, timeOut)
    }     
                                                                   
    close()
    {
        extern KERNEL32.CloseHandle(theHandle)
    }
}

public singleton threadControl
{
    sleep(int interval)
    {
        extern
        {
            extern KERNEL32.Sleep(interval)
        }
    }
}

public extension threadOp
{
    enterCriticalSection()
        => intern system'core_routines'__thread;
        
    leaveCriticalSection()
        => intern system'core_routines'__thread; 
} 