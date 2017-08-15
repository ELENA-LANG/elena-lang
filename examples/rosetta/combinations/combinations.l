import system'routines.
import extensions.
import extensions'routines.

const int M = 3.
const int N = 5. 

// --- Numbers ---

numbers = (:anN)
[
    ^ Array new length:anN; populate(:n)( n )
].

// --- Program ---

program =
[
    var aNumbers := numbers eval:N.    
    Combinator new:M of:aNumbers; forEach(:aRow)
    [
        console printLine:aRow
    ].
    
    console readChar.
].