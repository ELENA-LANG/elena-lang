import extensions;
import extensions'scripting;

preloaded subjects = new::(subjconst program, subjconst fibonacci, subjconst main);


public program()
{
//    if (program_arguments.Length == 1)
//        { console.printLine("Please provide the path to the file to interpret"); AbortException.raise() };

    try
    {
        var o := jscript.evalPath(/*program_arguments[1]*/"fibonacci.js").program;
        
        o.main();
        
        //jscript.evalPath(program_arguments[1])
    }
    catch(Exception e)
    {
        console.printLine:e
    }
}

public stat_program()
{
   console.writeLine:"Javascript code executed"
}