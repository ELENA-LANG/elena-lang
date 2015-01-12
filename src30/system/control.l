// --- true ---

#define(const)true = system'true.
#define(const)false = system'false.

#class(limited) BaseBoolValue
{
    #method then : aTrueAction = nil.

    #method then : aTrueAction &else:aFalseAction = nil.

    #method else : aTrueAction = nil.

    #method(type:bool) or : anExpr = $self.

    #method(type:bool) and : anExpr = $self.

    #method(type:bool) xor : aBool = $self.

    #method equal : anExpr = $self.

    #method(type:bool) equal &bool:aBool = $self.

    #method(type:bool) not = $self.

    #method literal = "unknown".

    #method bool = $self.

    #method clone = $self.
}

#symbol true = BaseBoolValue
{
    #method then : aTrueAction = aTrueAction eval.

    #method then : aTrueAction &else:aFalseAction = aTrueAction eval.

    #method else : aTrueAction = nil.

    #method(type:bool) or : anExpr = $self.

    #method(type:bool) and : anExpr = anExpr bool.

    #method(type:bool) xor : aBool
    [
        aBool ? [ ^ false bool. ].
        
        ^ $self.
    ]

    #method equal : anExpr = anExpr bool.

    #method(type:bool) equal &bool:aBool = aBool.

    #method(type:bool) not = false.

    #method literal = "true".

    #method bool = $self.

    #method clone = $self.
}.

// --- false ---

#symbol false = BaseBoolValue
{
    #method then : anAction = nil.
    
    #method then : aTrueAction &else:aFalseAction = aFalseAction eval.

    #method else : aFalseAction = aFalseAction eval.

    #method(type:bool) or : anExpr = anExpr bool.

    #method(type:bool) and : anExpr = $self.

    #method(type:bool) xor : anExpr 
    [
        anExpr ? [ ^ true. ].
        
        ^ $self.
    ]

    #method equal : anExpr
    [
        anExpr ? [ ^ $self. ].
        
        ^ true.
    ]

    #method(type:bool) equal &bool:anExpr
    [
        anExpr ? [ ^ $self. ].
        
        ^ true.
    ]

    #method not = true.

    #method literal = "false".

    #method bool = $self.

    #method clone = $self.
}.

// === Basic prototypes ===

// --- Enumerator ---

#class(limited) Enumerator
{
    #method enumerator = $self.
    
    #method get = nil.
    
    #method(type:bool) next = false.
    
    #method reset []
}

// --- Iterator ---

#class(limited) Iterator
{
    #method iterator = $self.
    
    #method(type:bool) available = false.

    #method get = nil.
    
    #method set : anObject []
    
    #method read &out'index:anIndex []
    
    #method index
    [
        #var(type:int)anIndex := $self index.
                        
        ^ IntNumber new &int:anIndex.
    ]                
    
    #method write &index:anIndex [] 
    
    #method append &index:anOffset
    [
        #var(type:int)anIndex := $self index.
        
        $self write &index:(anIndex + anOffset).
    ]
        
    #method reduce &index:anOffset
    [
        #var(type:int)anIndex := $self index.
        
        $self write &index:(anIndex - anOffset).
    ]
}

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
    #method cast : aVerb &to:aTarget = aTarget::aVerb func1:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func1:$self.
    
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
    #method cast : aVerb &to:aTarget = aTarget::aVerb func2:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func2:$self.
    
    #method eval : aParam1 : aParam2 []
}

#class(limited)BaseFunction3
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func3:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func3:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 []
}

#class(limited)BaseFunction4
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func4:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func4:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 []
}

#class(limited)BaseFunction5
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func5:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func5:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
}

#class(limited)BaseFunction6
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func6:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func6:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
}

#class(limited)BaseFunction7
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func7:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func7:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
}

#class(limited)BaseFunction8
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func8:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func8:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
}

#class(limited)BaseFunction9
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func9:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func9:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
}

#class(limited)BaseFunction10
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func10:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func10:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
}

#class(limited)BaseFunction11
{
    #method cast : aVerb &to:aTarget = aTarget::aVerb func11:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &func11:$self.
    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
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
    #method cast : aVerb &to:aTarget = aTarget::aVerb nfunc:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &nfunc:$self.
    
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
    #method cast : aVerb &to:aTarget = aTarget::aVerb nnfunc:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &nnfunc:$self.
    
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
    #method cast : aVerb &to:aTarget = aTarget::aVerb n3func:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &n3func:$self.
    
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
        theValue := true equal &bool:(aValue bool).
    ]

    #method value = theValue.

    #method write : aValue
    [
        theValue := true equal &bool:(aValue bool).
    ]

    #method bool = theValue.

    #method append : aValue
    [
        theValue := theValue or:aValue bool.
    ]
            
    #method multiplyBy : aValue
    [
        theValue := theValue and:aValue bool.
    ]
            
    #method invert
    [
        theValue := theValue not bool.
    ]            

    #method equal &bool:aValue
        => theValue.
            
    #method clone = Boolean new:theValue.

    #method cast:aVerb &to:aTareget = aTareget::aVerb bool:theValue.
            
    #method => theValue.
}

