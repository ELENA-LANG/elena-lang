import system'text;
import system'dynamic;

// --- IOException ---

public class ScriptEngineException : Exception
{
    constructor new(string message)
        <= super new(message);   
}

// --- scriptEngine ---

public sealed class ScriptEngine
{
   int _scopeId;

   constructor()
   {
      _scopeId := extern elenasm.NewScopeSMLA()
   }

   internal string lastScriptError()
   {
      byte buffer[256];
      int  length := extern elenasm.GetStatusSMLA(buffer, 255);
            
      ^ UTF8Encoding.toString(0, length, buffer);
   }

    
   string evaluateScriptAsText(string script)
   {
      pointer tape := extern elenasm.InterpretScopeScriptSMLA
                            (_scopeId, script);
            
      if (tape.isUnassigned())
         {   ScriptEngineException.raise(self.lastScriptError()) };

      auto retVal := cast string(tape);    

      extern elenasm.ReleaseSMLA(tape);

      ^ retVal
   }
    
   private loadScriptFile(string path, int encoding, int autoDetect)
   {
      pointer ptr := extern elenasm.InterpretScopeFileSMLA
                            (_scopeId, path, encoding, autoDetect);

      if (ptr.isUnassigned())
         {   ScriptEngineException.raise(self.lastScriptError()) };
      
      extern elenasm.ReleaseSMLA(ptr)
   }

   buildScriptFile(string path, int encoding, int autoDetect)
   {
      pointer ptr := extern elenasm.InterpretScopeFileSMLA
                            (_scopeId, path, encoding, autoDetect);

      if (ptr.isUnassigned())
         {   ScriptEngineException.raise(self.lastScriptError()) };

      ^ self.buildTape(ptr)
   }

   private buildTape(pointer tape)
   {
      var retVal := nil;
      byte temp[1024];        
      int len := extern elenasm.GetLengthSMLA(tape);

      if (len <= 1024) {
         tape.copyTo(temp, len);
            
         retVal := StreamTapeInterpreter.build(temp, len)
      }
      else {
         auto dump := new byte[](len);
            
         tape.copyTo(dump, len);
            
         retVal := StreamTapeInterpreter.build(dump, len);
      };
                                                                                                            
      extern elenasm.ReleaseSMLA(tape);
        
      ^ retVal
   }

   string loadScriptAsText(string script)
      <= evaluateScriptAsText(script);

   loadPath(string path)
   {
      self.loadScriptFile(path, 0, -1);
   }

   buildScriptFile(string path)
      = self.buildScriptFile(path, 0, -1);

   buildScript(string script)
   {
      pointer tape := extern elenasm.InterpretScopeScriptSMLA
                            (_scopeId, script);
            
      if (tape.isUnassigned())
         {   ScriptEngineException.raise(self.lastScriptError()) };


      ^ self.buildTape(tape)
   }
}