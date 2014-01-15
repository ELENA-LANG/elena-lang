// --- Handler ---

#class Handle
{
    #field(type:int)theData.
    
    #method close
    [
        system'external'KERNEL32 CloseHandle
            &handle:$self.
    ]

    #method <= &handle:$self.
}