import extensions;
import extensions'scripting;

public program()
{
    if (program_arguments.Length == 1)
        { console.printLine("Please provide the path to the file to interpret"); AbortException.raise() };

    tscript.interpretPath(program_arguments[1]);
}