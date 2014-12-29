// --- true ---

#define(const)true = system'true.
#define(const)false = system'false.

#symbol true =
{
    then : aTrueAction = aTrueAction eval.

    then : aTrueAction &else:aFalseAction = aTrueAction eval.

    else : aTrueAction = nil.

    or : anExpr = $self.

    and : anExpr = anExpr bool.

    xor : aBool = aBool ? [ false ] ! [ true ].

    equal : anExpr = anExpr bool.

    not = false.

    literal = "true".

    bool = $self.

    clone = $self.
}.

// --- false ---

#symbol false =
{
    then : anAction = nil.
    
    then : aTrueAction &else:aFalseAction = aFalseAction eval.

    else : aFalseAction = aFalseAction eval.

    or : anExpr = anExpr bool.

    and : anExpr = $self.

    xor : anExpr = anExpr ? [ true ] ! [ false ].

    equal : anExpr = anExpr ? [ false ] ! [ true ].

    not = true.

    literal = "false".

    bool = $self.

    clone = $self.
}.

// --- base functions ---

#class(limited)BaseFunction
{
    #method eval []
    
    #method run &if:aBool
    [
        #loop aBool ?
        [
            $self eval.
        ].
    ]

    #method run &until:aBool
    [
        #var(type:bool)flag := false.
        
        #loop flag !
        [
            $self eval.
            
            flag := aBool bool.
        ].
    ]
}

#class(limited)BaseFunction1
{
    #method eval : aParam []
    
    #method run &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:(anEnumerator get).
        ].        
    ]
    
    #method retrieve &enumerator:anEnumerator
    [
        #var anItem := nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            $self eval:anItem ?
                [ ^ anItem. ].
        ].        
        
        ^ nil.        
    ]
}

#class(limited)BaseFunction2
{
    #method eval : aParam1 : aParam2 []
}

#class(limited)BaseFunction3
{
    #method eval : aParam1 : aParam2 : aParam3 []
}

#class(limited)BaseFunction4
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 []
}

#class(limited)BaseFunction5
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
}

#class(limited)BaseFunction6
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
}

#class(limited)BaseFunction7
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
}

#class(limited)BaseFunction8
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
}

#class(limited)BaseFunction9
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
}

#class(limited)BaseFunction10
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
}

#class(limited)BaseFunction11
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
}

#class(limited)BaseFunction12
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 : aParam12 []
}

#class(limited)BaseFunction13
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 : aParam12 : aParam13 []
}

#class(limited)BaseFunction14
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 : aParam12 : aParam13 : aParam14 []
}

// --- base lazy expression ---

#class(limited)BaseLazyExpression
{
    #method eval []
    
    #method => system'internal'expression_dispatch.
}

// --- IndexFunction ---

#class(limited)IndexFunction1
{
    #method eval &index:n []

    #method rewind &range &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop(i >= n) ?
        [
            $self eval &index:i.
            
            i := i - dx.
        ].
        
        ^ nil.        
    ]          
    
    #method run &range &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop(i <= n) ?
        [
            $self eval &index:i.
            
            i := i + dx.
        ].
        
        ^ nil.        
    ]
    
    #method retrieve &range &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop(i <= n) ?
        [
            ($self eval &index:i)
                ? [ ^ IntNumber new &int:i. ].
            
            i := i + dx.
        ].
        
        ^ nil.        
    ]
}

#class(limited)IndexFunction2
{
    #method eval &index:i &index:j []

    #method rewind &area &int:i &int:j &int:n &int:m &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        
        #loop(x >= n) ?
        [
            y := j.
            
            #loop (y >= m) ?
            [
                $self eval &index:x &index:y.
                
                y := y - dx.
            ].
                        
            x := x - dx.
        ].
        
        ^ nil.        
    ]          
    
    #method run &area &int:i &int:j &int:n &int:m &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        
        #loop(x <= n) ?
        [
            y := j.
            
            #loop (y <= m) ?
            [
                $self eval &index:x &index:y.
                
                y := y + dx.
            ].
                        
            x := x + dx.
        ].
        
        ^ nil.        
    ]
}

#class(limited)IndexFunction3
{
    #method eval &index:i &index:j &index:k []

    #method rewind &space &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        #var(type:int)z := k.
        
        #loop(x >= n) ?
        [
            y := j.
            
            #loop (y >= m) ?
            [
                z := k.
                
                #loop (z >= p) ?
                [
                    $self eval &index:x &index:y &index:z.
                    
                    z := z - dx.
                ].                
                
                y := y - dx.
            ].
                        
            x := x - dx.
        ].
        
        ^ nil.        
    ]          
    
    #method run &space &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        #var(type:int)z := k.
        
        #loop(x <= n) ?
        [
            y := j.
            
            #loop (y <= m) ?
            [
                z := k.
                
                #loop (z <= p) ?
                [
                    $self eval &index:x &index:y &index:z.
                    
                    z := z + dx.
                ].                
                
                y := y + dx.
            ].
                        
            x := x + dx.
        ].
        
        ^ nil.        
    ]
}

// --- Boolean ---

#class Boolean :: BaseValue
{
    #field(type:bool) theValue.

    #constructor new : aValue
    [        
        theValue := true == aValue.
    ]

    #method value = theValue.

    #method write : aValue
    [
        theValue := true == aValue.
    ]

    #method bool = theValue.

    #method append : aValue
    [
        theValue := theValue or:aValue.
    ]
            
    #method multiplyBy : aValue
    [
        theValue := theValue and:aValue.
    ]
            
    #method invert
    [
        theValue := theValue not.
    ]            

    #method equal &bool:aValue
        => theValue.
            
    #method clone = Boolean new:theValue.

    #method cast:aVerb &to:aTareget = aTareget::aVerb bool:theValue.
            
    #method => theValue.
}
