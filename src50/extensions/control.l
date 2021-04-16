//
//public extension loopOp
//{
//    doWhile(action)
//    {
//        while (cast bool(self))
//        {
//            action()
//        }
//    }
//    
//    doUntil(action)
//    {
//        until (cast bool(self))
//        {
//            action()
//        }
//    }
//    
//    doWhile()
//    {
//        while (cast bool(self))
//        {
//        }
//    }
//}
//
//// --- testOp ---
//
//public extension testOp
//{
//    assertTrue()
//        = Assert.ifTrue(self);
//    
//    assertFalse()
//        = Assert.ifFalse(self);
//}

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
    
    notFailed(Func f)
    {
        f() \\ 
            on:(e)
            {
                AssertException.raise("Assertion failed")
            };
    }
    
    ifFailed(Func f)
    {
        var failed := false;
        f() \\ 
            on:(e)
            {
                failed := true;
            };
            
        Assert.ifTrue(failed);
    }

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

//// --- controlOp ---
//
//public extension controlOp
//{
//    saveTo(o)
//    {
//        o.write:self;
//        
//        ^ self
//    }
//}
//
//public exchange(ref object l, ref object r)
//{
//    var tmp := l;
//    l := r;
//    r := tmp
//}
//
//public singleton control
//{
//    do(action)
//    {
//        ^ action()
//    }
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
