#import system'routines.
#import extensions.

// --- Program ---

program =
[
    var s :="hello".
    
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
    
    console readChar.
].