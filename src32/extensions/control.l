import system'routines.

int extension intLoopOp
{
    stacksafe to(int anEnd) doEach(nfunc aFunction)
    [
       int aStart := self.

       if (aStart < anEnd)
            [ ^ aFunction run int:aStart int:anEnd int:1 ];
            [ ^ aFunction rewind int:aStart int:anEnd int:1 ]
    ]

    stacksafe to(int anEnd) doEach(func aFunction)
        = self~intLoopOp to:anEnd doEach(&int:n)[ aFunction eval:n ].
            
    stacksafe to(int anEnd) doEach(ext_mssg anExtension)
        = self~intLoopOp to:anEnd doEach(&int:n)[ n~anExtension get ].
            
    multi to:anEnd doEach:anAction
        = self~intLoopOp to:anEnd doEach(&int:n)[ anAction eval:n ].
}

extension forLoopOp
{
    to(object aMaxValue) doEach(func1 aFunction)
        = aFunction run enumerator:(RangeEnumerator new from:self to:aMaxValue).

    to(object aMaxValue) doEach(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue doEach(:n)[ n~aMessage get ].

    multi to:aMaxValue doEach:anAction
        = self~forLoopOp to:aMaxValue doEach(:n)[ anAction eval:n ].

//    to:aMaxValue by:aStep doEach:aFunction
//        = aFunction run enumerator:(RangeEnumerator new from:self to:aMaxValue by:aStep).

    till(object aMaxValue) doEach(func1 aFunction)
        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue).

    till(object aMaxValue) doEach(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue doEach(:n)[ n~aMessage get ].

    multi till:aMaxValue doEach:anAction
        = self~forLoopOp to:aMaxValue doEach(:n)[ anAction eval:n ].

    till(object aMaxValue) by(object aStep) doEach(func1 aFunction)
        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue by:aStep).

    till(object aMaxValue) by(object aStep) doEach(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue by:aStep doEach(:n)[ n~aMessage get ].

    multi till:aMaxValue by:aStep doEach:anAction
        = self~forLoopOp to:aMaxValue by:aStep doEach(:n)[ anAction eval:n ].

//    till:aMaxValue by:aStep doEach:aFunction
//        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue by:aStep).
//        
//    to:aMaxValue seekEach:aFunction
//        = aFunction retrieve enumerator:(RangeEnumerator new from:self to:aMaxValue).
//        
//    till:aMaxValue seekEach:aFunction
//        = aFunction retrieve enumerator:(IntervalEnumerator new from:self till:aMaxValue).
//        
//    to:aMaxValue by:aStep seekEach:aFunction
//        = aFunction retrieve enumerator:(RangeEnumerator new from:self to:aMaxValue by:aStep).
}

//extension loopOp
//{
//    doWhile:anAction
//    [
//        while (self bool)
//        [
//            anAction eval.
//        ].
//    ]
//    
//    doWhile
//    [
//        while (self bool)
//        [
//        ].
//    ]
//}
//
//// --- testOp ---
//
//extension testOp
//{
//    assert ifTrue
//    [
//        ifnot (self)
//            [ console writeLine:"Assertion fails!" ].
//    ]
//    
//    assert ifFalse
//    [
//        if(self)
//            [ console writeLine:"Assertion fails!" ].
//    ]
//}
//
//// --- controlOp ---
//
//extension controlOp
//{
//    save to:target
//    [
//        target write:self.
//        
//        ^   self.
//    ]
//}
//
//// --- control ---
//
//class control =
//{
////    try : anAction &catching:aHandler
////    [
////        ^ anAction eval | if:aHandler.
////    ]    
//    
//    eval:object for:action
//        = action eval:object.
//    
//    do:anAction = anAction eval.
//    
//    do:anAction until:anExpr
//        = anAction run until:anExpr.
//
//    while:anExpr
//        = $self while:anExpr do:[].
//
//    while:anExpr do:anAction
//        = anAction run while:anExpr.
//
//    run int:aStart int:anEnd every:aFunction
//        = (aStart < anEnd)
//            ? [ aFunction run int:aStart int:anEnd int:1 ]
//            ! [ aFunction rewind int:aStart int:anEnd int:1 ].
//    
//    run int:aStart int:anEnd int:aStep every:aFunction
//        = (aStart < anEnd)
//            ? [ aFunction run int:aStart int:anEnd int:aStep ]
//            ! [ aFunction rewind int:aStart int:anEnd int:aStep ].
//}.
