#import system'routines.
#import extensions.
#import extensions'routines.

const M = 3.
const N = 5. 

// --- Numbers ---

numbers = (:anN)
[
    Array new &length:(anN int) set &every: (&index:n) [ Integer new &int:n ]
].

// --- Program ---

program =
[
    var aNumbers := numbers eval:N.    
    Combinator new:M &of:aNumbers run &each: aRow
    [
        console writeLine:aRow.
    ].
    
    console readChar.
].