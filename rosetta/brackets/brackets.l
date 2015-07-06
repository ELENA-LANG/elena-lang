// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

#define system.
#define system'routines.
#define extensions.

// --- RandomBrackets ---

#symbol randomBrackets =
{
    new : aLength
        = (0 == aLength)
            ? [ emptyLiteralValue ]
            ! [
                #var aBrackets := 
                    Array new &length:(aLength int) set &every: (&index:i) [ #91 ] 
                    + 
                    Array new &length:(aLength int) set &every: (&index:i)[ #93 ].
        
                aBrackets randomize:(aLength * 2).

                ^ aBrackets summarize:(String new) literal.
            ].
}.

#class(extension)op
{
    #method isBalanced
    [
        #var aCounter := Integer new:0.
    
        self seek &each:aChar [ (aCounter += (aChar => #91 ? [ 1 ] #93 ? [ -1 ])) < 0 ].
    
        ^ (0 == aCounter).
    ]
}

// --- Program ---

#symbol program =                                             
[
    0 to:9 &doEach: (:aLength)
    [
        #var anStr := randomBrackets new:aLength.

        console writeLine:"""":anStr:"""":((anStr isBalanced) => true ? [ " is balanced" ] false ? [ " is not balanced" ]).
    ].

    console readChar.
].
