#import system.
#import system'io.
#import system'collections.

#symbol $loadBytes = aProgram 
[
    #var(type:vbyte) byte := 0.
    #var br := BinaryFileReader new &path:aProgram.   
    #var bta := List new.
    #loop (br available) ?
    [
        br read &vbyte:byte.
        bta += byte.
    ].
    br close.  
    ^bta. 
].

#symbol program =
[      
    #var cpu := cpuvm'Cpu new.
    cpu loadProgram : ($loadBytes:"test.bin").
    cpu emulate.
].