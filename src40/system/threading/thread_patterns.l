// --- lock statement

public lock<block>(expr)
{
    auto v := expr;
    
    try
    {
        v.enterCriticalSection();
        
        block;
    }
    finally
    {
        v.leaveCriticalSection()
    }
}
