#define system.
#define system'routines.
#define extensions.
#define extensions'routines.

// --- Joinable --

#symbol joinable = (:aFormer:aLater)
    [ (aFormer@(aFormer length - 1)) == (aLater@0) ].

#symbol dispatcher =
{
    eval : anArray &func2:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1).
    ]

    eval : anArray &func3:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1):(anArray@2).
    ]
    
    eval : anArray &func4:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1):(anArray@2):(anArray@3).
    ]
    
    eval : anArray &func5:aFunction
    [
        ^ aFunction eval:(anArray@0):(anArray@1):(anArray@2):(anArray@3):(anArray@4).
    ]

}.

// --- AmbValueCollection ---

#class AmbValueCollection
{
    #field theCombinator.
    
    #constructor new &args:Arguments
    [
        theCombinator := SequentialEnumerator new &args:Arguments.
    ]

    #method seek : aCondition
    [
        theCombinator reset.

        theCombinator seek &each: v
        [
            ^ aCondition cast:%eval &to:dispatcher &with:v.
        ].
    ]
    
    #method do : aFunction
    [
        #var aResult := theCombinator get.
        (nil != aResult)
            ? [ aFunction cast:%eval &to:dispatcher &with:aResult. ]
            ! [ #throw InvalidArgumentException new. ].
    ]
}

// --- ambOperator ---

#symbol ambOperator =
{
    for &args:Arguments
        = AmbValueCollection new &args:Arguments.
}.

// --- Program ---

#symbol program1 =
[
    ambOperator for &args:(1,2,4):(4,5,6) seek: (:a:b) [ a * b == 8 ] do: (:a:b)
        [ console writeLine: a : " * " : b : " = 8" ]
        | if &InvalidArgumentError: e [ console writeLine:"AMB is angry". ].
].

#symbol program2 =
[
    ambOperator 
        for &args:("the","that","a"):("frog", "elephant", "thing"):("walked", "treaded", "grows"):("slowly", "quickly")
        seek: (:a:b:c:d) [ (joinable:a:b) and:(joinable:b:c) and:(joinable:c:d) ]
        do: (:a:b:c:d) [ console writeLine:a:" ":b:" ":c:" ":d. ]
        | if &InvalidArgumentError: e [ console writeLine:"AMB is angry". ].
].
