import extensions;
import extensions'scripting;
import system'dynamic'expressions;

public program()
{
    if (program_arguments.Length == 1)
        { console.printLine("Please provide the path to the file to interpret"); AbortException.raise() };

    lscript.interpretPath(program_arguments[1]);
}