#import system.
#import system'text.
#import extensions.
#import system'dynamic.

// --- IOException ---

#class(exceptionof:ScriptError) ScriptEngineException
{
    #constructor new
        <= %new:(scriptEngine $lastScriptError).
        
    #constructor new : aMessage
        <= %new &literal:(aMessage literal).
        
    #constructor new &literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]    
}

// --- scriptEngine ---

#symbol scriptEngine =
{
    #method(stacksafe)$lastScriptError
    [
        #var(byte:256)aBuffer.
        #var(int)aLength := system'external'elenasm GetStatus
            &bytearray:aBuffer
            &int:255.
            
        ^ UTF8Encoding toLiteral &int:0 &int:aLength &bytearray:aBuffer.  
    ]
    
    #method(stacksafe)$lastVMError
    [
        #var(dirty_ptr)err := system'external GetVMLastError.
            
        ^ err literal.  
    ]
    
    #method(stacksafe) evaluateTape &dirty_ptr:tape
        => system'core_routines'__vmtape.
    
    #method(stacksafe) evaluateScript &literal:aScript
    [
        #var(dirty_ptr)tape := system'external'elenasm InterpretScript
            &literal:aScript.
            
        (tape is &empty)
            ? [ #throw ScriptEngineException new. ].
            
        #var retVal := $self evaluateTape &dirty_ptr:tape.
        
        system'external'elenasm Release &dirty_ptr:tape.
        
        ($nil == retVal)
            ? [ #throw ScriptEngineException new:($self $lastVMError). ].
            
        ^ retVal.            
    ]

    #method(stacksafe) loadScriptFile &literal:path &int:anEncoding &int:anAutoDetect
    [
        #var(dirty_ptr)tape := system'external'elenasm InterpretFile
            &literal:path
            &int:anEncoding
            &int:anAutoDetect.
            
        (tape is &empty)
            ? [ #throw ScriptEngineException new. ].
            
        system'external'elenasm Release &dirty_ptr:tape.            
    ]
    
    #method(stacksafe) evalScriptFile &literal:path &int:anEncoding &int:anAutoDetect
    [
        #var(dirty_ptr)tape := system'external'elenasm InterpretFile
            &literal:path
            &int:anEncoding
            &int:anAutoDetect.
            
        (tape is &empty)
            ? [ #throw ScriptEngineException new. ].
            
        #var retVal := $self evaluateTape &dirty_ptr:tape.
        
        system'external'elenasm Release &dirty_ptr:tape.
        
        ($nil == retVal)
            ? [ #throw ScriptEngineException new:($self $lastVMError). ].
            
        ^ retVal.            
    ]
        
    evaluate : aScript
        = $self evaluateScript &literal:(aScript literal).
        
    load &path:aPath
    [
        $self loadScriptFile &literal:(aPath literal) &int:0 &int:-1.
    ]
        
    eval &path:aPath
        = $self evalScriptFile &literal:(aPath literal) &int:0 &int:-1.
}.

// --- globlas ---

#static globals = DynamicStruct new.