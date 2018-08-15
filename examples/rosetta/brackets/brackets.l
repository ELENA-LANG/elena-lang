// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

import system'routines.
import extensions.
import extensions'text.

// --- RandomBrackets ---

randomBrackets(len)
[
    if (0 == len)
        [ ^emptyLiteral ];
        [
            var aBrackets := 
                Array new(len); populate(:i)($91) 
                + 
                Array new(len); populate(:i)($93).
    
            aBrackets := aBrackets randomize(len * 2).

            ^ aBrackets summarize(StringWriter new); toLiteral
        ]
]

extension op
{
    isBalanced
    [
        var aCounter := Integer new:0.
    
        self seekEach(:ch)(aCounter append((ch==$91)iif (1,-1)) < 0).
    
        ^ (0 == aCounter)
    ]
}

// --- Program ---

public program
[
    0 to:9 do(:len)
    [
        var str := randomBrackets(len).

        console printLine("""",str,"""",str isBalanced; iif(" is balanced"," is not balanced"))
    ].

    console readChar
]
