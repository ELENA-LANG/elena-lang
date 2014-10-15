#define system.

// --- ScriptEngineException --

#class(type:ScriptFailed)ScriptEngineException : Exception
{
    #constructor new
    [
        theMessage := "A Script engine error".
    ]    
}

// --- scriptEngine ---

#symbol scriptEngine =
{
    evaluate &literal:aScript
        <evaluate>.
        
    load &path:aPath
        <= %loadScript &literal::aPath &int:0 &int:-1.
        
    loadScript &literal:aLiteral &int:anEncoding &int:anAutoDetect
        <load>.
}.