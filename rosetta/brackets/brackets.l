// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

#define std'dictionary'*.
#define std'basic'*.
#define std'patterns'*.
#define ext'utils'*.
#define ext'routines'*.

// --- RandomBrackets ---

#symbol RandomBrackets : aLength =
    Randomized 
        &&count:(aLength * 2)
        &base:(FilledWideLiteral &&pattern:"[" &count:aLength + (FilledWideLiteral &&pattern:"]" &count:aLength)).

#symbol IsBalanced : aLiteral =
[
    #var aCounter := Integer::0.
    
    #if Scan::aLiteral seek: aChar = (aCounter append:(aChar ifequal:"[" ^^1 | ^^-1) < 0).
    
    ^ (0 == aCounter).
].

// --- Program ---

#symbol Program =
[
    loop &&from:0 &till:10 run: aLength =
    [
        #var anStr := RandomBrackets::aLength.
        #var balanced := IsBalanced::anStr.
        
        'program'output << """" << anStr << (balanced is ^^""" is balanced%n" | ^^""" is not balanced%n").
    ].
    
    'program'input get.
].
