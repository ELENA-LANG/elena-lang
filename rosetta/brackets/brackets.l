// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

#define system.
#define extensions.

// --- RandomBrackets ---

#symbol randomBrackets = &&:aLength
[
    #var aBrackets := Array new &type'length:aLength &action: &&:i[ CharValue new:"[" ] + Array new &type'length:aLength &action: &&:i[ CharValue new:"]" ].
    
    randomControl randomize:(aLength * 2) &array:aBrackets.
    
    ^ Summing new:(String new) foreach:aBrackets Literal.
].

#symbol isBalanced = &&:aLiteral
[
    #var aCounter := Integer new:0.

    control foreach:aLiteral &until: &&:aChar [ aCounter append:(aChar => "[" ? [ 1 ] "]" ? [ -1 ]) < 0 ].

    ^ (0 == aCounter).
].

// --- Program ---

#symbol program =
[
    control from:0 &till:10 &do: &&:aLength
    [
        #var anStr := randomBrackets:aLength.
        #var balanced := isBalanced:anStr.

        console write:"""" write:anStr write:"""" writeLine:(balanced => true ? [ " is balanced" ] false ? [ " is not balanced" ]).
    ].

    console readChar.
].
