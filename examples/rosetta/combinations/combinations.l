import system'routines.
import extensions.
import extensions'routines.

const int M = 3.
const int N = 5. 

// --- Numbers ---

numbers = (:anN)
[
    ^ Array new length:anN; set every: (&int:n)[ ^n ]
].

// --- Program ---

program =
[
    var aNumbers := numbers eval:N.    
    Combinator new:M of:aNumbers; run each: (:aRow)
    [
        console writeLine:aRow
    ].
    
    console readChar.
].