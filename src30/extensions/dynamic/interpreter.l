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
}.