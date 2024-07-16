import extensions;
import extensions'scripting;

public program()
{
   var engine := new ScriptEngine();
   engine.loadPath("grammar.es");

   var script := "1 2 3 4 5";

   console.printLine(script,"=>",engine.loadScriptAsText(script));
}