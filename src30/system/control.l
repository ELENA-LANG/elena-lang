#define(const)true = system'true.
#define(const)false = system'false.


// --- true ---

#symbol(type:bool) true =
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

#symbol(type:bool) false =
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

// --- action role class ---

#class(role) actionOp
{
    #method run &if:aBool
    [
        #loop aBool ?
        [
            self eval.
        ].
    ]

    #method run &until:aBool
    [
        #loop 
        [
            self eval.
            
            false equal:aBool
        ].
    ]
}

// --- function role class ---

#class(role) functionOp
{    
    #method run &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            self eval:(anEnumerator get).
        ].        
        
        ^ nil.        
    ]
    
//    #method run &enumerator:anEnumerator &where:aConditionFunction
//    [
//        #var anItem.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            aConditionFunction eval:anItem
//                ? [ self eval:anItem. ].
//        ].        
//        
//        ^ nil.        
//    ]
    
    #method retrieve &enumerator:anEnumerator
    [
        #var anItem.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            self eval:anItem ?
                [ #break anItem. ].
        ].        
        
        ^ nil.        
    ]

    #method run &range &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop(i <= n) ?
        [
            self eval &int:i.
            
            i := i + dx.
        ].
        
        ^ nil.        
    ]          
                    
    #method rewind &range &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop(i >= n) ?
        [
            self eval &int:i.
            
            i := i - dx.
        ].
        
        ^ nil.        
    ]          
    
    #method run &for:anIterator &from:aStart &to:anEndValue &by:aStep
    [
        anIterator << aStart.
        
        #loop (anIterator <= anEndValue) ?
        [
            self eval:anIterator.
            
            anIterator += aStep.
        ].
        
        ^ nil.        
    ]
}

// --- Action wrapper ---

#class(type:action) Action
{
    #method => <action_dispatch>.
}

// --- Expression ---

#class Expression
{
    #method => <expression_dispatch>.
}

// --- control ---

#symbol control =
{
    do:anAction = anAction eval | stop.
    
    do:anAction &until:anExpr
        = anAction~actionOp run &until:anExpr | stop.

    while:anExpr
        = $self while:anExpr &do:[].

    while:anExpr &do:anAction
        = anAction~actionOp run &if:anExpr | stop.

    foreach:anEnumerable &do:aFunction
        = aFunction~functionOp run &enumerator::anEnumerable | stop.

    foreach:anEnumerable &until:aFunction
        = aFunction~functionOp retrieve &enumerator::anEnumerable | stop.

    for:anEnumerator &do:aFunction
        = aFunction~functionOp run &enumerator:anEnumerator | stop.
    
    forrange &int:aStart &int:anEnd &do:aFunction
        = aStart < anEnd
            ? [ aFunction~functionOp run &range &int:aStart &int:anEnd &int:1 | stop ]
            ! [ aFunction~functionOp rewind &range &int:aStart &int:anEnd &int:1 | stop ].
    
    forrange &int:aStart &int:anEnd &int:aStep &do:aFunction
        = aStart < anEnd
            ? [ aFunction~functionOp run &range &int:aStart &int:anEnd &int:aStep | stop ]
            ! [ aFunction~functionOp rewind &range &int:aStart &int:anEnd &int:aStep | stop ].

    from:aStart &to:anEndValue &do:aFunction
        = aFunction~functionOp run &for:(system'Integer new) &from:aStart &to:anEndValue &by:1 | stop.
}.
