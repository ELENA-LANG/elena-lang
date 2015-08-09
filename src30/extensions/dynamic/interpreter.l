#define system.
#define system'text.
#define extensions.
//#define system'dynamic.

// --- IOException ---

#class ScriptEngineException :: Exception
{
    #constructor new
        <= %new:(scriptEngine $lastScriptError).
        
    #constructor new : aMessage
        <= %new &literal:(aMessage literal) &subject: %ScriptError.
}

// --- scriptEngine ---

#symbol scriptEngine =
{
    #method(stacksafe)$lastScriptError
    [
        #var(type:bytearray,size:256)aBuffer.
        #var(type:int)aLength := system'external'elenasm GetStatus
            &bytearray:aBuffer
            &int:255.
            
        ^ UTF8Encoding toLiteral &int:0 &int:aLength &bytearray:aBuffer.  
    ]
    
    evaluateScript &literal:aScript
        => system'internal'evaluate.

    loadScriptFile &literal:aPath &int:anEncoding &int:anAutoDetect
        => system'internal'load.
        
    evaluate : aScript
        = $self evaluateScript &literal:(aScript literal).
        
    evaluate &path:aPath
        = $self loadScriptFile &literal:(aPath literal) &int:0 &int:-1.
        
    load &path:aPath
    [
        $self loadScriptFile &literal:(aPath literal) &int:0 &int:-1.
    ]
}.

//// --- globlas ---
//
//#static globals = DynamicStruct new.