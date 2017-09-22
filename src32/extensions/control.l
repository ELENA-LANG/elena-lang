import system'routines.

int extension intLoopOp
{
    stacksafe runIntLoop int:anEnd nfunc:aFunction
    [
       int aStart := self.

       if (aStart < anEnd)
            [ ^ aFunction run int:aStart int:anEnd int:1 ];
            [ ^ aFunction rewind int:aStart int:anEnd int:1 ]
    ]

    stacksafe to(int anEnd) do(nfunc aFunction)
        = self~intLoopOp runIntLoop int:anEnd nfunc:aFunction.
            
    stacksafe to(int anEnd) do(func1 aFunction)
        = self~intLoopOp runIntLoop int:anEnd nfunc(&int:n)[ aFunction eval:n ].
            
    stacksafe to(int anEnd) do(ext_mssg anExtension)
        = self~intLoopOp runIntLoop int:anEnd nfunc(&int:n)[ n~anExtension get ].
            
    multi to:anEnd do:anAction
        = self~intLoopOp runIntLoop int(anEnd int) nfunc(&int:n)[ anAction eval:n ].
}

extension forLoopOp
{
    to(object aMaxValue) do(func1 aFunction)
        = aFunction run enumerator(RangeEnumerator new from:self to:aMaxValue).

    to(object aMaxValue) do(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue do(:n)[ n~aMessage get ].

    multi to:aMaxValue do:anAction
        = self~forLoopOp to:aMaxValue do(:n)[ anAction eval:n ].

    to(object aMaxValue) by(object aStep) do(func1 aFunction)
        = aFunction run enumerator(RangeEnumerator new from:self to:aMaxValue by:aStep).

    to(object aMaxValue) by(object aStep) do(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ n~aMessage get ].

    multi to:aMaxValue by:aStep do:anAction
        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ anAction eval:n ].

    till(object aMaxValue) do(func1 aFunction)
        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue).

    till(object aMaxValue) do(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue do(:n)[ n~aMessage get ].

    multi till:aMaxValue do:anAction
        = self~forLoopOp to:aMaxValue do(:n)[ anAction eval:n ].

    till(object aMaxValue) by(object aStep) do(func1 aFunction)
        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue by:aStep).

    till(object aMaxValue) by(object aStep) do(ExtensionMessage aMessage)
        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ n~aMessage get ].

    multi till:aMaxValue by:aStep do:anAction
        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ anAction eval:n ].

    to:aMaxValue seek:aFunction
        = aFunction retrieve enumerator(RangeEnumerator new from:self to:aMaxValue).
        
    till:aMaxValue seek:aFunction
        = aFunction retrieve enumerator(IntervalEnumerator new from:self till:aMaxValue).
        
    to:aMaxValue by:aStep seek:aFunction
        = aFunction retrieve enumerator(RangeEnumerator new from:self to:aMaxValue by:aStep).
}

extension loopOp
{
    doWhile:anAction
    [
        while (self bool)
        [
            anAction eval.
        ].
    ]
    
    doWhile
    [
        while (self bool)
        [
        ].
    ]
}

// --- testOp ---

extension testOp
{
    assertTrue
    [
        ifnot (self)
            [ console writeLine:"Assertion fails!" ].
    ]
    
    assertFalse
    [
        if(self)
            [ console writeLine:"Assertion fails!" ].
    ]
}

// --- controlOp ---

extension controlOp
{
    saveTo:target
    [
        target write:self.
        
        ^   self.
    ]
    
    exchangeWith:v
    [
        var tmp := self value.
        self set(v value).
        v set(tmp).
    ]
}

// --- control ---

class control =
{
//    try : anAction &catching:aHandler
//    [
//        ^ anAction eval | if:aHandler.
//    ]    
    
    do:object with:action
        = action eval:object.
    
    do:anAction = anAction eval.
    
    do:anAction until:anExpr
        = anAction run until:anExpr.

    while:anExpr
        = $self while:anExpr do:[].

    while:anExpr do:anAction
        = anAction run while:anExpr.

//    run int:aStart int:anEnd every:aFunction
//        = (aStart < anEnd)
//            ? [ aFunction run int:aStart int:anEnd int:1 ]
//            ! [ aFunction rewind int:aStart int:anEnd int:1 ].
//    
//    run int:aStart int:anEnd int:aStep every:aFunction
//        = (aStart < anEnd)
//            ? [ aFunction run int:aStart int:anEnd int:aStep ]
//            ! [ aFunction rewind int:aStart int:anEnd int:aStep ].
}.
