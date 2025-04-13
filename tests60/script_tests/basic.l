import extensions;
import extensions'scripting;
import ltests;

public evalTest() : testCase()
{
   var ret := lscript.interpretLine("""Hello "".add(""World"")");

   Assert.ifEqual(ret, "Hello World");

   Console.writeLine(".");
}


public extensionCallTest() : testCase()
{
    Console.print("Calling an extension in evaluation mode:");

    var t := new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\lscript60.es");
            
    var o := t.buildScript("import extensions; public program() { Console.printLine(""Done"") }");
    
    o.eval();
}