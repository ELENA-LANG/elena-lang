// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

#define system.
#define system'routines.
#define extensions.

// --- RandomBrackets ---

#symbol randomBrackets = (:aLength)
[   
    ^ (0 == aLength)
        ? [ emptyLiteralValue ]
        ! [
            #var aBrackets := 
                Array new &length:(aLength int) set &every: (&index:i) [ CharValue new &short:91 ] 
                + 
                Array new &length:(aLength int) set &every: (&index:i)[ CharValue new &short:93 ].
    
            aBrackets::randomOp randomize:(aLength * 2).
    
            ^ control run:aBrackets &forEach:(Summing new:(String new)) literal.
        ].
].

#symbol isBalanced = (:aLiteral)
[
    #var aCounter := Integer new:0.

    control run:aLiteral &seeking: aChar [ aCounter append:(aChar => "[" ? [ 1 ] "]" ? [ -1 ]) < 0 ].

    ^ (0 == aCounter).
].

// --- Program ---

#symbol program =                                             
[
    control run &int:0 &int:9 &forEvery: (&index:aLength)
    [
        #var anStr := randomBrackets:aLength.
        #var balanced := isBalanced:anStr.

        consoleEx writeLine:"""":anStr:"""":(balanced => true ? [ " is balanced" ] false ? [ " is not balanced" ]).
    ].

    console readChar.
].
