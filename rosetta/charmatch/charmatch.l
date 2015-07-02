#define system.
#define system'routines.
#define extensions.

// --- Program ---

#symbol program =
[
    #var s :="hello".
    
    (s startingWith:"hel") ?
    [
        console writeLine:s:" starts with hel".
    ].
    
    (s endingWith:"llo") ?
    [
        console writeLine:s:" ends with llo".
    ].

    (s containing:"hello") ?
    [
        console writeLine:s:" contains el".
    ].
].