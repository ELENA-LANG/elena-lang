import extensions;
import extensions'scripting;
import ltests;

public evalTest() : testCase()
{
   var ret := lscript.interpretLine("""Hello "".add(""World"")");

   Assert.ifEqual(ret, "Hello World");

   console.write(".");
}
