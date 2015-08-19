#import system.

#symbol(const,type:int)INFINITE = 0FFFFFFFFh.

// --- ThreadHandle ---

#class(struct:embeddable,sealed) ThreadHandle
{
    #field(type:handle) theHandle.
    #field(type:rt_ptr) theActionPtr.
    
    #constructor new &func:anAction
    [
        theActionPtr := RTPointer new:anAction.

        theHandle := system'external'KERNEL32 CreateThread
                        &int:0
                        &int:0
                        &symbol_ref:system'internal'win_thread_start
                        &int:theActionPtr
                        &int:4
                        &int:0.
    ]

    #method start
    [
        system'external'KERNEL32 ResumeThread
            &handle:theHandle.
    ]

    #method join
        <= %join &int:INFINITE.
                                                                   
    #method join &int:aTimeOut
    [
        system'external'KERNEL32 WaitForSingleObject
            &handle:theHandle
            &int:aTimeOut.
    ]     
                                                                   
    #method close
    [
        
    ]
}