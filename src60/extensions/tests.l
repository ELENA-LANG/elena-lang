// --- Assert ---

public class AssertException : Exception
{
    constructor new(string message)
        <= super new(message);
}

public singleton Assert
{
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
