import system'routines.

public extension<int> intLoopOp
{
    stacksafe runIntLoop(int anEnd, Func<int,object> aFunction)
    [
       int aStart := self.

       if (aStart < anEnd)
            [ ^ aFunction run(aStart, anEnd, 1) ];
            [ ^ aFunction rewind(aStart, anEnd, 1) ]
    ]

    stacksafe runIntLoop(int anEnd, int aStep, Func<int,object> aFunction)
    [
       int aStart := self.

       if (aStep > 0)
            [ ^ aFunction run(aStart, anEnd, aStep) ];
            [ ^ aFunction rewind(aStart, anEnd, aStep) ]
    ]

    stacksafe to(int anEnd) do(Func<int,object> aFunction)
        = self~intLoopOp runIntLoop(anEnd, aFunction).
            
    stacksafe till(int anEnd) do(Func<int,object> aFunction)
        = self~intLoopOp runIntLoop(anEnd-1, aFunction).
            
    stacksafe to(int anEnd) do(Func1 aFunction)
        = self~intLoopOp runIntLoop(anEnd, (:n)<int>[ aFunction(n) ]).
            
    stacksafe till(int anEnd) do(Func1 aFunction)
        = self~intLoopOp runIntLoop(anEnd-1, (:n)<int>[ aFunction(n) ]).
            
//    stacksafe to(int anEnd) do(ext_mssg anExtension)
//        = self~intLoopOp runIntLoop int:anEnd nfunc(:n)<int>[ n~anExtension get ].
//            
//    multi to:anEnd do:anAction
//        = self~intLoopOp runIntLoop int(anEnd int) nfunc(:n)<int>[ anAction eval:n ].
//        
//    multi till:anEnd do:anAction
//        = self~intLoopOp runIntLoop int(anEnd int - 1) nfunc(:n)<int>[ anAction eval:n ].
                
    stacksafe to(int anEnd) by(int aStep) do(Func<int,object> aFunction)
        = self~intLoopOp runIntLoop(anEnd, aStep, aFunction).
            
//    stacksafe till(int anEnd) by(int aStep) do(nfunc aFunction)
//        = self~intLoopOp runIntLoop int(anEnd-1) int:aStep nfunc:aFunction.
            
    stacksafe to(int anEnd) by(int aStep) do(Func1 aFunction)
        = self~intLoopOp runIntLoop(anEnd, aStep, (:n)<int>[ aFunction(n) ]).
            
//    stacksafe to(int anEnd) by(int aStep) do(ext_mssg anExtension)
//        = self~intLoopOp runIntLoop int:anEnd int:aStep nfunc(:n)<int>[ n~anExtension get ].
//            
//    multi to:anEnd by:aStep do:anAction
//        = self~intLoopOp runIntLoop int(anEnd int) int:aStep nfunc(:n)<int>[ anAction eval:n ].
//        
//    multi till:anEnd by:aStep do:anAction
//        = self~intLoopOp runIntLoop int(anEnd int - 1) int:aStep nfunc(:n)<int>[ anAction eval:n ].
//        
}

public extension forLoopOp
{
    to(object aMaxValue) do(Func1 aFunction)
        = aFunction run(RangeEnumerator from:self to:aMaxValue).

//    to(object aMaxValue) do(ExtensionMessage aMessage)
//        = self~forLoopOp to:aMaxValue do(:n)[ n~aMessage get ].
//
//    multi to:aMaxValue do:anAction
//        = self~forLoopOp to:aMaxValue do(:n)[ anAction eval:n ].

    to(object aMaxValue) by(object aStep) do(Func1 aFunction)
        = aFunction run(RangeEnumerator from:self to:aMaxValue by:aStep).

//    to(object aMaxValue) by(object aStep) do(ExtensionMessage aMessage)
//        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ n~aMessage get ].
//
//    multi to:aMaxValue by:aStep do:anAction
//        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ anAction eval:n ].

    till(object aMaxValue) do(Func1 aFunction)
        = aFunction run(IntervalEnumerator from:self till:aMaxValue).

//    till(object aMaxValue) do(ExtensionMessage aMessage)
//        = self~forLoopOp to:aMaxValue do(:n)[ n~aMessage get ].
//
//    multi till:aMaxValue do:anAction
//        = self~forLoopOp to:aMaxValue do(:n)[ anAction eval:n ].

//    till(int aMaxValue) do(nfunc aFunction)
//        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue by:aStep).

    till(object aMaxValue) by(object aStep) do(Func1 aFunction)
        = aFunction run(IntervalEnumerator from:self till:aMaxValue by:aStep).

//    till(object aMaxValue) by(object aStep) do(ExtensionMessage aMessage)
//        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ n~aMessage get ].
//
//    multi till:aMaxValue by:aStep do:anAction
//        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ anAction eval:n ].
//
//    to:aMaxValue seek:aFunction
//        = aFunction retrieve enumerator(RangeEnumerator new from:self to:aMaxValue).
//        
//    till:aMaxValue seek:aFunction
//        = aFunction retrieve enumerator(IntervalEnumerator new from:self till:aMaxValue).
//        
//    to:aMaxValue by:aStep seek:aFunction
//        = aFunction retrieve enumerator(RangeEnumerator new from:self to:aMaxValue by:aStep).
}

public extension loopOp
{
    doWhile:anAction
    [
        while (T<bool>(self))
        [
            anAction().
        ].
    ]
    
    doWhile
    [
        while (T<bool>(self))
        [
        ].
    ]        
}

public extension<Func> blockOp
{
    repeatUntil:aCondition
    [
        self().
        until (T<bool>(aCondition))
        [
            self().
        ].
    ]        
}

//lazy_func extension lazyOp
//{
//    whileTrue func:action
//    [
//        while(self)
//        [
//            action eval
//        ]
//    ]
//
//    whileFalse func:action
//    [
//        until(self)
//        [
//            action eval
//        ]
//    ]        
//}

// --- testOp ---

public extension testOp
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

// --- Assert ---

class AssertException :: Exception.

public singleton Assert
{
    ifEqual(object expr, object val)
    [
        ifnot(val == expr)
        [
            AssertException new("Assertion failed"); raise
        ]
    ]
    
    notFailed(Func f)
    [
        (f())
            | if(:e)
            [
                console writeLine:(e message).
                
                AssertException new("Assertion failed"); raise
            ].
    ]
    
    ifTrue : b
    [
        ifnot(b)
        [
            AssertException new("Assertion failed"); raise
        ]
    ]
    
    ifFalse : b
    [
        if(b)
        [
            AssertException new("Assertion failed"); raise
        ]
    ]
}

// --- controlOp ---

public extension controlOp
{
    saveTo:o
    [
        o write:self.
        
        ^ self.
    ]
    
    exchangeWith:v
    [
        var tmp := self value.
        self value := v value.
        v value := tmp.
    ]
}

public singleton control
{
    do:action
    [
        action().
    ]
}
