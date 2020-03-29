// --- lock statement

#declare lock<expr>(block)
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
