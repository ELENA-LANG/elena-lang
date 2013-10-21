// --- true ---

#symbol true =
{
    then : aTrueAction = aTrueAction eval.

    then : aTrueAction &else:aFalseAction = aTrueAction eval.

    else : aTrueAction = nil.

    or : anExpr = $self.
            
    and : anExpr <= and::anExpr.
    
    xor : anExpr <= xor::anExpr.

    and &bool:aBool = aBool.

    and &action:aBoolExpr = aBoolExpr eval.
    
    and &type'int:anInt 
        = 0 != anInt.
                
    xor &bool:aBool = aBool ? [ false ] ! [ true ].
    
    xor &action:aBoolExpr = aBoolExpr eval ? [ false ] ! [ true ].
            
    equal : aBool <= equal::aBool.
    
    equal &bool:aBool <= equal &type'object:aBool.
            
    equal &action:aBoolExpr 
        = $self equal &bool:(aBoolExpr eval).
            
    not = false.

    Literal = "true".

    <= &&bool:$self.    
}.

// --- false ---

#symbol false =
{
    then : anAction = nil.
    
    then : aTrueAction &else:aFalseAction = aFalseAction eval.

    else : aFalseAction = aFalseAction eval.

    or : anExpr <= or::anExpr.
            
    and : anExpr = $self.
    
    and &action:aBoolExpr = $self.
    
    xor : anExpr <= xor::anExpr.

    or &bool:aBool = aBool.

    or &action:aBool = aBool eval.
        
    or &type'int:anInt 
        = 0 != anInt.
                
    xor &bool:aBool = aBool ? [ true ] ! [ false ].
    
    xor &action:anExpr = anExpr eval ? [ true ] ! [ false ].

    equal : aBool <= equal::aBool.
    
    equal &bool:aBool <= equal &type'object:aBool.
            
    equal &action:aBoolExpr 
        = $self equal &bool:(aBoolExpr eval).
            
    not = true.

    Literal = "false".

    <= &&bool:$self.    
}.

// --- action role class ---

#class EAction
{
    #method run &if:aBool
    [
        #loop (true equal::aBool) ?
        [
            self eval.
        ].
    ]

    #method run &until:aBool
    [
        self eval.
        
        #loop (true equal::aBool) !
        [
            self eval.
        ].
    ]    
    
    #method iterate &each:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            self eval:(anEnumerator get).
        ].        
        
        ^ nil.        
    ]
    
    #method iterate &for:anIterator &from:aStart &to:anEndValue &by:aStep
    [
        anIterator << aStart.
        
        #loop (anIterator <= anEndValue) ?
        [
            self eval:anIterator.
            
            anIterator += aStep.
        ].
        
        ^ nil.        
    ]
    
    #method reverse &for:anIterator &from:aStart &to:anEndValue &by:aStep
    [
        anIterator << anEndValue.
        
        #loop (anIterator >= aStart) ?
        [
            self eval:anIterator.
            
            anIterator -= aStep.
        ].
        
        ^ nil.
    ]    
    
    #method reverse &for:anIterator &from:aStart &till:anEndValue &by:aStep
    [
        anIterator << anEndValue.
        
        #loop (anIterator > aStart) ?
        [
            anIterator -= aStep.
            
            self eval:anIterator.
        ].
        
        ^ nil.
    ]    
}

// --- control ---

#symbol control =
{
    do:anAction = anAction eval.
    
    do:anAction &until:anExpr
        = anAction~EAction run &until:anExpr | stop.
        
    while:anExpr
        = $self while:anExpr &do:[].

    while:anExpr &do:anAction
        = anAction~EAction run &if:anExpr | stop.

    foreach:anEnumerable &do:anAction
        = anAction~EAction iterate &each:(anEnumerable Enumerator) | stop.

    foreach:anEnumerable &until:anAction
    [
        #var anEnumerator := anEnumerable Enumerator.
        
        ^ control while:[anEnumerator next] &do:
        [
            anAction eval:(anEnumerator get) ?
                [ #break anEnumerator get. ].
        ].
    ]

    foreach:anEnumerable &until:aCodition &do:anAction
    [
        #var anEnumerator := anEnumerable Enumerator.
        
        ^ control while:[ anEnumerator next and::aCodition ] 
            &do:[ anAction eval:(anEnumerator get). ].
    ]

    for:anIterator &do:anAction
     = anAction~EAction iterate &for:anIterator | stop.

    for:anIterator &from:aStart &to:anEndValue &by:aStep &do:anAction
        = anAction~EAction iterate &for:anIterator &from:aStart &to:anEndValue &by:aStep | stop.

    from:aStart &to:anEndValue &do:anAction
        = anAction~EAction iterate &for:(system'Integer new) &from:aStart &to:anEndValue &by:1 | stop.
            
    from:aStart &till:anEndValue &do:anAction
        = anAction~EAction iterate &for:(system'Integer new) &from:aStart &to:(-1 + anEndValue) &by:1 | stop.
            
    for:anIterator &from:aStart &backTo:anEndValue &by:aStep &do:anAction
        = anAction~EAction reverse &for:anIterator &from:anEndValue &to:aStart &by:aStep | stop.
            
    from:aStart &backTo:anEndValue &do:anAction
        = anAction~EAction reverse &for:(system'Integer new) &from:anEndValue &to:aStart &by:1 | stop.
}.
