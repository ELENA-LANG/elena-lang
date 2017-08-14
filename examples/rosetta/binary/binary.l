/// showing the sequence of binary digits for given numbers

import system'routines.
import extensions.

// --- Program ---

program =
[
    (5,50,9000) forEach(:n)
    [
        console printLine(n toLiteral base:2).
    ].
].