import extensions;
import extensions'scripting;

public program()
{
   var engine := new ScriptEngine();
   engine.loadPath("grammar.es");

   var script := "<Form :Name=""MainWindow""></Form>";

   console.printLine(engine.loadScriptAsText(script));
}