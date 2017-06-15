import system'routines.
import extensions.
import extensions'routines.

// --- Joinable --

joinable = (:aFormer:aLater)
    [ ^ aFormer@(aFormer length - 1) == aLater@0 ].

dispatcher =
{
    eval : anArray func2:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1).
    ]

    eval : anArray func3:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1):(anArray@2).
    ]
    
    eval : anArray func4:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1):(anArray@2):(anArray@3).
    ]
    
    eval : anArray func5:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1):(anArray@2):(anArray@3):(anArray@4).
    ]

}.

// --- AmbValueCollection ---

class AmbValueCollection
{
    object theCombinator.
    
    constructor new args:Arguments
    [
        theCombinator := SequentialEnumerator new args:Arguments.
    ]

    seek : aCondition
    [
        theCombinator reset.

        theCombinator seek each: (:v)
        [
            ^ aCondition cast:%eval to:dispatcher with:v.
        ].
    ]
    
    do : aFunction
    [
        var aResult := theCombinator get.
        if (nil != aResult)
            [ aFunction cast:%eval to:dispatcher with:aResult ];
            [ InvalidArgumentException new; raise ]
    ]
}

// --- ambOperator ---

ambOperator =
{
    generic for args:Arguments
        = AmbValueCollection new args:Arguments.
}.

// --- Program ---

program1 =
[
    try(ambOperator for:(1,2,4):(4,5,6); seek: (:a:b) [ ^a * b == 8 ]; do: (:a:b)
        [ console writeLine: a : " * " : b : " = 8" ])
    {
        generic : e
        [
            console writeLine:"AMB is angry"
        ]
    }.
        
    console readChar.        
].

program2 =
[
    try(ambOperator 
        for:("the","that","a"):("frog", "elephant", "thing"):("walked", "treaded", "grows"):("slowly", "quickly");
        seek: (:a:b:c:d) [ ^ joinable eval:a:b && joinable eval:b:c && joinable eval:c:d ];
        do: (:a:b:c:d) [ console writeLine:a:" ":b:" ":c:" ":d ])
    {
        generic : e
        [
            console writeLine:"AMB is angry"
        ]        
    }.
        
    console readChar.
].
