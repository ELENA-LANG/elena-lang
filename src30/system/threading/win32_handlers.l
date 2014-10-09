#define system.
#define system'io.

#symbol(const)INFINITE = 0FFFFFFFFh.

// --- ThreadStarter ---

#symbol $threadStarter = <thread_start>.

// --- ThreadHandle ---

#class ThreadHandle : Handle
{
    #constructor new &action:anAction
    [
        #var(type:int)param.

        referenceTableControl allocate:anAction &out'int:param.
                        
        theData := system'external'KERNEL32 CreateThread
                        &int:0
                        &int:0
                        &intptr: #symbol $threadStarter
                        &int:param
                        &int:4
                        &int:0.
    ]

    #method start
    [
        system'external'KERNEL32 ResumeThread
            &handle:$self.
    ]

    #method join
        <= %join &int:INFINITE.
                                                                   
    #method join &int:aTimeOut
    [
        system'external'KERNEL32 WaitForSingleObject
            &handle:$self
            &int:aTimeOut.
    ]     
                                                                   
    #method close
    [
    ]
}