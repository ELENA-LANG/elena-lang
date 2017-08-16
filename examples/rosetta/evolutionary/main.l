#import system'routines.
#import extensions.

// --- program ---

const Target = "METHINKS IT IS LIKE A WEASEL".
const AllowedCharacters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ".

const C = 100.
const P = 0.05r.

rnd = randomGenerator.

randomChar 
    = AllowedCharacters @ (rnd nextInt:(AllowedCharacters length)).

extension evoHelper
{
    randomString
        = 0 till:self repeat(:x) [ randomChar ] summarize:(String new) literal.
        
    fitness &of:s
        = self zip:s &into:(:a:b)[ (a == b)iif:1:0 ] summarize:(Integer new) int.
        
    mutate : p
        = self select &each: ch [ (rnd nextReal <= p) iif:randomChar:ch ] summarize:(String new) literal.
}

class EvoAlgorithm :: Enumerator
{
    object theTarget.
    object theCurrent.
    object theVariantCount.

    constructor new : s &of:count
    [
        theTarget := s.
        theVariantCount := count int.
    ]

    get = theCurrent.

    next
    [
        if ($nil == theCurrent)
            [ theCurrent := theTarget length randomString. ^ true. ].
            
        if (theTarget == theCurrent)
            [ ^ false. ].
            
        var variants := Array new:theVariantCount set &every:(&index:x) [ theCurrent mutate:P ].
        
        theCurrent := variants array sort:(:a:b) [ a fitness &of:Target > b fitness &of:Target ] getAt:0.
        
        ^ true.
    ]            
}

program =
[
    var attempt := Integer new.
    EvoAlgorithm new:Target &of:C run &each:current
    [
        console 
            writeLiteral:"#":(attempt += 1) &paddingLeft:10
            writeLine:" ":current:" fitness: ":(current fitness &of:Target).
    ].
    
    console readChar.
].