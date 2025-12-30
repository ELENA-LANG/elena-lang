import system'io;
import extensions;
 
public Program()
{
    if (Program_arguments.Length != 3)
        { Console.printLine("Please provide the path to the file and a new length"); AbortException.raise() };
 
    auto file := File.assign(program_arguments[1]);
    var length := Program_arguments[2].toInt();
 
    if:not(file.Available)
        { Console.printLine("File ",file," does not exist"); AbortException.raise() };
 
    file.Length := length
}