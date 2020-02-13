//import system'routines.
//
//public extension<int> intLoopOp
//{
//    runIntLoop(int anEnd, Func<int,object> aFunction)
//    [
//       int aStart := self.
//
//       if (aStart < anEnd)
//            [ ^ aFunction run(aStart, anEnd, 1) ];
//            [ ^ aFunction rewind(aStart, anEnd, 1) ]
//    ]
//
//    runIntLoop(int anEnd, int aStep, Func<int,object> aFunction)
//    [
//       int aStart := self.
//
//       if (aStep > 0)
//            [ ^ aFunction run(aStart, anEnd, aStep) ];
//            [ ^ aFunction rewind(aStart, anEnd, aStep) ]
//    ]
//
//    to(int anEnd) do(Func<int,object> aFunction)
//        = self~intLoopOp runIntLoop(anEnd, aFunction).
//            
//    till(int anEnd) do(Func<int,object> aFunction)
//        = self~intLoopOp runIntLoop(anEnd-1, aFunction).
//            
//    to(int anEnd) do(Func1 aFunction)
//        = self~intLoopOp runIntLoop(anEnd, (:n)<int>[ aFunction(n) ]).
//            
//    till(int anEnd) do(Func1 aFunction)
//        = self~intLoopOp runIntLoop(anEnd-1, (:n)<int>[ aFunction(n) ]).
//            
////    stacksafe to(int anEnd) do(ext_mssg anExtension)
////        = self~intLoopOp runIntLoop int:anEnd nfunc(:n)<int>[ n~anExtension get ].
////            
////    multi to:anEnd do:anAction
////        = self~intLoopOp runIntLoop int(anEnd int) nfunc(:n)<int>[ anAction eval:n ].
////        
////    multi till:anEnd do:anAction
////        = self~intLoopOp runIntLoop int(anEnd int - 1) nfunc(:n)<int>[ anAction eval:n ].
//                
//    to(int anEnd) by(int aStep) do(Func<int,object> aFunction)
//        = self~intLoopOp runIntLoop(anEnd, aStep, aFunction).
//            
////    stacksafe till(int anEnd) by(int aStep) do(nfunc aFunction)
////        = self~intLoopOp runIntLoop int(anEnd-1) int:aStep nfunc:aFunction.
//            
//    to(int anEnd) by(int aStep) do(Func1 aFunction)
//        = self~intLoopOp runIntLoop(anEnd, aStep, (:n)<int>[ aFunction(n) ]).
//            
////    stacksafe to(int anEnd) by(int aStep) do(ext_mssg anExtension)
////        = self~intLoopOp runIntLoop int:anEnd int:aStep nfunc(:n)<int>[ n~anExtension get ].
////            
////    multi to:anEnd by:aStep do:anAction
////        = self~intLoopOp runIntLoop int(anEnd int) int:aStep nfunc(:n)<int>[ anAction eval:n ].
////        
////    multi till:anEnd by:aStep do:anAction
////        = self~intLoopOp runIntLoop int(anEnd int - 1) int:aStep nfunc(:n)<int>[ anAction eval:n ].
////        
//}
//
//public extension forLoopOp
//{
//    to(object aMaxValue) do(Func1 aFunction)
//        = aFunction run(RangeEnumerator from:self to:aMaxValue).
//
////    to(object aMaxValue) do(ExtensionMessage aMessage)
////        = self~forLoopOp to:aMaxValue do(:n)[ n~aMessage get ].
////
////    multi to:aMaxValue do:anAction
////        = self~forLoopOp to:aMaxValue do(:n)[ anAction eval:n ].
//
//    to(object aMaxValue) by(object aStep) do(Func1 aFunction)
//        = aFunction run(RangeEnumerator from:self to:aMaxValue by:aStep).
//
////    to(object aMaxValue) by(object aStep) do(ExtensionMessage aMessage)
////        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ n~aMessage get ].
////
////    multi to:aMaxValue by:aStep do:anAction
////        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ anAction eval:n ].
//
//    till(object aMaxValue) do(Func1 aFunction)
//        = aFunction run(IntervalEnumerator from:self till:aMaxValue).
//
////    till(object aMaxValue) do(ExtensionMessage aMessage)
////        = self~forLoopOp to:aMaxValue do(:n)[ n~aMessage get ].
////
////    multi till:aMaxValue do:anAction
////        = self~forLoopOp to:aMaxValue do(:n)[ anAction eval:n ].
//
////    till(int aMaxValue) do(nfunc aFunction)
////        = aFunction run enumerator:(IntervalEnumerator new from:self till:aMaxValue by:aStep).
//
//    till(object aMaxValue) by(object aStep) do(Func1 aFunction)
//        = aFunction run(IntervalEnumerator from:self till:aMaxValue by:aStep).
//
////    till(object aMaxValue) by(object aStep) do(ExtensionMessage aMessage)
////        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ n~aMessage get ].
////
////    multi till:aMaxValue by:aStep do:anAction
////        = self~forLoopOp to:aMaxValue by:aStep do(:n)[ anAction eval:n ].
//
//    to(object maxValue) seek(Func1 func)
//        = func retrieve(RangeEnumerator from:self to:maxValue).
//        
//    till(object maxValue) seek(Func1 func)
//        = func retrieve(IntervalEnumerator from:self till:maxValue).
//        
//    to(object maxValue) by(object step) seek(Func1 func)
//        = func retrieve(RangeEnumerator from:self to:maxValue by:step).
//}

public extension loopOp
{
    doWhile(action)
    {
        while (cast bool(self))
        {
            action()
        }
    }
    
    doUntil(action)
    {
        until (cast bool(self))
        {
            action()
        }
    }
    
    doWhile()
    {
        while (cast bool(self))
        {
        }
    }
}

//public extension<Func> blockOp
//{
//    repeatUntil:aCondition
//    [
//        self().
//        until (T<bool>(aCondition))
//        [
//            self().
//        ].
//    ]        
//    
//    repeatWhile:aCondition
//    [
//        self().
//        while (T<bool>(aCondition))
//        [
//            self().
//        ].
//    ]         
//}
//
////lazy_func extension lazyOp
////{
////    whileTrue func:action
////    [
////        while(self)
////        [
////            action eval
////        ]
////    ]
////
////    whileFalse func:action
////    [
////        until(self)
////        [
////            action eval
////        ]
////    ]        
////}

// --- testOp ---

public extension testOp
{
    assertTrue()
        = Assert.ifTrue(self);
    
    assertFalse()
        = Assert.ifFalse(self);
}

// --- Assert ---

public class AssertException : Exception
{
    constructor new(string message)
        <= new(message);
    
    static raise(string message)
    {
        AssertException.new(message).raise()
    }    
}

public singleton Assert
{
    ifEqual(object expr, object val)
    {
        ifnot(val == expr)
        {
            AssertException.raise("Assertion failed")
        }
    }
    
//    notFailed(Func f)
//    [
//        (f())
//            | if(:e)
//            [
//                console writeLine:(e message).
//                
//                AssertException new("Assertion failed"); raise
//            ].
//    ]
    
    ifTrue(b)
    {
        ifnot(b)
        {
            AssertException.raise("Assertion failed")
        }
    }
    
    ifFalse(b)
    {
        if(b)
        {
            AssertException.raise("Assertion failed")
        }
    }
}

// --- controlOp ---

public extension controlOp
{
    saveTo(o)
    {
        o.write:self;
        
        ^ self
    }
}

public exchange(ref object l, ref object r)
{
    var tmp := l;
    l := r;
    r := tmp
}

//public singleton control
//{
//    do:action
//    [
//        action().
//    ]
//}

// --- invoker ---

public extension invokeOp : MessageName
{
    getPropertyValue(target)
    {
        Message mssg := self.getPropertyMessage();
        
        ^ mssg(target)
    
    }
    setPropertyValue(target,value)
    {
        Message mssg := self.setPropertyMessage();
        
        mssg(target, value)
    }
}
