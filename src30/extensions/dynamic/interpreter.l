// === ver 3.4.1 ===

#import system.
#import system'text.
#import extensions.
#import system'dynamic.

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
    [
        #var(type:int)retVal := system'external'elenasm EvaluateScript
            &literal:aScript.
            
        (retVal == 0)
            ? [ #throw ScriptEngineException new. ].
    ]

    loadScriptFile &literal:aPath &int:anEncoding &int:anAutoDetect
    [
        #var(type:int)retVal := system'external'elenasm EvaluateFile
            &literal:aPath
            &int:anEncoding
            &int:anAutoDetect.
            
        (retVal == 0)
            ? [ #throw ScriptEngineException new. ].
    ]
        
    evaluate : aScript
        = $self evaluateScript &literal:(aScript literal).
        
    evaluate &path:aPath
        = $self loadScriptFile &literal:(aPath literal) &int:0 &int:-1.
        
    load &path:aPath
    [
        $self loadScriptFile &literal:(aPath literal) &int:0 &int:-1.
    ]
}.

// --- globlas ---

#static globals = DynamicStruct new.