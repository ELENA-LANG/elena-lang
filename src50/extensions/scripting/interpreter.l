import system'text;
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

    buildTape(dirty_ptr tape)
    {
        var retVal := nil;
        byte temp[1024];        
        int len := tape;
        dirty_ptr current := tape + 4;
        int allocated := current;
        
        current := current + 4;
                       
        if (len <= 1024) {
            current.copyTo(temp, len);
            
            retVal := StreamTapeInterpreter.build(temp, len, allocated)
        }
        else {
            auto dump := new byte[](len);
            
            current.copyTo(dump, len);
            
            retVal := StreamTapeInterpreter.build(dump, len, allocated);
        };
                                                                                                            
        extern elenasm.Release(tape);
        
        ^ retVal
    }

    buildScriptFile(string path, int encoding, int autoDetect)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);

        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };

        ^ self.buildTape(tape)             
    }
                                
    buildScriptFile(string path)
        = self.buildScriptFile(path, 0, -1);

    buildScript(string script)
    {
        byte temp[256];        
        dirty_ptr tape := extern elenasm.InterpretScopeScript
                            (scopeId, script);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };

        ^ self.buildTape(tape)             
    }       

    string loadScriptFileAsText(string path, int encoding, int autoDetect)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };

        auto retVal := cast string(tape);    
                                    
        extern elenasm.Release(tape);
        
        ^ retVal
    }
    
    string evaluateScriptAsText(string script)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeScript
                            (scopeId, script);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };

        auto retVal := cast string(tape);    
                                    
        extern elenasm.Release(tape);
        
        ^ retVal
    }
        
    string loadPathAsText(string path)
    {
        ^ self.loadScriptFileAsText(path, 0, -1)
    }
    
    string loadScriptAsText(string script)
        <= evaluateScriptAsText(script);
    
    loadScript(string script)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeScript
                            (scopeId, script);
            
        if (tape.isEmpty())
            {   ScriptEngineException.new(self.lastScriptError()).raise() };
            
        extern elenasm.Release(tape)
    }
    
    loadScriptFile(string path, int encoding, int autoDetect)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);
            
        if (tape.isEmpty())
            {   ScriptEngineException.new(self.lastScriptError()).raise() };
            
        extern elenasm.Release(tape)
    }
    
    loadPath(string path)
    {
        self.loadScriptFile(path, 0, -1)
    }
}

// --- globlas ---

static globals = system'collections'Dictionary.new();