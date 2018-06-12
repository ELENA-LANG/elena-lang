import system'text.
import extensions.
import system'dynamic.

// --- IOException ---

public class ScriptEngineException :: Exception
{
    constructor new(literal l)
        <= new(l).
}

// --- scriptEngine ---

public class ScriptEngine
{
    int scopeId.
    
    constructor new
    [
        scopeId := system'external'elenasm NewScope.
    ]
    
    internal stacksafe lastScriptError
    [
        V<byte,256> aBuffer.
        int         aLength := system'external'elenasm GetStatus
                                    (T<ByteArray>(aBuffer), 255).
            
        ^ UTF8Encoding toLiteral(0, aLength, T<ByteArray>(aBuffer))
    ]
    
    internal stacksafe lastVMError
    [
        dirty_ptr err := system'external GetVMLastError.
            
        ^ err literal.  
    ]
    
    stacksafe evaluateTape(dirty_ptr tape)
        => system'core_routines'__vmtape.
    
    stacksafe evaluateScript(literal aScript)
    [
        dirty_ptr tape := system'external'elenasm InterpretScopeScript
                                (scopeId, aScript).
            
        if (tape isEmpty)
            [ ScriptEngineException new(self lastScriptError); raise ].
            
        var retVal := self evaluateTape(tape).
        
        system'external'elenasm Release(tape).
        
        if(nil == retVal)
            [ ScriptEngineException new(self lastVMError); raise ].
            
        ^ retVal
    ]

    stacksafe loadScriptFile(literal path, int anEncoding, int anAutoDetect)
    [
        dirty_ptr tape := system'external'elenasm InterpretScopeFile
                            (scopeId, path, anEncoding, anAutoDetect).
            
        if (tape isEmpty)
            [ ScriptEngineException new(self lastScriptError); raise ].
            
        system'external'elenasm Release(tape).
    ]
    
    stacksafe evalScriptFile(literal path, int anEncoding, int anAutoDetect)
    [
        dirty_ptr tape := system'external'elenasm InterpretScopeFile
                            (scopeId, path, anEncoding, anAutoDetect).
            
        if (tape isEmpty)
            [ ScriptEngineException new(self lastScriptError); raise ].
            
        var retVal := self evaluateTape(tape).
        
        system'external'elenasm Release(tape).
        
        if (nil == retVal)
            [ ScriptEngineException new(self lastVMError); raise ].
            
        ^ retVal
    ]
        
    evaluate : aScript
        = self evaluateScript(aScript).
        
    loadPath(literal aPath)
    [
        self loadScriptFile(aPath, 0, -1).
    ]
        
    evalPath(literal aPath)
        = self evalScriptFile(aPath, 0, -1).
}

// --- globlas ---

static globals = system'collections'Dictionary new.