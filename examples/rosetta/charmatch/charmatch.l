import system'routines.
import extensions.

// --- Program ---

program =
[
    var s :="hello".
    
    (s startingWith:"hel") ?
    [
        console printLine(s," starts with hel").
    ].
    
    (s endingWith:"llo") ?
    [
        console printLine(s," ends with llo").
    ].

    (s containing:"hello") ?
    [
        console printLine(s," contains el").
    ].
    
    console readChar.
].