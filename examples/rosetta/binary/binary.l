/// showing the sequence of binary digits for given numbers

import system'routines.
import extensions.

// --- Program ---

public program =
[
    (5,50,9000) forEach(:n)
    [
        console printLine(n toLiteral(2)).
    ].
].