#define system.

#symbol Program =
[
       #var(type:int)aVariable.
       #var(type:int)aLibHandle := system'external'libdl dlopen 
                                          &literal:"libc.so.6"
                                          &int:1.

       console writeLine:aLibHandle.

       aVariable := system'external'libdl dlsym
                        &int:aLibHandle  
                        &literal:"stdout".

       console writeLine:aVariable.
].