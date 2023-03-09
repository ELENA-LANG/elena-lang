import system'text;

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
      _scopeId := extern elenasm.NewScope()
   }

   internal string lastScriptError()
   {
      byte buffer[256];
      int  length := extern elenasm.GetStatus(buffer, 255);
            
      ^ UTF8Encoding.toString(0, length, buffer);
   }
    
   private loadScriptFile(string path, int encoding, int autoDetect)
   {
      pointer ptr := extern elenasm.InterpretScopeFile
                            (_scopeId, path, encoding, autoDetect);

      if (ptr.isUnassigned())
         {   ScriptEngineException.raise(self.lastScriptError()) };
      
      extern elenasm.Release(ptr)
   }

   loadPath(string path)
   {
      self.loadScriptFile(path, 0, -1);
   }
}