// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

import system'routines.
import extensions.

// --- RandomBrackets ---

randomBrackets =
{
    new : aLength
    [
        if (0 == aLength)
            [ ^emptyLiteralValue ];
            [
                var aBrackets := 
                    Array new length:(aLength int); set every: (&int:i) [ ^$91 ] 
                    + 
                    Array new length:(aLength int); set every: (&int:i)[ ^$93 ].
        
                aBrackets := aBrackets randomize:(aLength * 2).

                ^ aBrackets summarize:(String new); literal
            ]
    ]
}.

extension op
{
    isBalanced
    [
        var aCounter := Integer new:0.
    
        self seek each: (:aChar)[ ^(aCounter append:(aChar => $91 [ 1 ]; $93 [ -1 ])) < 0 ].
    
        ^ (0 == aCounter)
    ]
}

// --- Program ---

program =                                             
[
    0 to:9 doEach: (:aLength)
    [
        var anStr := randomBrackets new:aLength.

        console printLine:"""":anStr:"""":(anStr isBalanced; iif:" is balanced":" is not balanced")
    ].

    console readChar
].
