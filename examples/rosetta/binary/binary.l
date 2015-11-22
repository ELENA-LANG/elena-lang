#import system.
#import system'routines.
#import extensions.

// --- Program ---

#symbol program =
[
    (5,50,9000) run &each: n
    [
        console writeLine:(n toLiteral &base:2).
    ].
].