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
    
    #method(stacksafe)$lastVMError
    [
        #var(type:dirty_ptr)err := system'external GetVMLastError.
            
        ^ err literal.  
    ]
    
    #method(stacksafe) evaluateTape &dirty_ptr:tape
        => system'core_routines'evaluate_tape.
    
    #method(stacksafe) evaluateScript &literal:aScript
    [
        #var(type:dirty_ptr)tape := system'external'elenasm InterpretScript
            &literal:aScript.
            
        (tape == 0)
            ? [ #throw ScriptEngineException new. ].
            
        #var retVal := $self evaluateTape &dirty_ptr:tape.
        
        system'external'elenasm Release &dirty_ptr:tape.
        
        ($nil == retVal)
            ? [ #throw ScriptEngineException new:($self $lastVMError). ].
            
        ^ retVal.            
    ]

    #method(stacksafe) loadScriptFile &literal:path &int:anEncoding &int:anAutoDetect
    [
        #var(type:dirty_ptr)tape := system'external'elenasm InterpretFile
            &literal:path
            &int:anEncoding
            &int:anAutoDetect.
            
        (tape == 0)
            ? [ #throw ScriptEngineException new. ].
            
        system'external'elenasm Release &dirty_ptr:tape.            
    ]
        
    evaluate : aScript
        = $self evaluateScript &literal:(aScript literal).
        
    load &path:aPath
    [
        $self loadScriptFile &literal:(aPath literal) &int:0 &int:-1.
    ]
}.

// --- globlas ---

#static globals = DynamicStruct new.