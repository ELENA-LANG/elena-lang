import system'routines.
import extensions.
import extensions'routines.

const int M = 3.
const int N = 5. 

// --- Numbers ---

numbers = (:anN)
[
    ^ Array new(anN); populate(:n)<int>( n )
].

// --- Program ---

public program =
[
    var aNumbers := numbers(N).    
    Combinator new:M of:aNumbers; forEach(:aRow)
    [
        console printLine(aRow toLiteral)
    ].
    
    console readChar.
].