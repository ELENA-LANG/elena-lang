import system'text;
import extensions;
import system'dynamic;
import system'collections;

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
    
    // obsolete
    evaluateTape(dirty_ptr tape)
        => system'core_routines'__vmtape;
    
    // obsolete
    eval(script)
        = self.evaluateScript(script);
        
    // obsolete
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

    // obsolete
    evalScriptFile(string path, int encoding, int autoDetect)
    {
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };
            
        var retVal := self.evaluateTape(tape);
        
        extern elenasm.Release(tape);
        
        if(nil == retVal)
            { ScriptEngineException.new(self.lastVMError()).raise() };
            
        ^ retVal
    }
        
    // obsolete
    evalPath(string path)
        = self.evalScriptFile(path, 0, -1);

    buildScriptFile(string path)
        = self.buildScriptFile(path, 0, -1);

    buildScriptFile(string path, int encoding, int autoDetect)
    {
        byte temp[256];        
        dirty_ptr tape := extern elenasm.InterpretScopeFile
                            (scopeId, path, encoding, autoDetect);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };

        dirty_ptr current := tape;
        int len := current;
        auto result := new ArrayList();
        while (len != 0) {
            current := current + 4;
            
            if (len < 100h) {                
                current.copyTo(temp, len);
                
                var item := StreamTapeInterpreter.build(temp, 0, len);
                result.append(item)                
            }
            else {
                auto dump := new byte[](len);
                
                current.copyTo(dump, len);
                
                var item := StreamTapeInterpreter.build(dump, 0, len);
                result.append(item)
            };
            
            current := current + len;
            len := current;
        };                                            
                                                                                                            
        extern elenasm.Release(tape);
        
        ^ result.Value
    }

    buildScript(string script)
    {
        byte temp[256];        
        dirty_ptr tape := extern elenasm.InterpretScopeScript
                            (scopeId, script);
            
        if (tape.isEmpty())
            { ScriptEngineException.new(self.lastScriptError()).raise() };

        dirty_ptr current := tape;
        int len := current;
        auto result := new ArrayList();
        while (len != 0) {
            current := current + 4;
            
            if (len < 100h) {                
                current.copyTo(temp, len);
                
                var item := StreamTapeInterpreter.build(temp, 0, len);
                result.append(item)                
            }
            else {
                auto dump := new byte[](len);
                
                current.copyTo(dump, len);
                
                var item := StreamTapeInterpreter.build(dump, 0, len);
                result.append(item)
            };
            
            current := current + len;
            len := current;
        };                                            
                                                                                                            
        extern elenasm.Release(tape);
        
        ^ result.Value
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

static globals = new system'collections'Dictionary();