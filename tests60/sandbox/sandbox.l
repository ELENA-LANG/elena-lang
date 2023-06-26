import extensions;
import extensions'scripting;
   
public program()
{
   console.writeLine("Test");

    var t := new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\tscript60.es");
            
   // var o := t.buildScript("public program() { console.writeLine(""Hello World"") }");

   var o := t.buildScript("printMe(o) { system'console.writeLine(o) }").compile();
    
   var f := o.printMe();

   f(2);
}
