import extensions;
import extensions'scripting;
   
public program()
{
    var t := new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\lscript60.es");
            
    var o := t.buildScript("import extensions; public program() { console.printLine(""Hello World"") }");
    
    o.eval();
}
