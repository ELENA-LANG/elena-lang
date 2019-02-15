import extensions;
import extensions'dynamic;
import extensions'scripting;

public program()
{
    if (program_arguments.Length == 1)
        { console.printLine("Please provide the path to the file to interpret"); AbortException.raise() };

    try
    {
        script.loadAndEval("grammar_js.es", program_arguments[1])
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