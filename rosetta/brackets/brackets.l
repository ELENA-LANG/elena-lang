// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

#define system.
#define system'routines.
#define extensions.

// --- RandomBrackets ---

#symbol randomBrackets = (:aLength)
[
    #var aBrackets := arrayControl new &length:aLength &each: i[ CharValue new:91 ] + arrayControl new &length:aLength &each: i[ CharValue new:93 ].
    
    randomControl randomize:(aLength * 2) &array:aBrackets.
    
    ^ Summing new:(String new) foreach:aBrackets literal.
].

#symbol isBalanced = (:aLiteral)
[
    #var aCounter := Integer new:0.

    control foreach:aLiteral &until: aChar [ aCounter append:(aChar => "[" ? [ 1 ] "]" ? [ -1 ]) < 0 ].

    ^ (0 == aCounter).
].

// --- Program ---

#symbol program =                                             
[
    #var anStr := randomBrackets:1.
    
    control from:0 &till:10 &do: aLength
    [
        #var anStr := randomBrackets:aLength.
        #var balanced := isBalanced:anStr.

        consoleEx writeLine:"""":anStr:"""":(balanced => true ? [ " is balanced" ] false ? [ " is not balanced" ]).
    ].

    console readChar.
].
