// --- Assert ---

public class AssertException : Exception
{
    constructor new(string message)
        <= super new(message);
    
    static raise(string message)
    {
        AssertException.new(message).raise()
    }    
}

public singleton Assert
{
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
