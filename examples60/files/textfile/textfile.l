import system'io;
import extensions;
import extensions'routines;

public program()
{
    if (Program_arguments.Length == 1)
        { Console.writeLine("Please provide the path to the file to view"); AbortException.raise() };

    File.assign(Program_arguments[1]).forEachLine(PrintingLn);
  
    Console.readChar() // wait for any key
}