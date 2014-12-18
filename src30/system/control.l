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

// --- base functions ---

#class(limited,type:func)BaseFunction
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

#class(limited,type:func1)BaseFunction1
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

#class(limited,type:func2)BaseFunction2
{
    #method eval : aParam1 : aParam2 []
}

#class(limited,type:func3)BaseFunction3
{
    #method eval : aParam1 : aParam2 : aParam3 []
}

#class(limited,type:func4)BaseFunction4
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 []
}

#class(limited,type:func5)BaseFunction5
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
}

#class(limited,type:func6)BaseFunction6
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
}

#class(limited,type:func7)BaseFunction7
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
}

#class(limited,type:func8)BaseFunction8
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
}

#class(limited,type:func9)BaseFunction9
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
}

#class(limited,type:func10)BaseFunction10
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
}

#class(limited,type:func11)BaseFunction11
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
}

#class(limited,type:func12)BaseFunction12
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 : aParam12 []
}

#class(limited,type:func13)BaseFunction13
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 : aParam12 : aParam13 []
}

#class(limited,type:func14)BaseFunction14
{
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 : aParam12 : aParam13 : aParam14 []
}

// --- base lazy expression ---

#class(limited)BaseLazyExpression
{
    #method eval []
    
    #method => <expression_dispatch>.
}

// --- IndexFunction ---

#class(limited,type:indexfunc)IndexFunction
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
}

#class(limited,type:matrixfunc)IndexFunction2
{
    #method eval &index:i &index:j []

    #method rewind &matrix &int:i &int:j &int:n &int:m &int:dx
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
    
    #method run &matrix &int:i &int:j &int:n &int:m &int:dx
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

#class(limited,type:coordfunc)IndexFunction3
{
    #method eval &index:i &index:j &index:k []

    #method rewind &coord &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
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
    
    #method run &coord &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
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

#class Boolean : BaseValue
{
    #field theValue.

    #constructor new : aValue
    [        
        theValue := true == aValue.
    ]

    #method value = theValue.

    #method write : aValue
    [
        theValue := true == aValue.
    ]

    #method bool = theValue bool.

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
            
    #method clone = Boolean new:($self value).

    #method cast : aTypecaster = aTypecaster bool:($self value).
            
    #method => theValue.
}

//// --- function role class ---
//
//#class(extension:func1) functionOp
//{    
////    #method run &enumerator:anEnumerator &where:aConditionFunction
////    [
////        #var anItem.
////        #loop (anEnumerator next) ?
////        [
////            anItem := anEnumerator get.
////            
////            aConditionFunction eval:anItem
////                ? [ self eval:anItem. ].
////        ].        
////        
////        ^ nil.        
////    ]
//
//    #method run &range &int:aStart &int:n &int:dx
//    [
//        #var(type:int)i := aStart.
//        
//        #loop(i <= n) ?
//        [
//            self eval:i.
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
//            self eval:i.
//            
//            i := i - dx.
//        ].
//        
//        ^ nil.        
//    ]
//    
////    #method run &for:anIterator &from:aStart &to:anEndValue &by:aStep
////    [
////        anIterator << aStart.
////        
////        #loop (anIterator <= anEndValue) ?
////        [
////            self eval:anIterator.
////            
////            anIterator += aStep.
////        ].
////        
////        ^ nil.        
////    ]
//}

// --- control ---

#symbol(type:control) control =
{
    do:anAction = anAction eval.
    
    do:anAction &until:anExpr
        = anAction run &until:anExpr.

    while:anExpr
        = $self while:anExpr &do:[].

    while:anExpr &do:anAction
        = anAction run &if:anExpr.

    run:anEnumerable &forEach:aFunction
        = aFunction run &enumerator::anEnumerable.

    run:anEnumerable &seeking:aFunction
        = aFunction retrieve &enumerator::anEnumerable.

////    for:anEnumerator &do:aFunction
////        = aFunction~functionOp run &enumerator:anEnumerator | stop.

    runRange &int:aStart &int:anEnd &forEvery:aFunction
        = aStart < anEnd
            ? [ aFunction run &range &int:aStart &int:anEnd &int:1 ]
            ! [ aFunction rewind &range &int:aStart &int:anEnd &int:1 ].
    
    runRange &int:aStart &int:anEnd &int:aStep &forEvery:aFunction
        = aStart < anEnd
            ? [ aFunction run &range &int:aStart &int:anEnd &int:aStep ]
            ! [ aFunction rewind &range &int:aStart &int:anEnd &int:aStep ].

////    from:aStart &to:anEndValue &do:aFunction
////        = aFunction~functionOp run &for:(system'Integer new) &from:aStart &to:anEndValue &by:1 | stop.
}.
