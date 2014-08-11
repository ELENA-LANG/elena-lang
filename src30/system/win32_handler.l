// --- Handler ---

#type(size:4)handle.

#class(type:handle) Handle
{
    #field(type:int)theData.

//    #method close
//    [
//        system'external'KERNEL32 CloseHandle
//            &handle:$self.
//    ]
}
