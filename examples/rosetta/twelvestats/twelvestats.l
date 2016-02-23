#import system.
#import system'routines.
#import extensions.

#class(extension)op
{
    #method toBitArray
        = self select &each: b[ b iif:"T ":"F " ] summarize:(String new).
}

#symbol puzzle =
(
    bits [ bits length == 12 ],
    
    bits [ bits last:6 select &each:x [ x iif:1:0 ] summarize == 3 ],
    
    bits [ bits select &each:(:i:x) [ (i is &even)and:x iif:1:0 summarize == 2 ]],
    
    bits [ bits@4 iif:(bits@5 && bits@6):true ],
    
    bits [ bits filter &pattern:(:a:b:c) [ (a && b && c) not ] count > 0 ],
    
    bits [ bits select &each:(:i:x) [ (i is &odd)and:x iif:1:0 summarize == 4 ]],
    
    bits [ (bits@1) xor:(bits@2) ],
    
    bits [ bits@6 iif:(bits@5 && bits@4):true ],
    
    bits [ bits top:6 select &each:x [ x iif:1:0 ] summarize == 3 ],
    
    bits [ bits filter &pattern:(:a:b) [ a && b ] count > 0 ],
    
    bits [ (bits@6) xor:(bits@7) xor:(bits@78) ],
    
    bits [ bits filter &pattern:(:a:b:c:d) [ a && b && c && d ] count > 0 ]
).

#symbol program =
[
    0 till:(2 power &int:12) &doEach:n
    [
        #var bits := BitArray32 new:n.
        
        #var counts := puzzle select &each:puzzle [ puzzle eval:bits iif:1:0 ] summarize.
        
        counts =>
            12 ? [ console writeLine:"Total hit":(bits toBitArray). ]
            11 ? [ console writeLine:"Near miss":(bits toBitArray). ]
            0 ? [ console writeLine:"Total miss":(bits toBitArray). ].
    ].
    
    console readChar.
].