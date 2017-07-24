import system'text.
import extensions.
import system'dynamic.

// --- IOException ---

class ScriptEngineException :: exceptionof(ScriptError)
{
    constructor new
        <= new:(scriptEngine $lastScriptError).
        
    constructor new : aMessage
        <= new literal:(aMessage literal).
        
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]    
}

// --- scriptEngine ---

class scriptEngine =
{
    stacksafe $lastScriptError
    [
        byte aBuffer(256).
        int  aLength := system'external'elenasm GetStatus
                                bytearray:aBuffer
                                int:255.
            
        ^ UTF8Encoding toLiteral int:0 int:aLength bytearray:aBuffer
    ]
    
    stacksafe $lastVMError
    [
        dirty_ptr err := system'external GetVMLastError.
            
        ^ err literal.  
    ]
    
    stacksafe evaluateTape dirty_ptr:tape
        => system'core_routines'__vmtape.
    
    stacksafe evaluateScript literal:aScript
    [
        dirty_ptr tape := system'external'elenasm InterpretScript
                                literal:aScript.
            
        if (tape is empty)
            [ ScriptEngineException new; raise ].
            
        var retVal := $self evaluateTape dirty_ptr:tape.
        
        system'external'elenasm Release dirty_ptr:tape.
        
        if($nil == retVal)
            [ ScriptEngineException new:($self $lastVMError); raise ].
            
        ^ retVal
    ]

    stacksafe loadScriptFile literal:path int:anEncoding int:anAutoDetect
    [
        dirty_ptr tape := system'external'elenasm InterpretFile
                            literal:path
                            int:anEncoding
                            int:anAutoDetect.
            
        if (tape is empty)
            [ ScriptEngineException new; raise ].
            
        system'external'elenasm Release dirty_ptr:tape.            
    ]
    
    stacksafe evalScriptFile literal:path int:anEncoding int:anAutoDetect
    [
        dirty_ptr tape := system'external'elenasm InterpretFile
                            literal:path
                            int:anEncoding
                            int:anAutoDetect.
            
        if (tape is empty)
            [ ScriptEngineException new; raise ].
            
        var retVal := $self evaluateTape dirty_ptr:tape.
        
        system'external'elenasm Release dirty_ptr:tape.
        
        if ($nil == retVal)
            [ ScriptEngineException new:($self $lastVMError); raise ].
            
        ^ retVal
    ]
        
    evaluate : aScript
        = $self evaluateScript literal:aScript.
        
    load path:aPath
    [
        $self loadScriptFile literal:aPath int:0 int:-1.
    ]
        
    eval path:aPath
        = $self evalScriptFile literal:aPath int:0 int:-1.
}.

// --- globlas ---

static globals = DynamicStruct new.