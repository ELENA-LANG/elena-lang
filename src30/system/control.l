#define(const)true = system'true.
#define(const)false = system'false.

#type bool.

// --- true ---

#symbol(type:bool) true =
{
    then : aTrueAction = aTrueAction eval.

    then : aTrueAction &else:aFalseAction = aTrueAction eval.

    else : aTrueAction = nil.

    or : anExpr = $self.

    and : anExpr => &and::anExpr.

    xor : anExpr => &xor::anExpr.

    and &bool:aBool = aBool.

    and &action:aBoolExpr = aBoolExpr eval bool.

    and &object:aBool = aBool bool.

    xor &bool:aBool = aBool ? [ false ] ! [ true ].
    
    xor &action:aBoolExpr = aBoolExpr eval ? [ false ] ! [ true ].
            
    xor &object:anObject => &xor &bool::anObject.
            
    equal : aBool => &equal::aBool.
    
    equal &bool:aBool = aBool ? [ true ] ! [ false ].
            
    equal &action:aBoolExpr 
        = $self equal &bool::(aBoolExpr eval).
            
    equal &object:anObject => &equal &bool::anObject.
            
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

    or : anExpr => &or::anExpr.
            
    and : anExpr = $self.

    and &action:aBoolExpr = $self.
    
    xor : anExpr => &xor::anExpr.

    or &bool:aBool = aBool.

    or &object:anObject = anObject bool.

    or &action:aBool = aBool eval bool.
                
    xor &bool:aBool = aBool ? [ true ] ! [ false ].
    
    xor &action:anExpr = anExpr eval ? [ true ] ! [ false ].

    xor &object:anObject => &xor &bool::anObject.
    
    equal : aBool => &equal::aBool.
    
    equal &bool:aBool = aBool ? [ false ] ! [ true ].
            
    equal &action:aBoolExpr 
        = $self equal &bool::(aBoolExpr eval).
            
    equal &object:anObject => &equal &bool::anObject.
            
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
        #loop (true equal::aBool) ?
        [
            self eval.
        ].
    ]

    #method run &until:aBool
    [
        #loop 
        [
            self eval.
            
            false equal::aBool
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

//    #method run &range &int:aStart &int:n &int:dx
//    [
//        #var(type:int)i := aStart.
//        
//        #loop(i <= n) ?
//        [
//            self eval &int:i.
//            
//            i := i + dx.
//        ].
//        
//        ^ nil.        
//    ]          
//                    
//    #method rewind &range &int:aStart &int:n &int:dx
//    [
//        #var(type:int)i := aStart.
//        
//        #loop(i >= n) ?
//        [
//            self eval &int:i.
//            
//            i := i - dx.
//        ].
//        
//        ^ nil.        
//    ]          
//    
//    #method run &for:anIterator &from:aStart &to:anEndValue &by:aStep
//    [
//        anIterator << aStart.
//        
//        #loop (anIterator <= anEndValue) ?
//        [
//            self eval:anIterator.
//            
//            anIterator += aStep.
//        ].
//        
//        ^ nil.        
//    ]    
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

//    for:anEnumerator &do:aFunction
//        = aFunction~functionOp run &enumerator:anEnumerator | stop.

    foreach:anEnumerable &until:aFunction
        = aFunction~functionOp retrieve &enumerator::anEnumerable | stop.
    
//    forrange &int:aStart &int:anEnd &do:aFunction
//        = aStart < anEnd
//            ? [ aFunction~functionOp run &range &int:aStart &int:anEnd &int:1 | stop ]
//            ! [ aFunction~functionOp rewind &range &int:aStart &int:anEnd &int:1 | stop ].
//    
//    forrange &int:aStart &int:anEnd &int:aStep &do:aFunction
//        = aStart < anEnd
//            ? [ aFunction~functionOp run &range &int:aStart &int:anEnd &int:aStep | stop ]
//            ! [ aFunction~functionOp rewind &range &int:aStart &int:anEnd &int:aStep | stop ].
//
//    from:aStart &to:anEndValue &do:aFunction
//        = aFunction~functionOp run &for:(system'Integer new) &from:aStart &to:anEndValue &by:1 | stop.
}.
