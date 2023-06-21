import extensions;
import extensions'scripting;
   
public program()
{
    var t := new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\tscript60.es");
            
    var o := t.buildScript("public program() { console.writeLine(2) }");
    
    o.eval();
}
