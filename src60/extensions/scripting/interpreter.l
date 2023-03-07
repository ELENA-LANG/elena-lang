// --- scriptEngine ---

public sealed class ScriptEngine
{
   int _scopeId;

   constructor()
   {
      _scopeId := extern elenasm.NewScope()
   }

   private loadScriptFile(string path, int encoding, int autoDetect)
   {
      pointer ptr := extern elenasm.InterpretScopeFile
                            (_scopeId, path, encoding, autoDetect);
   }

   loadPath(string path)
   {
      self.loadScriptFile(path, 0, -1);
   }
}