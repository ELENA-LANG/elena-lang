import extensions;
import extensions'scripting;

public program()
{
    if (program_arguments.Length == 1)
        { console.printLine("Please provide the path to the file to interpret"); AbortException.raise() };

    try
    {
        var o := jscript.evalPath(program_arguments[1]).program;
        
        o.main();
    }
    catch(Exception e)
    {
        console.printLine:e
    }
}

public stat_program()
{
   forward jsprogram.main()
}