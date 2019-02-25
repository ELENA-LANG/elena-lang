import extensions;
import extensions'scripting;

public program()
{
    escript.eval("system'console.writeLine(""Hello World"");");
    
    var o := escript.eval(
       "^ { eval(x) { ^extensions'math'mathControl.power(x, 2 ) }}");

    console.printLine(o.eval(2));        
    
    var o2 := escript.eval(
       "^ { eval(x,y) { ^x.add(y) }}");

    console.printLine(o2.eval(2,3));        
}