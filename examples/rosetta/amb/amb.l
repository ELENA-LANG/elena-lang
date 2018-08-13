import system'routines.
import extensions.
import extensions'routines.

// --- Joinable --

joinable(former,later) = (former[former length - 1] == later[0]).

dispatcher =
{
    eval(object a, Func2 f)
    [
        ^ f(a[0],a[1]).
    ]

    eval(object a, Func3 f)
    [
        ^ f(a[0], a[1],a[2]).
    ]
    
    eval(object a, Func4 f)
    [
        ^ f(a[0],a[1],a[2],a[3]).
    ]
    
    eval(object a, Func5 f)
    [
        ^ f(a[0],a[1],a[2],a[3],a[4]).
    ]

}.

// --- AmbValueCollection ---

class AmbValueCollection
{
    object theCombinator.
    
    generic constructor new(V<object> args)
    [
        theCombinator := SequentialEnumerator new(args).
    ]

    seek : cond
    [
        theCombinator reset.

        theCombinator seekEach(:v)(dispatcher eval(v,cond))
    ]
    
    do : aFunction
    [
        var aResult := theCombinator get.
        if (nil != aResult)
            [ dispatcher eval(aResult,aFunction) ];
            [ InvalidArgumentException new; raise ]
    ]
}

// --- ambOperator ---

ambOperator =
{
    generic for(V<object> args)
        = AmbValueCollection new(args).
}.

// --- Program ---

public program1
[
    try(ambOperator for((1,2,4),(4,5,6)); seek(:a:b)(a * b == 8); do(:a:b)
        [ console printLine(a," * ",b," = 8") ])
    {
        on(Exception e)
        [
            console printLine:"AMB is angry"
        ]
    }.
        
    console readChar
]

public program2
[
    try(ambOperator 
        for(("the","that","a"),("frog", "elephant", "thing"),("walked", "treaded", "grows"),("slowly", "quickly"));
        seek(:a:b:c:d) ( joinable(a,b) && joinable(b,c) && joinable(c,d) );
        do(:a:b:c:d) [ console printLine(a," ",b," ",c," ",d) ])
    {
        on(Exception e)
        [
            console printLine:"AMB is angry"
        ]        
    }.
        
    console readChar
]
