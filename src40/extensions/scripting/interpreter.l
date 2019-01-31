import system'text;
import extensions;
import system'dynamic;

// --- IOException ---

public class ScriptEngineException : Exception
{
    constructor new(string l)
        <= new(l);
}

// --- scriptEngine ---

public class ScriptEngine
{
    int scopeId;
    
    constructor()
    {
        scopeId := extern elenasm.NewScope()
    }
    
    internal lastScriptError()
    {
        byte buffer[256];
        int  length := extern elenasm.GetStatus(buffer, 255);
            
        ^ UTF8Encoding.toString(0, length, buffer);
    }
    
    internal lastVMError()
    {
        dirty_ptr err := extern elenart.GetVMLastError();
            
        ^ cast string(err)  
    }
    
    evaluateTape(dirty_ptr tape)
        => system'core_routines'__vmtape;
    
    evaluateScript(string script)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeScript
                                (scopeId, script);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };
            
        var retVal := self.evaluateTape(tape);
        
        extern elenasm.Release(tape);
        
        if(nil == retVal)
            { ScriptEngineException.new(self.lastVMError()).raise() };
            
        ^ retVal
    }

    loadScriptFile(string path, int encoding, int autoDetect)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);
            
        if (tape.isEmpty())
            { ScriptEngineException new(self.lastScriptError()).raise() };
            
        extern elenasm.Release(tape)
    }
    
    evalScriptFile(string path, int encoding, int autoDetect)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);
            
        if (tape.isEmpty())
            { ScriptEngineException new(self.lastScriptError()).raise() };
            
        var retVal := self.evaluateTape(tape);
        
        extern elenasm.Release(tape);
        
        if(nil == retVal)
            { ScriptEngineException.new(self.lastVMError()).raise() };
            
        ^ retVal
    }
        
    evaluate(script)
        = self.evaluateScript(script);
        
    loadPath(string path)
    {
        self.loadScriptFile(path, 0, -1)
    }
        
    evalPath(string path)
        = self.evalScriptFile(path, 0, -1);
}

// --- globlas ---

static globals = new system'collections'Dictionary();